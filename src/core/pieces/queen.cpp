#include "queen.h"
#include "../board.h"
#include <cstdlib>

bool Queen::is_legal_move(Square from, Square to, const Board& board) const {
    if (from == to) return false;
    int dr = std::abs(to.row() - from.row());
    int dc = std::abs(to.col() - from.col());
    bool straight = (from.row() == to.row()) || (from.col() == to.col());
    bool diagonal = (dr == dc);
    if (!straight && !diagonal) return false;
    return board.path_clear(from, to);
}
