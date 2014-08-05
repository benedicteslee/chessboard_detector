#include <vector>
#include <cmath>
#include <stdexcept>

#include "boarddetector.h"
#include "Line.h"
#include "cvutils.h"
#include "square.h"
#include "typedefs.h"
#include "board.h"
#include "squareExpander.h"
#include "remover.h"
#include "regression.h"

BoardDetector::~BoardDetector()
{
}

BoardDetector::BoardDetector(cv::Mat& image_, std::vector<Line> lines_)
{
    image = image_;

    if (image.channels() != 1){
        throw std::invalid_argument("Give me a grayscale image");
    }

    lines = lines_;
    //detectChessboardRegion(); // TODO Use template matching to find rough region of chessboard
    categorizeLines();
}

Lines BoardDetector::get_hlinesSorted()
{
    return hlinesSorted;
}

Lines BoardDetector::get_vlinesSorted()
{
    return vlinesSorted;
}

Corners BoardDetector::getCorners()
{
    return corners;
}

Board BoardDetector::detect(bool doDraw)
{
    Board possibleBoard = Board(image, hlinesSorted, vlinesSorted);
    //auto hlines = get_hlinesSorted();

    if (doDraw) possibleBoard.draw();
    cv::destroyAllWindows();
    //findVanishingPoint(); // use?
    //removeSpuriousLines(); // TODO Remove lines not belonging to the chessboard

    Remover remover(possibleBoard);

    filterBasedOnSquareSize(possibleBoard, remover);
    indices colreq1 = remover.getCurrentColRequests();
    indices rowreq1 = remover.getCurrentRowRequests();
    //remover.remove();
    //possibleBoard.draw();

    filterBasedOnRowType(possibleBoard, remover);
    indices colreq2 = remover.getCurrentColRequests();
    indices rowreq2 = remover.getCurrentRowRequests();
    //remover.remove();
    //possibleBoard.draw();

    filterBasedOnColType(possibleBoard, remover);
    indices colreq3 = remover.getCurrentColRequests();
    indices rowreq3 = remover.getCurrentRowRequests();

    remover.remove();
    if (doDraw) possibleBoard.draw();

    std::pair<int,int> status = possibleBoard.getStatus();

    bool addColumns = false;
    bool addRows = false;

    if (status.first > 0)
        addRows = true;

    while (addRows){
        requestRowExpansion(possibleBoard);
        status = possibleBoard.getStatus();
        if (doDraw) possibleBoard.draw();
        if (status.first <= 0)
            addRows = false;
    }

    if (status.second > 0) // cols are missing
        addColumns = true;

    while (addColumns){
        requestColumnExpansion(possibleBoard);
        status  = possibleBoard.getStatus();
        if (doDraw) possibleBoard.draw();
        if (status.second <= 0)
            addColumns = false;
    }

    //const Square& square = possibleBoard.getRef(0);
    //bool check = square.containsPoint(cv::Point2d(200,200));

    return possibleBoard;
}

void BoardDetector::categorizeLines(){
    for (size_t i=0; i<lines.size();i++)
    {
        if (lines[i].slope < 0.1 && lines[i].slope > -0.1) // TODO: make numbers dynamic based on iamge size!
        {
            hlinesIdx.push_back((int) i);
        }
        else
        {
            vlinesIdx.push_back((int) i);
        }
    }

    /////////// H O R I Z O N T A L   L I N E S ///////////
    // Sort horizontal lines
    std::vector<std::pair<int,double> > yints(hlinesIdx.size());

    std::pair<int,double> p;
    for (size_t i = 0; i < hlinesIdx.size(); ++i) {
        Line line = lines[hlinesIdx[i]];
        p.first = (int) i;
        p.second = (double) line.yIntercept;
        yints[i] = p;
    }

    std::sort(yints.begin(), yints.end(), cvutils::pairIsLess);

    // Remove duplicate horizontal lines
    std::vector<int> removeIdx1(yints.size(), 0);
    for (size_t i = 1; i < yints.size(); ++i) {
        double yint1 = yints[i-1].second;
        double yint2 = yints[i].second;
        double diff =  abs(yint1 - yint2);
        if (diff < 3)
            removeIdx1[i] = 1;
        else
            removeIdx1[i] = 0;
    }

    // remove false horizontal lines
    std::vector<double> slopes(hlinesIdx.size());
    for (size_t i = 0; i < hlinesIdx.size(); i++){
        slopes[i] = lines[hlinesIdx[i]].slope;
    }
    double meanNoOutliers = cvutils::meanNoOutliers(slopes);
    double tolerance = meanNoOutliers * 0.1;
    // Add sorted unique horizontal lines to field
    for (size_t i = 0; i < hlinesIdx.size(); ++i) {

        if (removeIdx1[i] == 0){
            Line line = lines.at(hlinesIdx[yints[i].first]);
            //if (std::abs(line.slope - meanNoOutliers) <= tolerance)
                hlinesSorted.push_back(line);
        }
    }

    /////////// V E R T I C A L   L I N E S ///////////
    // Sort vertical lines
    std::vector<std::pair<int,double> > xints(vlinesIdx.size());

    std::pair<int,double> p2;
    for (size_t i = 0; i < vlinesIdx.size(); ++i) {
        Line line = lines[vlinesIdx[i]];
        p2.first = (int) i;
        p2.second = (double) line.xlookup(0, 1);
        xints[i] = p2;
    }

    std::sort(xints.begin(), xints.end(), cvutils::pairIsLess);

    // Remove duplicate vertical lines
    std::vector<int> removeIdx2(xints.size());
    for (size_t i = 1; i < xints.size(); ++i) {
        double xint1 = xints[i-1].second;
        double xint2 = xints[i].second;
        double diff =  abs(xint1 - xint2);
        if (diff < 10)
            removeIdx2[i] = 1;
        else
            removeIdx2[i] = 0;
    }

    // Add sorted unique vertical lines to field
    for (size_t i = 0; i < vlinesIdx.size(); i++) {
        if (removeIdx2[i] == 0){
            int idx1 = xints[i].first;
            int idx2 = vlinesIdx[idx1];
            Line line = lines.at(idx2);
            vlinesSorted.push_back(line);
        }
    }
}

void BoardDetector::findVanishingPoint(){
    int numlines = (int) vlinesIdx.size();
    Points2d vpoints;
    vpoints.reserve(numlines*(numlines-1)/2);

    // Calculate intersection between each possible pair of lines
    // TODO: this should be gotten from findIntersections()
    for (int i=0;i<numlines;i++){
        int idx1 = vlinesIdx.at(i);
        Line line1 = lines.at(idx1);
        for (int j=i+1;j<numlines;j++){
            int idx2 = vlinesIdx.at(j);
            Line line2 = lines.at(idx2);
            cv::Point2d vpoint;
            line1.Intersection(line2, vpoint);
            vpoints.push_back(vpoint);
        }
    }

    cv::Point2d meanpoint = cvutils::centerpoint(vpoints);

    std::vector<double> dists(vpoints.size());
    std::vector<double> stdevs(vpoints.size());
    for (size_t i=0;i<vpoints.size();i++){
        dists[i] = cv::norm(vpoints[i] - meanpoint);
    }

    std::vector<double> distsSorted(dists);
    std::sort(distsSorted.begin(),distsSorted.end());
    int quantileSize = vpoints.size()/4;
    double quantile1 = distsSorted[quantileSize-1];
    double quantile2 = distsSorted[quantileSize*3 -1];

    Points2d voters;
    for (size_t i=0;i<vpoints.size();i++){
        if (dists[i] >= quantile1 && dists[i] <= quantile2){
            voters.push_back(vpoints[i]);
        }
    }
    vanishingPoint = cvutils::centerpoint(voters);
}

void BoardDetector::filterBasedOnSquareSize(Board &board, Remover &remover)
{
    size_t nCols = board.getNumCols();
    size_t nRows = board.getNumRows();

    std::vector<int> hlengths(nCols);
    std::vector<int> vlengths(nRows);

    // Flag outliers based on horizontal lengths
    std::vector<size_t> houtliers(nCols,0);
    for (size_t row = 0; row < nRows; row++){
        Squares squares = board.getRow(row);
        for (size_t col = 0; col < squares.size(); col++){
            hlengths.at(col) = squares.at(col).getHLength();
        }
        houtliers = cvutils::flagOutliers(hlengths);
        remover.addToRow(row, houtliers);
    }

    // TODO BETTER METHOD FOR CORRECTING FOR VIEWPOINT EFFECT
    // Flag outliers based on vertical lengths
    for (size_t col = 0; col < board.getNumCols(); col++){
        Squares squares = board.getCol(col);
        for (size_t row = 0; row < squares.size(); row++){
            vlengths.at(row) = squares.at(row).getVLength();
        }

        //double midmean = cvutils::meanNoOutliers(vlengths);
        Regression<int> reg(vlengths);
        std::vector<double> errors = reg.squaredErrors();
        double meanerror = cv::mean(errors)[0];
        for (size_t row = 0; row < squares.size(); row++){
            bool doVote = errors.at(row) > meanerror;
            if (doVote)
                remover.addToElement(row, col, 1);
        }
    }
}

void BoardDetector::filterBasedOnRowType(Board& board, Remover& remover)
{
    std::vector<int> types = board.getRowTypes();

    std::vector<size_t> votes(board.getNumCols());
    std::for_each(votes.begin(), votes.end(), [](size_t& v){v = 1;});
    for (size_t row = 0; row < types.size(); row++){
        if (types.at(row) == 0){
            remover.addToRow(row, votes);
        }
    }
}

void BoardDetector::filterBasedOnColType(Board& board, Remover& remover)
{
    std::vector<int> types = board.getColTypes();

    std::vector<size_t> votes(board.getNumRows());
    std::for_each(votes.begin(), votes.end(), [](size_t& v){v = 1;});
    for (size_t col = 0; col < types.size(); col++){
        if (types.at(col) == 0){
            remover.addToCol(col, votes);
        }
    }

}

void BoardDetector::requestColumnExpansion(Board& board)
{
    Squares left = board.getCol(0);
    Squares right = board.getCol(board.getNumCols()-1);

    std::vector<int> leftTypes = Square::getSquareTypes(left);
    std::vector<int> rightTypes = Square::getSquareTypes(right);

    int sumLeft = cv::sum(leftTypes)[0];
    int sumRight = cv::sum(rightTypes)[0];

    Direction dir = LEFT;
    if (sumLeft < sumRight)
        dir = RIGHT;

    board.expand(dir);
}

void BoardDetector::requestRowExpansion(Board &board)
{
    Squares top = board.getRow(0);
    Squares bottom = board.getRow(board.getNumRows()-1);

    std::vector<int> topTypes = Square::getSquareTypes(top);
    std::vector<int> bottomTypes = Square::getSquareTypes(bottom);

    int sumTop = cv::sum(topTypes)[0];
    int sumBottom = cv::sum(bottomTypes)[0];

    Direction dir = UP;
    if (sumTop < sumBottom)
        dir = DOWN;

    board.expand(dir);

}


