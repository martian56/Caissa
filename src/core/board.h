#pragma once
#include "piece.h"
#include "square.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

constexpr int BOARD_SIZE = 8;

/// State carried over from the last move so en-passant can be resolved.
struct LastMove {
    bool was_double_pawn_push = false;
    Square en_passant_target;    ///< the square the pawn jumped over
    Color color = Color::White;  ///< color that just played
};

/// One move request. Built by parsing user input or by the engine.
struct Move {
    Square from;
    Square to;
    bool is_promotion = false;
    char promo_piece = 'Q';            ///< Q / R / B / N
    bool is_kingside_castle = false;   ///< O-O
    bool is_queenside_castle = false;  ///< O-O-O
    bool is_en_passant = false;

    Move() = default;
    Move(Square f, Square t) : from(f), to(t) {}
};

/// What execute_move() returns; pass it back to undo_move() to reverse.
/// The unique_ptr that owns a promotion piece lives in pieces_; this
/// struct only holds an observer (raw) pointer to it.
struct MoveUndo {
    Move move;
    Piece* moved_piece = nullptr;
    Piece* captured_piece = nullptr;  ///< null if the move wasn't a capture
    Square capture_square;            ///< usually = move.to (en passant differs)
    bool moved_had_moved_before = false;
    LastMove last_move_before;

    Piece* castling_rook = nullptr;
    Square rook_from, rook_to;
    bool rook_had_moved_before = false;

    Piece* pawn_before_promotion = nullptr;
    Piece* new_promoted_piece = nullptr;  ///< observer; lives inside pieces_
};

/**
 * @brief 8x8 chessboard. Owns every piece it creates.
 *
 * The board itself is a grid of raw `Piece*` (non-owning observers). Real
 * ownership lives in `pieces_[color]` as `std::vector<unique_ptr<Piece>>`,
 * which means the destructor is defaulted — no manual delete loop.
 *
 * Captured pieces aren't removed from the roster; they get a flag instead.
 * Keeping them around makes execute_move/undo_move free of any allocation,
 * which matters for the alpha-beta search.
 */
class Board {
public:
    using PieceList = std::vector<std::unique_ptr<Piece>>;

    Board();
    ~Board() = default;
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    /// Render the board to stdout (text mode).
    void display() const;

    bool is_square_empty(Square s) const;
    Piece* get_piece(Square s) const;
    void place_piece(Piece* p, Square s);

    /**
     * @brief All squares strictly between `from` and `to` are empty?
     * @pre `from` and `to` lie on the same row, column, or diagonal.
     */
    bool path_clear(Square from, Square to) const;

    /// Position of the king of color `c`, or an invalid Square if absent
    /// (shouldn't normally happen; just a defensive sentinel).
    Square find_king(Color c) const;

    /// True if the king of color `c` is currently attacked.
    bool is_in_check(Color c) const;

    /// True if `c` has at least one move that doesn't leave its own king
    /// in check. Used for both checkmate and stalemate detection.
    bool has_any_legal_move(Color c);

    /**
     * @brief Apply a move (already validated by the caller).
     * @return MoveUndo containing everything needed to reverse the move.
     */
    MoveUndo execute_move(const Move& m);

    /// Reverse a previous execute_move() using its returned MoveUndo.
    void undo_move(const MoveUndo& u);

    /**
     * @brief Visit every legal move for `c`.
     * @param c                  Side to enumerate.
     * @param expand_promotions  True ⇒ generate Q/R/B/N for each promotion;
     *                           false ⇒ only Queen (faster, used by mate detection).
     * @param callback           Receives each Move; return true to continue,
     *                           false to stop iteration early.
     * @return true if iteration ran to completion, false if stopped early.
     *
     * Used by has_any_legal_move and by the engine's MoveGenerator. Castling
     * is NOT enumerated here — it's handled in Game and is rare enough that
     * leaving it out of mate detection is harmless (you can't castle out of
     * check anyway).
     */
    template <typename Fn> bool for_each_legal_move(Color c, bool expand_promotions, Fn&& callback);

    /**
     * @brief Canonical-position string for the test harness.
     *
     * Order: a1,b1,…,h1,a2,…,h8. Each piece is `wK`/`bP`/etc.; empty
     * squares contribute nothing before their comma. Every square is
     * comma-terminated, including the last.
     */
    std::string canonical_position() const;

    const LastMove& last_move() const { return last_move_; }
    void set_last_move(const LastMove& d) { last_move_ = d; }

    /// Read-only roster for color `c`. Includes captured pieces — filter
    /// them out with `is_captured()`.
    const PieceList& pieces(Color c) const { return pieces_[to_index(c)]; }

private:
    std::array<std::array<Piece*, 8>, 8> board_{};  // observers; default zero-inits
    std::array<PieceList, 2> pieces_;               // owners
    LastMove last_move_;

    void execute_castling(const Move& m, MoveUndo& u);
    void execute_normal_move(const Move& m, MoveUndo& u);
    void undo_castling(const MoveUndo& u);
    void undo_normal_move(const MoveUndo& u);

    std::unique_ptr<Piece> create_promoted_piece(Color c, char which, Square s);
};

// for_each_legal_move has to live in the header so all callers can
// instantiate it.
template <typename Fn>
bool Board::for_each_legal_move(Color c, bool expand_promotions, Fn&& callback) {
    for (const auto& piece : pieces_[to_index(c)]) {
        if (piece->is_captured()) continue;
        const Square from = piece->get_position();

        for (int r = 0; r < 8; r++) {
            for (int col = 0; col < 8; col++) {
                const Square to(r, col);
                if (from == to) continue;
                if (!piece->is_legal_move(from, to, *this)) continue;

                Piece* target = board_[r][col];
                if (target && target->get_color() == c) continue;

                const bool is_ep =
                    (piece->piece_letter() == 'P') && (target == nullptr) && (from.col() != col);
                const bool is_promo_rank =
                    (piece->piece_letter() == 'P') &&
                    ((c == Color::White && r == 7) || (c == Color::Black && r == 0));

                auto try_move = [&](Move m) -> bool {
                    MoveUndo u = execute_move(m);
                    const bool in_check = is_in_check(c);
                    undo_move(u);
                    if (in_check) return true;  // illegal, but keep iterating
                    return callback(m);
                };

                Move base_m(from, to);
                if (is_ep) base_m.is_en_passant = true;

                if (is_promo_rank) {
                    const std::array<char, 4> promos =
                        expand_promotions ? std::array<char, 4>{'Q', 'R', 'B', 'N'}
                                          : std::array<char, 4>{'Q', 'Q', 'Q', 'Q'};
                    const int n_promos = expand_promotions ? 4 : 1;
                    for (int i = 0; i < n_promos; i++) {
                        Move pm = base_m;
                        pm.is_promotion = true;
                        pm.promo_piece = promos[i];
                        if (!try_move(pm)) return false;
                    }
                } else {
                    if (!try_move(base_m)) return false;
                }
            }
        }
    }
    return true;
}
