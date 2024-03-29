#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <vector>
#include <string>
#include "Line.h"
#include "cvutils.h"
#include "corner.h"
#include "typedefs.h"
#include "board.h"
#include "remover.h"
#include "report.h"

class BoardDetector
{
public:
    ~BoardDetector();
    BoardDetector(std::vector<Line>);

    Lines get_hlinesSorted();
    Lines get_vlinesSorted();
    Corners getCorners();

    bool detect(Board &dst, std::string *reportPath = 0);
    void writeHoughAfterCategorizationToGlobal();
private:
    bool boardInitialized;
    void categorizeLines();
    Lines filterBasedOnVanishingPoint(Lines vlines);
    void filterBasedOnSquareSize(Board& Board, Remover& remover);
    void filterBasedOnRowType(Board& Board, Remover& remover);
    void filterBasedOnColType(Board& Board, Remover& remover);
    void requestColumnExpansion(Board &board);
    void requestRowExpansion(Board& board);

    std::vector<Line> lines;
    std::vector<double> slopes;
    int xvpoint;
    std::vector<int> hlinesIdx;
    std::vector<int> vlinesIdx;
    Lines hlinesSorted;
    Lines vlinesSorted;
    std::vector<std::vector<int>> squareTypes;
    Corners corners;

};

#endif // POSTPROCESS_H
