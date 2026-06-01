#include "Board.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"
#include <sstream>
#include <cctype>

// ==========  ŒÕ—“–” “Œ– / ƒ≈—“–” “Œ– ==========

Board::Board() : board(8, std::vector<Figure*>(8, nullptr)) {}

Board::~Board() {
    clear();
}

void Board::clear() {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            delete board[x][y];
            board[x][y] = nullptr;
        }
    }
    whiteKingPos = Position(-1, -1);
    blackKingPos = Position(-1, -1);
}

// ========== Œ—ÕŒ¬Õ€≈ Ã≈“Œƒ€ ==========

void Board::setFigure(Position pos, Figure* figure) {
    if (!pos.isValid()) return;
    board[pos.x][pos.y] = figure;
    if (figure) {
        if (figure->getSymbol() == 'K') {
            if (figure->getIsWhite()) whiteKingPos = pos;
            else blackKingPos = pos;
        }
    }
}

Figure* Board::getFigureAt(Position pos) const {
    if (!pos.isValid()) return nullptr;
    return board[pos.x][pos.y];
}

void Board::removeFigure(Position pos) {
    if (!pos.isValid()) return;
    board[pos.x][pos.y] = nullptr;
}

// ========== «¿√–”« ¿ œŒ«»÷»» »« ‘¿…À¿ ==========

void Board::addFigureFromString(const std::string& figStr, bool isWhite) {
    if (figStr.length() < 2) return;

    char figType = figStr[0];
    std::string posStr;

    // ≈ÒÎË ÔÂ‚˚È ÒËÏ‚ÓÎ ÌÂ ·ÛÍ‚‡ (ˆËÙ‡) ËÎË ÒÚÓÍ‡ ËÁ 2 ·ÛÍ‚ ó ˝ÚÓ ÔÂ¯Í‡
    if (figType >= '0' && figType <= '9') {
        // œÂ¯Í‡: ÒÚÓÍ‡ ÚËÔ‡ "A5"
        figType = 'P';
        posStr = figStr;
    }
    else if (figType == 'p') {
        // œÂ¯Í‡: ÒÚÓÍ‡ ÚËÔ‡ "pA5"
        figType = 'P';
        posStr = figStr.substr(1);
    }
    else {
        // Œ·˚˜Ì‡ˇ ÙË„Û‡: "QA4"
        posStr = figStr.substr(1);
    }

    Position pos = Position::fromString(posStr);
    if (!pos.isValid()) return;

    Figure* figure = nullptr;
    switch (figType) {
    case 'K': figure = new King(pos, isWhite); break;
    case 'Q': figure = new Queen(pos, isWhite); break;
    case 'R': figure = new Rook(pos, isWhite); break;
    case 'B': figure = new Bishop(pos, isWhite); break;
    case 'N': figure = new Knight(pos, isWhite); break;
    case 'P': figure = new Pawn(pos, isWhite); break;
    default: return;
    }
    setFigure(pos, figure);
}

void Board::loadFromFile(const std::string& filename) {
    clear();
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line.find("White:") != std::string::npos) {
            int count;
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            iss >> count;

            for (int i = 0; i < count; i++) {
                std::getline(file, line);
                if (!line.empty()) {
                    addFigureFromString(line, true);
                }
            }
        }
        else if (line.find("Black:") != std::string::npos) {
            int count;
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            iss >> count;

            for (int i = 0; i < count; i++) {
                std::getline(file, line);
                if (!line.empty()) {
                    addFigureFromString(line, false);
                }
            }
        }
    }
    file.close();
}

// ========== œ–Œ¬≈– ¿ ¿“¿ » ==========

bool Board::isSquareAttacked(Position pos, bool byWhite) const {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Figure* fig = board[x][y];
            if (fig && fig->getIsWhite() == byWhite) {
                std::vector<Position> moves = fig->getRawMoves(*this);
                for (const Position& move : moves) {
                    if (move == pos) return true;
                }
            }
        }
    }
    return false;
}

bool Board::isCheck(bool whiteKing) {
    Position kingPos = getKingPos(whiteKing);
    if (!kingPos.isValid()) return false;
    return isSquareAttacked(kingPos, !whiteKing);
}

bool Board::isCheckmate(bool whiteKing) {
    if (!isCheck(whiteKing)) return false;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Figure* fig = board[x][y];
            if (fig && fig->getIsWhite() == whiteKing) {
                std::vector<Position> moves = fig->getRawMoves(*this);
                for (const Position& move : moves) {
                    Figure* captured = getFigureAt(move);
                    Position oldPos = fig->getPos();

                    setFigure(move, fig);
                    setFigure(oldPos, nullptr);

                    bool stillCheck = isCheck(whiteKing);

                    setFigure(oldPos, fig);
                    setFigure(move, captured);

                    if (!stillCheck) return false;
                }
            }
        }
    }
    return true;
}

// ========== ÕŒ¬€≈ Ã≈“Œƒ€ ==========

bool Board::isMateInOne(bool whiteMoves) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Figure* fig = board[x][y];
            if (fig && fig->getIsWhite() == whiteMoves) {
                std::vector<Position> moves = fig->getRawMoves(*this);
                for (const Position& move : moves) {
                    Figure* captured = getFigureAt(move);
                    Position oldPos = fig->getPos();
                    setFigure(move, fig);
                    setFigure(oldPos, nullptr);

                    bool isMate = isCheckmate(!whiteMoves);

                    setFigure(oldPos, fig);
                    setFigure(move, captured);

                    if (isMate) return true;
                }
            }
        }
    }
    return false;
}

std::vector<Move> Board::getMovesThatLeadToMate(bool whiteMoves) {
    std::vector<Move> winningMoves;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Figure* fig = board[x][y];
            if (fig && fig->getIsWhite() == whiteMoves) {
                std::vector<Position> moves = fig->getRawMoves(*this);
                for (const Position& move : moves) {
                    Figure* captured = getFigureAt(move);
                    Position oldPos = fig->getPos();
                    setFigure(move, fig);
                    setFigure(oldPos, nullptr);

                    bool opponentCanEscape = false;
                    for (int x2 = 0; x2 < 8 && !opponentCanEscape; x2++) {
                        for (int y2 = 0; y2 < 8 && !opponentCanEscape; y2++) {
                            Figure* oppFig = board[x2][y2];
                            if (oppFig && oppFig->getIsWhite() != whiteMoves) {
                                std::vector<Position> oppMoves = oppFig->getRawMoves(*this);
                                for (const Position& oppMove : oppMoves) {
                                    Figure* oppCaptured = getFigureAt(oppMove);
                                    Position oppOldPos = oppFig->getPos();
                                    setFigure(oppMove, oppFig);
                                    setFigure(oppOldPos, nullptr);

                                    if (!isCheckmate(!whiteMoves)) {
                                        opponentCanEscape = true;
                                    }

                                    setFigure(oppOldPos, oppFig);
                                    setFigure(oppMove, oppCaptured);

                                    if (opponentCanEscape) break;
                                }
                            }
                        }
                    }

                    if (!opponentCanEscape) {
                        Move m;
                        m.from = oldPos;
                        m.to = move;
                        m.figure = fig;
                        m.captured = captured;
                        winningMoves.push_back(m);
                    }

                    setFigure(oldPos, fig);
                    setFigure(move, captured);
                }
            }
        }
    }
    return winningMoves;
}

// ========== ¬€¬Œƒ ==========

void Board::print() const {
    std::cout << "  +-----------------+" << std::endl;
    for (int y = 7; y >= 0; y--) {
        std::cout << (y + 1) << " | ";
        for (int x = 0; x < 8; x++) {
            Figure* fig = board[x][y];
            if (fig) {
                char c = fig->getSymbol();
                if (!fig->getIsWhite()) c = std::tolower(c);
                std::cout << c << " ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "  +-----------------+" << std::endl;
    std::cout << "    a b c d e f g h" << std::endl;
}