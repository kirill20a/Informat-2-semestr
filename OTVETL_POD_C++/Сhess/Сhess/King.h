#ifndef KING_H
#define KING_H

#include "Figure.h"
#include "Board.h"

class King : public Figure {
public:
    King(Position pos, bool isWhite) : Figure(pos, isWhite, 'K') {}

    std::vector<Position> getRawMoves(const Board& board) const override {
        std::vector<Position> moves;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                Position newPos(pos.x + dx, pos.y + dy);
                if (!newPos.isValid()) continue;
                Figure* fig = board.getFigureAt(newPos);
                if (!fig || fig->getIsWhite() != isWhite) {
                    moves.push_back(newPos);
                }
            }
        }
        return moves;
    }

    std::string getName() const override { return "Король"; }
};

#endif