#pragma once
#include "../piece.h"

/**
 * @brief Queen — rook + bishop combined.
 *
 * Either pattern works (straight line OR diagonal), with the same
 * path-clear requirement.
 */
class Queen : public Piece {
public:
    using Piece::Piece;

    /// @return true iff move is rook- or bishop-shaped and the path is clear.
    bool is_legal_move(Square from, Square to, const Board& board) const override;

    char piece_letter() const override { return 'Q'; }
};
