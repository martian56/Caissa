#pragma once
#include "../piece.h"

/**
 * @brief Pawn — the trickiest piece, by some margin.
 *
 * Direction depends on color (white moves up the rows, black moves down).
 * Recognised patterns:
 *   - forward 1 to an empty square,
 *   - forward 2 from the starting rank if both squares are empty,
 *   - diagonal capture of an opponent piece,
 *   - en-passant: diagonal move to an *empty* square that matches the
 *     last-move state stored in Board (set whenever the opposing side
 *     just played a double pawn push).
 *
 * Promotion is detected at the Game level when `to.row()` is the last
 * rank — this method doesn't know about it.
 */
class Pawn : public Piece {
public:
    using Piece::Piece;

    /// @return true iff one of the four pawn patterns matches.
    bool is_legal_move(Square from, Square to, const Board& board) const override;

    char piece_letter() const override { return 'P'; }
};
