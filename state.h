#ifndef STATE_H
#define STATE_H

#include "matrix.h"
#include "typedefs.h"

const std::vector<std::pair<int,int> > innermoves{std::make_pair(-1,-1),std::make_pair(-1,1),std::make_pair(1,1),std::make_pair(1,-1)};
const std::vector<std::pair<int,int> > outermoves{std::make_pair(-2,-2),std::make_pair(-2,2),std::make_pair(2,2),std::make_pair(2,-2)};

class Path{
public:
    Path(){
        captured = 0;
    }
    ~Path(){
        int hei = 1;
    }

    void add(std::pair<int,int> move){
        moves.push_back(move);
    }

    int captured;
    std::vector<std::pair<int,int> > moves;
    int start;
};

class State : public matrix<int>
{
public:
    // Constructors
    State();
    explicit State(const State *state);
    State(const State *state, size_t pieceIdx, std::pair<int,int> move);
    State MultiMoves(size_t pieceIdx, std::vector<std::pair<int,int>> moves) const;

    // Methods
    void print() const;
    void copyTo(State& state) const;

    bool isEndOfGame() const {return (nBlack == 0 || nWhite == 0);}

    std::vector<int> getSurroundings(size_t element) const;
    static std::vector<State> findMovesForPiece(const State *state, int pieceIdx);
    static Path findPath(State *state, int pieceidx, int depth, Path currentpath, bool moveUp, bool moveDown);
    std::vector<State> findMovesForPlayer(int player) const;

    // Extended methods
    void addToElement(size_t row, size_t col, int increment);

    // Static methods
    static State createState(int id = 1);

private:
    int nBlack;
    int nWhite;
};



#endif // STATE_H

