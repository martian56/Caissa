#include "pawn.h"
#include "../board.h"
#include <cstdlib>

bool Pawn::is_legal_move(Square from, Square to, const Board& board) const {
    if (from == to) return false;

    const int direction = (color_ == Color::White) ? +1 : -1;
    const int starting_rank = (color_ == Color::White) ? 1 : 6;
    const int dr = to.row() - from.row();
    const int dc = to.col() - from.col();

    // 1) Forward 1 to empty square.
    if (dc == 0 && dr == direction && board.is_square_empty(to)) return true;

    // 2) Forward 2 from starting rank, both squares empty.
    if (dc == 0 && dr == 2 * direction && from.row() == starting_rank &&
        board.is_square_empty(to) &&
        board.is_square_empty(Square(from.row() + direction, from.col())))
        return true;

    // 3) Diagonal capture of opponent piece.
    if (std::abs(dc) == 1 && dr == direction) {
        Piece* target = board.get_piece(to);
        if (target != nullptr && target->get_color() != color_) return true;

        // 4) En passant: target square is empty but matches last-move state.
        const auto& lm = board.last_move();
        if (lm.was_double_pawn_push && lm.en_passant_target == to) return true;
    }
    return false;
}
