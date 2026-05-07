#include "king.h"
#include "../board.h"
#include <cstdlib>

bool King::is_legal_move(Square from, Square to, const Board& /*board*/) const {
    if (from == to) return false;
    int dr = std::abs(to.row() - from.row());
    int dc = std::abs(to.col() - from.col());
    return dr <= 1 && dc <= 1;
}
