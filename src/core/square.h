#pragma once
#include <string>

/**
 * @brief A square on the 8x8 chessboard.
 *
 * Stored internally as (row, col): row 0 = rank 1, col 0 = file a.
 * So a1 = (0,0), h1 = (0,7), a8 = (7,0), h8 = (7,7).
 */
class Square {
    int row_;
    int col_;

public:
    Square() : row_(-1), col_(-1) {}
    Square(int row, int col) : row_(row), col_(col) {}

    /// Build from algebraic notation (e.g. "a1"). Invalid input yields an invalid square.
    explicit Square(const std::string& algebraic);

    int row() const { return row_; }
    int col() const { return col_; }

    /// True iff both row and col are within [0,7].
    bool is_valid() const { return row_ >= 0 && row_ < 8 && col_ >= 0 && col_ < 8; }

    /// Convert to algebraic notation, e.g. (0,0) -> "a1".
    std::string to_string() const;

    bool operator==(const Square& o) const { return row_ == o.row_ && col_ == o.col_; }
    bool operator!=(const Square& o) const { return !(*this == o); }
};
