#include "move_generator.h"

std::vector<Move> MoveGenerator::all_legal_moves(Board& board, Color color) {
    std::vector<Move> moves;
    moves.reserve(64);
    board.for_each_legal_move(color, /*expand_promotions=*/true, [&](const Move& m) {
        moves.push_back(m);
        return true;  // continue iterating
    });
    return moves;
}
