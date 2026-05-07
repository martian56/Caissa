#pragma once
#include "../core/board.h"

/**
 * @brief Material-only position evaluator.
 *
 * No positional terms, no king safety, no pawn structure — just the
 * piece values. Plenty for a depth-3 search to look reasonable, and
 * keeps the engine code small.
 */
class Evaluator {
public:
    /**
     * @brief Material balance of `board`.
     *
     * Values: P = 100, N = 320, B = 330, R = 500, Q = 900, K = 0.
     *
     * @return Centipawn score from White's perspective. Positive favours
     *         White, negative favours Black.
     */
    static int material_score(const Board& board);
};
