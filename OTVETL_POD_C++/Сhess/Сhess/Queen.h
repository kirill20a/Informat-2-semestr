#ifndef QUEEN_H
#define QUEEN_H

#include "Figure.h"
#include "Board.h"

class Queen : public Figure {
public:
    Queen(Position pos, bool isWhite) : Figure(pos, isWhite, 'Q') {}

    std::vector<Position> getRawMoves(const Board& board) const override {
        std::vector<Position> moves;
        // 8 направлений (комбинация ладьи и слона)
        int dx[] = { 1, -1, 0, 0, 1, 1, -1, -1 };
        int dy[] = { 0, 0, 1, -1, 1, -1, 1, -1 };

        for (int dir = 0; dir < 8; dir++) {
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

    std::string getName() const override { return "Ферзь"; }
};

#endif