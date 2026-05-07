#include "rook.h"
#include "../board.h"

bool Rook::is_legal_move(Square from, Square to, const Board& board) const {
    if (from == to) return false;
    if (from.row() != to.row() && from.col() != to.col()) return false;
    return board.path_clear(from, to);
}
