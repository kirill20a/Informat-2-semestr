#ifndef PAWN_H
#define PAWN_H

#include "Figure.h"
#include "Board.h"

class Pawn : public Figure {
public:
    Pawn(Position pos, bool isWhite) : Figure(pos, isWhite, 'P') {}

    std::vector<Position> getRawMoves(const Board& board) const override {
        std::vector<Position> moves;
        int direction = isWhite ? 1 : -1;  // белые вверх, чёрные вниз
        int startRow = isWhite ? 1 : 6;    // начальная позиция для двойного хода

        // Один шаг вперёд
        Position oneStep(pos.x, pos.y + direction);
        if (oneStep.isValid() && !board.getFigureAt(oneStep)) {
            moves.push_back(oneStep);

            // Два шага с начальной позиции
            Position twoStep(pos.x, pos.y + 2 * direction);
            if (pos.y == startRow && !board.getFigureAt(twoStep)) {
                moves.push_back(twoStep);
            }
        }

        // Взятие по диагонали
        Position captureLeft(pos.x - 1, pos.y + direction);
        if (captureLeft.isValid()) {
            Figure* fig = board.getFigureAt(captureLeft);
            if (fig && fig->getIsWhite() != isWhite) {
                moves.push_back(captureLeft);
            }
        }

        Position captureRight(pos.x + 1, pos.y + direction);
        if (captureRight.isValid()) {
            Figure* fig = board.getFigureAt(captureRight);
            if (fig && fig->getIsWhite() != isWhite) {
                moves.push_back(captureRight);
            }
        }

        return moves;
    }

    std::string getName() const override { return "Пешка"; }
};

#endif