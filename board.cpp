#include "board.h"
#include "Line.h"
#include "square.h"
#include "typedefs.h"
#include "matrix.h"
#include "squareExpander.h"

Board::Board(cv::Mat& image_) : image(image_)
{
    nCols = 0;
    nRows = 0;
}

Board::Board(cv::Mat &image_, Lines hlinesSorted, Lines vlinesSorted) : image(image_)
{
    nCols = vlinesSorted.size() - 1;
    nRows = hlinesSorted.size() - 1;

    for (size_t i = 0; i < hlinesSorted.size()-1; ++i) {
        Line hlineUpper = hlinesSorted.at(i);
        Line hlineLower = hlinesSorted.at(i+1);
        for (size_t j = 0; j < vlinesSorted.size()-1; ++j) {
            Line vlineLeft = vlinesSorted.at(j);
            Line vlineRight = vlinesSorted.at(j+1);

            cv::Point upperLeft, upperRight, lowerLeft, lowerRight;
            hlineUpper.Intersection(vlineLeft, upperLeft);
            hlineUpper.Intersection(vlineRight, upperRight);
            hlineLower.Intersection(vlineLeft, lowerLeft);
            hlineLower.Intersection(vlineRight, lowerRight);

            Square square(image, upperLeft, upperRight, lowerRight, lowerLeft);

            elements.push_back(square);
        }
    }
}

void Board::determineRowTypes()
{
    rowTypes.clear();

    for (int i = 0; i < nRows; i++){
        std::vector<int> histogram(5,0);
        for (int j = 0; j < nCols; j++){
            Square& square = getRef(i, j);
            int type = square.getSquareType();
            ++histogram[ type ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < nCols-1) // if there are two or more votes for other categories
        {
            rowTypes.push_back(vote);
            //rowTypes.push_back(-1); // might be part of the board
        } else {
            rowTypes.push_back(vote);
        }
    }
}

void Board::determineColTypes()
{
    colTypes.clear();

    for (int i = 0; i < nCols; i++){
        std::vector<int> histogram(5,0);
        for (int j = 0; j < nRows; j++){
            Square& square =  getRef(j, i);
            int type = square.getSquareType();
            ++histogram[ type ];
        }

        int vote = std::max_element( histogram.begin(), histogram.end() ) - histogram.begin();
        if (vote == 0 && histogram[0] < nCols-1) // if there are two or more votes for other categories
        {
            colTypes.push_back(vote);
            //colTypes.push_back(-1); // might be part of the board
        } else {
            colTypes.push_back(vote);
        }
    }
}

std::vector<int> Board::getRowTypes()
{
    if (rowTypes.empty()){
        determineRowTypes();
    }
    return rowTypes;
}

std::vector<int> Board::getColTypes()
{
    if (colTypes.empty()){
        determineColTypes();
    }
    return colTypes;
}

void Board::draw()
{
    if (elements.empty()){
        std::cout << "This board is empty" << std::endl;
        return;
    }

    if (!image.data){
        throw std::invalid_argument("draw() has no image to draw on");
    }
    cv::Mat img_draw;
    image.copyTo(img_draw);

    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < elements.size(); i++) {

        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(img_draw, elements.at(i).getCornerpointsSorted(), col);
        cv::imshow("poly", img_draw);
        cv::waitKey(1);
    }
    cv::waitKey();
}

std::pair<int,int> Board::getStatus()
{
    std::pair<int,int> status; //row, col

    status.first = 8 - nRows; // number of rows needed to add/shrink
    status.second = 8 - nCols;

    return status;
}

void Board::expand(Direction dir)
{
    size_t size;
    Squares baseSquares;
    switch(dir){
    case UP:
        size = nCols;
        baseSquares = this->getRow(0);
        std::cout << "Adding row to top of board" << std::endl;
        break;
    case DOWN:
        size = nCols;
        baseSquares = this->getRow(nRows-1);
        std::cout << "Adding row to bottom of board" << std::endl;
        break;
    case LEFT:
        size = nRows;
        baseSquares = this->getCol(0);
        std::cout << "Adding column to left of board" << std::endl;
        break;
    case RIGHT:
        size = nRows;
        baseSquares = this->getCol(nCols-1);
        std::cout << "Adding column to right of board" << std::endl;
        break;

    }

    Squares newsquares(size);
    for (size_t i = 0; i < size; i++){
        SquareExpander se(image, baseSquares[i], dir);
        newsquares[i] = se.getSquare();
    }

    switch(dir){  // TODO: use function pointers instead and define them in the previous switch
    case UP:
    case DOWN:
        this->addRow(newsquares);
        break;
    case LEFT:
    case RIGHT:
        this->addCol(newsquares);
        break;
    }
}

