#include "board.h"
#include "typedefs.h"
#include <vector>
#include <stdexcept>


Board::Board(cv::Mat& image, Lines hlinesSorted, Lines vlinesSorted)
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

            squares.push_back(square);
        }
    }
}

void Board::addRow(Squares row)
{
    if (row.empty())
        return;

    if (nCols == 0){ // then this is the first row added, and it will determine number of columns
        nCols = row.size();
    }
    nRows++;

    if ((int)row.size() != nCols){
        std::invalid_argument("This row has the wrong length for this board");
    }

    for (size_t i = 0; i < row.size(); i++){
        squares.push_back(row.at(i));
    }


}

void Board::addCol(Squares col)
{
    if (col.empty())
        return;

    if (nRows == 0){
        nRows = col.size();
    }
    nCols++;

    if ((int)col.size() != nRows){
        std::invalid_argument("This column has the wrong length for this board");
    }

    for (int i = 0; i < nRows; i++){
        std::vector<Square>::iterator it = squares.begin() + i*nCols + 1;
        it = squares.insert(it, col.at(i));
    }
}

/*
Board::Board(Board oldBoard, std::vector<int> rowsToRemove, std::vector<int> colsToRemove)
{

    nCols = oldBoard.getNumCols() - colsToRemove.size();
    nRows = oldBoard.getNumRows() - rowsToRemove.size();

    for (int i = 0; i < oldBoard.getNumCols(); i++){
        if (std::count(colsToRemove.begin(), colsToRemove.end(), i) == 0){
            for (int j = 0; j < oldBoard.getNumRows(); j++){
                if (std::count(rowsToRemove.begin(), rowsToRemove.end(), j) == 0){
                    squares.push_back(oldBoard.getSquare(j,i));
                }
            }
        }
    }
}
*/

int Board::getIndex(int row, int col)
{
    return row * nCols + col; // assumes squares are concatenated row after row
}

Square Board::getSquare(int row, int col)
{
    int idx = getIndex(row, col);

    Square square = squares.at(idx);
    return square;
}

Square& Board::getSquareRef(int rowIdx, int colIdx){
    int idx = getIndex(rowIdx, colIdx);

    if (idx > (int)squares.size()-1){
        std::invalid_argument("Trying to access squares beyond number of elements");
    }
    Square& square = squares.at(idx);
    return square;
}

Squares Board::getRow(int rowIdx)
{

    if (rowIdx > nRows){
        throw std::invalid_argument("row index > number of rows in board");
    }

    int idx1 = rowIdx * nCols;
    int idx2 = idx1 + nCols - 1;

    Squares row(&squares[idx1], &squares[idx2]);
    return row;
}

Squares Board::getCol(int colIdx)
{
    if (colIdx > nCols){
        throw std::invalid_argument("col index > number of columns in board");
    }

    Squares column(nRows);
    for (int i = 0; i < nRows; i++){
        //Square square = squares.at(colIdx + nCols * i);
        int squaresIdx = colIdx + nCols * i + 1;
        column.push_back(squares.at(squaresIdx));
    }

    return column;
}

void Board::draw(cv::Mat &image)
{
    cv::RNG rng = cv::RNG(1234);
    for (size_t i = 0; i < squares.size(); i++) {

        cv::Scalar col = cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
        cv::fillConvexPoly(image, squares.at(i).getCornerpoints(), col);
        cv::imshow("poly", image);
        cv::waitKey(2);

    }
    cv::waitKey();
}



