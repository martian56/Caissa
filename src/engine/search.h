#pragma once
#include "../core/board.h"

/**
 * @brief Negamax alpha-beta search with material-only leaf evaluation.
 *
 * No state between calls. Mate scores are penalised by depth so the
 * search prefers a mate in 2 over a mate in 4 (and conversely).
 */
class Search {
public:
    static constexpr int DEFAULT_DEPTH = 3;

    /**
     * @param board   Live position; mutated and restored during recursion.
     * @param depth   Plies remaining. 0 ⇒ return the leaf evaluation.
     * @param alpha   Lower bound of the search window (negamax convention).
     * @param beta    Upper bound.
     * @param to_move Side to move at this node.
     * @return Best score for `to_move`, from `to_move`'s perspective.
     */
    static int alpha_beta(Board& board, int depth, int alpha, int beta, Color to_move);
};
