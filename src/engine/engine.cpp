#include "engine.h"
#include "search.h"
#include "move_generator.h"

namespace {
constexpr int INF = 200000;
}

Move Engine::best_move(Board& board, Color color) {
    auto moves = MoveGenerator::all_legal_moves(board, color);
    if (moves.empty()) return Move();

    int best_score = -INF;
    Move best;
    for (const Move& m : moves) {
        MoveUndo u = board.execute_move(m);
        const int score = -Search::alpha_beta(board, depth_ - 1, -INF, INF, opposite(color));
        board.undo_move(u);

        if (score > best_score) {
            best_score = score;
            best = m;
        }
    }
    return best;
}
