#pragma once
#include "../core/board.h"

/**
 * @brief AI facade — picks a move at a fixed search depth.
 *
 * Internally runs alpha-beta negamax with material-only evaluation.
 * Stateless beyond the depth setting, so a single Engine instance is
 * fine for a whole game (or several).
 *
 * The whole `engine/` folder is optional: deleting it leaves `core/`
 * compiling cleanly because Game has no compile-time link to Engine.
 */
class Engine {
    int depth_;

public:
    /// @param depth Search ply count. 3 is enough for material-only play.
    explicit Engine(int depth = 3) : depth_(depth) {}

    /**
     * @brief Pick the best move for `color`.
     * @param board The live position. Mutated during search and fully
     *              restored before this returns (try-then-undo).
     * @param color Side to move.
     * @return The chosen move. If there's no legal move at all (game
     *         already over), returns a default-constructed Move whose
     *         `from.is_valid()` is false.
     */
    Move best_move(Board& board, Color color);
};
