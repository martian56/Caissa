#pragma once
#include "board.h"
#include <string>

/**
 * @brief Top-level game controller.
 *
 * Reads moves from stdin, validates them, applies them to the Board,
 * alternates turns, handles `/quit` `/resign` `/draw`, detects check /
 * checkmate / stalemate.
 *
 * On end of game (any reason) prints `<canonical_position> <result>` on
 * the last line of stdout — that's what the test harness compares against.
 *
 * Game knows nothing about the AI engine. Integration lives in main.cpp,
 * which calls `play_external_move` whenever it's the AI's turn.
 */
class Game {
    Board board_;
    Color to_move_;
    std::string result_;  ///< "1-0", "0-1", "1/2-1/2", or "?-?"
    bool finished_;

    bool try_move(const Square& from, const Square& to, char promo_inline);
    bool handle_castling(bool queenside);
    void check_end_of_game();
    void print_final_result();

public:
    Game();
    ~Game();

    void display() const;

    /// Read and process one move from stdin. Returns true once the game ended.
    bool play_turn();

    /**
     * @brief Apply a pre-validated move, e.g. one chosen by the AI.
     * @param m  The move. If `from.is_valid()` is false, treats this as
     *           "no move available" (game already over) and just emits
     *           the final result line.
     * @return true if the game has now ended.
     */
    bool play_external_move(const Move& m);

    Board& board() { return board_; }
    const Board& board() const { return board_; }
    Color to_move() const { return to_move_; }
    bool finished() const { return finished_; }
};
