#include "bishop.h"
#include "../board.h"
#include <cstdlib>

bool Bishop::is_legal_move(Square from, Square to, const Board& board) const {
    if (from == to) return false;
    int dr = std::abs(to.row() - from.row());
    int dc = std::abs(to.col() - from.col());
    if (dr != dc) return false;
    return board.path_clear(from, to);
}
