#pragma once
#include "../piece.h"

/**
 * @brief Bishop — any number of squares along a diagonal.
 *
 * Pattern: |Δrow| == |Δcol|. Path must be clear.
 */
class Bishop : public Piece {
public:
    using Piece::Piece;

    /// @return true iff move is on a diagonal and the path is clear.
    bool is_legal_move(Square from, Square to, const Board& board) const override;

    char piece_letter() const override { return 'B'; }
};
