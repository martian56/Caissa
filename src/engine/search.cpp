#include "search.h"
#include "evaluator.h"
#include "move_generator.h"

namespace {

constexpr int MATE_SCORE = 100000;
constexpr int INF = 200000;

int score_for(Color c, const Board& board) {
    // material_score is from White's perspective; flip for Black.
    const int s = Evaluator::material_score(board);
    return c == Color::White ? s : -s;
}

}  // namespace

int Search::alpha_beta(Board& board, int depth, int alpha, int beta, Color to_move) {
    if (depth == 0) return score_for(to_move, board);

    auto moves = MoveGenerator::all_legal_moves(board, to_move);
    if (moves.empty()) {
        // No moves: it's mate if we're in check, else stalemate.
        // The depth bonus makes short mates look worse than long ones to
        // the side getting mated, which is what we want.
        if (board.is_in_check(to_move)) return -MATE_SCORE - depth;
        return 0;
    }

    int best = -INF;
    for (const Move& m : moves) {
        MoveUndo u = board.execute_move(m);
        const int score = -alpha_beta(board, depth - 1, -beta, -alpha, opposite(to_move));
        board.undo_move(u);

        if (score > best) best = score;
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;  // beta cutoff
    }
    return best;
}
