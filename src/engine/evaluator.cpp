#include "evaluator.h"

namespace {

int piece_value(char letter) {
    switch (letter) {
    case 'P':
        return 100;
    case 'N':
        return 320;
    case 'B':
        return 330;
    case 'R':
        return 500;
    case 'Q':
        return 900;
    case 'K':
        return 0;  // king's life is handled via mate score
    default:
        return 0;
    }
}

}  // namespace

int Evaluator::material_score(const Board& board) {
    int score = 0;
    for (const auto& p : board.pieces(Color::White)) {
        if (p->is_captured()) continue;
        score += piece_value(p->piece_letter());
    }
    for (const auto& p : board.pieces(Color::Black)) {
        if (p->is_captured()) continue;
        score -= piece_value(p->piece_letter());
    }
    return score;
}
