#pragma once
#include "../core/board.h"
#include <vector>

/**
 * @brief Builds a vector of every legal move.
 *
 * Just a wrapper around Board::for_each_legal_move that pushes each move
 * into the result. Includes en-passant and the four promotion variants
 * per pawn promotion. Skips castling for now — keeps things simple.
 */
class MoveGenerator {
public:
    /**
     * @param board Live position; mutated during enumeration and restored.
     * @param color Side whose moves are listed.
     * @return Every legal move (possibly empty if mated/stalemated).
     */
    static std::vector<Move> all_legal_moves(Board& board, Color color);
};
