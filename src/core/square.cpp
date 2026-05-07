#include "square.h"

Square::Square(const std::string& algebraic) : row_(-1), col_(-1) {
    if (algebraic.size() != 2) return;
    char file = algebraic[0];
    char rank = algebraic[1];
    if (file < 'a' || file > 'h') return;
    if (rank < '1' || rank > '8') return;
    col_ = file - 'a';
    row_ = rank - '1';
}

std::string Square::to_string() const {
    if (!is_valid()) return "??";
    std::string out;
    out += static_cast<char>('a' + col_);
    out += static_cast<char>('1' + row_);
    return out;
}
