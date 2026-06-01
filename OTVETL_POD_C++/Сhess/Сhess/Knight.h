#ifndef KNIGHT_H
#define KNIGHT_H

#include "Figure.h"
#include "Board.h"

class Knight : public Figure {
public:
    Knight(Position pos, bool isWhite) : Figure(pos, isWhite, 'N') {}

    std::vector<Position> getRawMoves(const Board& board) const override {
        std::vector<Position> moves;
        int dx[] = { 2, 2, -2, -2, 1, 1, -1, -1 };
        int dy[] = { 1, -1, 1, -1, 2, -2, 2, -2 };

        for (int i = 0; i < 8; i++) {
            Position newPos(pos.x + dx[i], pos.y + dy[i]);
            if (!newPos.isValid()) continue;
            Figure* fig = board.getFigureAt(newPos);
            if (!fig || fig->getIsWhite() != isWhite) {
                moves.push_back(newPos);
            }
        }
        return moves;
    }

    std::string getName() const override { return "Конь"; }
};

#endif