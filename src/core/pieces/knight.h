#pragma once
#include "../piece.h"

/**
 * @brief Knight — L-shape (2+1 in any axis combination).
 *
 * Only piece allowed to jump over others, so the `board` parameter is
 * unused inside is_legal_move.
 */
class Knight : public Piece {
public:
    using Piece::Piece;

    /// @return true iff (|Δrow|, |Δcol|) is (1,2) or (2,1).
    bool is_legal_move(Square from, Square to, const Board& board) const override;

    /// 'N' rather than 'K' (which is the King).
    char piece_letter() const override { return 'N'; }
};
