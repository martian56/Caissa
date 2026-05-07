#pragma once
#include "../piece.h"

/**
 * @brief Rook — any number of squares along a row or column.
 *
 * Cannot leap, so path_clear is consulted. Castling is driven by Game,
 * not by this class.
 */
class Rook : public Piece {
public:
    using Piece::Piece;

    /// @return true iff `from` and `to` share a row XOR column AND the path is clear.
    bool is_legal_move(Square from, Square to, const Board& board) const override;

    char piece_letter() const override { return 'R'; }
};
