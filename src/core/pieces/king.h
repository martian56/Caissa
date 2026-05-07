#pragma once
#include "../piece.h"

/**
 * @brief King — one square in any of the 8 directions.
 *
 * Castling never reaches this method; Game::handle_castling builds a
 * Move with is_kingside_castle / is_queenside_castle set and skips the
 * piece's own legality check.
 */
class King : public Piece {
public:
    using Piece::Piece;

    /// @return true iff the destination is one step away in any direction.
    bool is_legal_move(Square from, Square to, const Board& board) const override;

    char piece_letter() const override { return 'K'; }
};
