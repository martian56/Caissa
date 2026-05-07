#include "board.h"
#include "pieces/rook.h"
#include "pieces/knight.h"
#include "pieces/bishop.h"
#include "pieces/queen.h"
#include "pieces/king.h"
#include "pieces/pawn.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>

namespace {

// Allocate a piece, drop the raw observer pointer onto the grid, and hand
// ownership to the roster. Used only by the constructor.
template <typename T, typename... Args>
void emplace_piece(Board::PieceList& roster, std::array<std::array<Piece*, 8>, 8>& board,
                   Args&&... args) {
    auto piece = std::make_unique<T>(std::forward<Args>(args)...);
    Square pos = piece->get_position();
    board[pos.row()][pos.col()] = piece.get();
    roster.push_back(std::move(piece));
}

}  // namespace

Board::Board() {
    auto& white = pieces_[to_index(Color::White)];
    auto& black = pieces_[to_index(Color::Black)];
    white.reserve(16);
    black.reserve(16);

    // White back rank
    emplace_piece<Rook>(white, board_, Color::White, "♖", 0, Square(0, 0));
    emplace_piece<Knight>(white, board_, Color::White, "♘", 1, Square(0, 1));
    emplace_piece<Bishop>(white, board_, Color::White, "♗", 2, Square(0, 2));
    emplace_piece<Queen>(white, board_, Color::White, "♕", 3, Square(0, 3));
    emplace_piece<King>(white, board_, Color::White, "♔", 4, Square(0, 4));
    emplace_piece<Bishop>(white, board_, Color::White, "♗", 5, Square(0, 5));
    emplace_piece<Knight>(white, board_, Color::White, "♘", 6, Square(0, 6));
    emplace_piece<Rook>(white, board_, Color::White, "♖", 7, Square(0, 7));

    // Black back rank
    emplace_piece<Rook>(black, board_, Color::Black, "♜", 8, Square(7, 0));
    emplace_piece<Knight>(black, board_, Color::Black, "♞", 9, Square(7, 1));
    emplace_piece<Bishop>(black, board_, Color::Black, "♝", 10, Square(7, 2));
    emplace_piece<Queen>(black, board_, Color::Black, "♛", 11, Square(7, 3));
    emplace_piece<King>(black, board_, Color::Black, "♚", 12, Square(7, 4));
    emplace_piece<Bishop>(black, board_, Color::Black, "♝", 13, Square(7, 5));
    emplace_piece<Knight>(black, board_, Color::Black, "♞", 14, Square(7, 6));
    emplace_piece<Rook>(black, board_, Color::Black, "♜", 15, Square(7, 7));

    // Pawns
    for (int i = 0; i < 8; i++) {
        emplace_piece<Pawn>(white, board_, Color::White, "♙", 16 + i, Square(1, i));
        emplace_piece<Pawn>(black, board_, Color::Black, "♟", 24 + i, Square(6, i));
    }
}

void Board::place_piece(Piece* p, Square s) {
    board_[s.row()][s.col()] = p;
    p->set_position(s);
}

bool Board::is_square_empty(Square s) const {
    if (!s.is_valid()) return true;
    return board_[s.row()][s.col()] == nullptr;
}

Piece* Board::get_piece(Square s) const {
    if (!s.is_valid()) return nullptr;
    return board_[s.row()][s.col()];
}

bool Board::path_clear(Square from, Square to) const {
    int dr = to.row() - from.row();
    int dc = to.col() - from.col();
    int sr = (dr > 0) - (dr < 0);
    int sc = (dc > 0) - (dc < 0);
    int r = from.row() + sr;
    int c = from.col() + sc;
    while (r != to.row() || c != to.col()) {
        if (board_[r][c] != nullptr) return false;
        r += sr;
        c += sc;
    }
    return true;
}

Square Board::find_king(Color c) const {
    for (const auto& p : pieces_[to_index(c)]) {
        if (p->is_captured()) continue;
        if (p->piece_letter() == 'K') return p->get_position();
    }
    return Square();
}

bool Board::is_in_check(Color c) const {
    Square king_pos = find_king(c);
    if (!king_pos.is_valid()) return false;
    for (const auto& p : pieces_[to_index(opposite(c))]) {
        if (p->is_captured()) continue;
        if (p->is_legal_move(p->get_position(), king_pos, *this)) return true;
    }
    return false;
}

void Board::display() const {
    const std::string blank_cell(5, ' ');
    std::cout << '\n';
    std::cout << "     a     b     c     d     e     f     g     h    \n";
    std::cout << "  +-----+-----+-----+-----+-----+-----+-----+-----+\n";
    for (int i = BOARD_SIZE - 1; i >= 0; i--) {
        std::cout << i + 1 << " ";
        for (int j = 0; j < BOARD_SIZE; j++) {
            std::cout << "|";
            if (board_[i][j]) {
                std::cout << "  ";
                board_[i][j]->display();
                std::cout << "  ";
            } else {
                std::cout << blank_cell;
            }
        }
        std::cout << "|\n  +-----+-----+-----+-----+-----+-----+-----+-----+\n";
    }
}

std::string Board::canonical_position() const {
    std::string out;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (Piece* p = board_[row][col]) {
                out += (p->get_color() == Color::White) ? 'w' : 'b';
                out += p->piece_letter();
            }
            out += ',';
        }
    }
    return out;
}

std::unique_ptr<Piece> Board::create_promoted_piece(Color c, char which, Square s) {
    int next_id = 100 + static_cast<int>(pieces_[0].size() + pieces_[1].size());
    const std::string white_glyphs[4] = {"♕", "♖", "♗", "♘"};  // Q R B N
    const std::string black_glyphs[4] = {"♛", "♜", "♝", "♞"};
    const std::string* glyphs = (c == Color::White) ? white_glyphs : black_glyphs;
    switch (which) {
    case 'R':
    case 'r':
        return std::make_unique<Rook>(c, glyphs[1], next_id, s);
    case 'B':
    case 'b':
        return std::make_unique<Bishop>(c, glyphs[2], next_id, s);
    case 'N':
    case 'n':
        return std::make_unique<Knight>(c, glyphs[3], next_id, s);
    case 'Q':
    case 'q':
    default:
        return std::make_unique<Queen>(c, glyphs[0], next_id, s);
    }
}

// ----- Move execution ------------------------------------------------------

MoveUndo Board::execute_move(const Move& m) {
    MoveUndo u;
    u.move = m;
    u.last_move_before = last_move_;

    if (m.is_kingside_castle || m.is_queenside_castle) {
        execute_castling(m, u);
    } else {
        execute_normal_move(m, u);
    }
    return u;
}

void Board::execute_castling(const Move& m, MoveUndo& u) {
    const int row = m.from.row();
    Piece* king = board_[row][4];
    u.moved_piece = king;
    u.moved_had_moved_before = king->has_moved();

    const int rook_from_col = m.is_kingside_castle ? 7 : 0;
    const int rook_to_col = m.is_kingside_castle ? 5 : 3;
    const int king_to_col = m.is_kingside_castle ? 6 : 2;

    Piece* rook = board_[row][rook_from_col];
    u.castling_rook = rook;
    u.rook_from = Square(row, rook_from_col);
    u.rook_to = Square(row, rook_to_col);
    u.rook_had_moved_before = rook->has_moved();

    board_[row][4] = nullptr;
    board_[row][rook_from_col] = nullptr;
    place_piece(king, Square(row, king_to_col));
    place_piece(rook, Square(row, rook_to_col));
    king->mark_moved();
    rook->mark_moved();

    last_move_ = {};
}

void Board::execute_normal_move(const Move& m, MoveUndo& u) {
    Piece* p = board_[m.from.row()][m.from.col()];
    u.moved_piece = p;
    u.moved_had_moved_before = p->has_moved();

    // Capture: en passant captures behind `to`, regular capture lands on `to`.
    if (m.is_en_passant) {
        const int captured_row = m.from.row();
        const int captured_col = m.to.col();
        Piece* victim = board_[captured_row][captured_col];
        u.captured_piece = victim;
        u.capture_square = Square(captured_row, captured_col);
        board_[captured_row][captured_col] = nullptr;
        if (victim) victim->mark_captured();
    } else if (Piece* victim = board_[m.to.row()][m.to.col()]) {
        u.captured_piece = victim;
        u.capture_square = m.to;
        victim->mark_captured();
    }

    board_[m.from.row()][m.from.col()] = nullptr;
    board_[m.to.row()][m.to.col()] = p;
    p->set_position(m.to);
    p->mark_moved();

    if (m.is_promotion) {
        u.pawn_before_promotion = p;
        auto new_piece_uptr = create_promoted_piece(p->get_color(), m.promo_piece, m.to);
        Piece* raw_new = new_piece_uptr.get();
        u.new_promoted_piece = raw_new;
        board_[m.to.row()][m.to.col()] = raw_new;
        raw_new->mark_moved();
        p->mark_captured();  // the pawn is now inactive (replaced by raw_new)
        pieces_[to_index(p->get_color())].push_back(std::move(new_piece_uptr));
    }

    // Record the en-passant target if this was a double push.
    if (u.moved_piece->piece_letter() == 'P' && std::abs(m.to.row() - m.from.row()) == 2) {
        last_move_ = LastMove{
            true,
            Square((m.from.row() + m.to.row()) / 2, m.from.col()),
            u.moved_piece->get_color(),
        };
    } else {
        last_move_ = {};
    }
}

void Board::undo_move(const MoveUndo& u) {
    last_move_ = u.last_move_before;
    if (u.move.is_kingside_castle || u.move.is_queenside_castle) {
        undo_castling(u);
    } else {
        undo_normal_move(u);
    }
}

void Board::undo_castling(const MoveUndo& u) {
    const int row = u.move.from.row();
    Piece* king = u.moved_piece;
    Piece* rook = u.castling_rook;

    const int king_to_col = u.move.is_kingside_castle ? 6 : 2;
    board_[row][king_to_col] = nullptr;
    board_[u.rook_to.row()][u.rook_to.col()] = nullptr;

    board_[row][4] = king;
    king->set_position(Square(row, 4));
    if (!u.moved_had_moved_before) king->unmark_moved();

    board_[u.rook_from.row()][u.rook_from.col()] = rook;
    rook->set_position(u.rook_from);
    if (!u.rook_had_moved_before) rook->unmark_moved();
}

void Board::undo_normal_move(const MoveUndo& u) {
    // Drop the promoted piece and put the pawn back on the destination first.
    // Order matters here: the captured-piece restore below writes to the
    // capture square, which equals `to` for normal captures.
    if (u.move.is_promotion) {
        auto& vec = pieces_[to_index(u.moved_piece->get_color())];
        auto it = std::find_if(vec.begin(), vec.end(), [&](const std::unique_ptr<Piece>& uptr) {
            return uptr.get() == u.new_promoted_piece;
        });
        if (it != vec.end()) vec.erase(it);  // unique_ptr's dtor frees
        u.pawn_before_promotion->mark_alive();
        board_[u.move.to.row()][u.move.to.col()] = u.pawn_before_promotion;
    }

    Piece* p = u.moved_piece;
    board_[u.move.to.row()][u.move.to.col()] = nullptr;
    board_[u.move.from.row()][u.move.from.col()] = p;
    p->set_position(u.move.from);
    if (!u.moved_had_moved_before) p->unmark_moved();

    if (u.captured_piece) {
        board_[u.capture_square.row()][u.capture_square.col()] = u.captured_piece;
        u.captured_piece->mark_alive();
        u.captured_piece->set_position(u.capture_square);
    }
}

bool Board::has_any_legal_move(Color c) {
    bool found = false;
    for_each_legal_move(c, /*expand_promotions=*/false, [&](const Move&) {
        found = true;
        return false;  // first legal move is enough; bail out
    });
    return found;
}
