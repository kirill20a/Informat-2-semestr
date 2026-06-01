#ifndef BISHOP_H
#define BISHOP_H

#include "Figure.h"
#include "Board.h"

class Bishop : public Figure {
public:
    Bishop(Position pos, bool isWhite) : Figure(pos, isWhite, 'B') {}

    std::vector<Position> getRawMoves(const Board& board) const override {
        std::vector<Position> moves;
        int dx[] = { 1, 1, -1, -1 };
        int dy[] = { 1, -1, 1, -1 };

        for (int dir = 0; dir < 4; dir++) {
            for (int step = 1; step <= 7; step++) {
                int nx = pos.x + dx[dir] * step;
                int ny = pos.y + dy[dir] * step;
                Position newPos(nx, ny);
                if (!newPos.isValid()) break;

                Figure* fig = board.getFigureAt(newPos);
                if (!fig) {
                    moves.push_back(newPos);
                }
                else {
                    if (fig->getIsWhite() != isWhite) moves.push_back(newPos);
                    break;
                }
            }
        }
        return moves;
    }

    std::string getName() const override { return "Слон"; }
};

#endif