#include "game.h"

#include <iostream>
#include <regex>
#include <string>
#include <cctype>

Game::Game() : to_move_(Color::White), result_("?-?"), finished_(false) {}

Game::~Game() = default;

void Game::display() const {
    board_.display();
}

// --- Input pattern helpers --------------------------------------------------

namespace {

bool is_valid_move_input(const std::string& cmd) {
    static const std::regex pattern("[a-h][1-8][a-h][1-8][qrbnQRBN]?");
    return std::regex_match(cmd, pattern);
}

bool is_kingside_castle_input(const std::string& cmd) {
    static const std::regex pattern("(O|o|0)-(O|o|0)");
    return std::regex_match(cmd, pattern);
}

bool is_queenside_castle_input(const std::string& cmd) {
    static const std::regex pattern("(O|o|0)-(O|o|0)-(O|o|0)");
    return std::regex_match(cmd, pattern);
}

}  // namespace

// ---------------------------------------------------------------------------

bool Game::try_move(const Square& from, const Square& to, char promo_inline) {
    Piece* p = board_.get_piece(from);
    if (!p) {
        std::cout << "Error: no piece on origin square " << from.to_string() << ".\n";
        return false;
    }
    if (p->get_color() != to_move_) {
        std::cout << "Error: piece on " << from.to_string() << " does not belong to "
                  << (to_move_ == Color::White ? "white" : "black") << ".\n";
        return false;
    }
    if (!p->is_legal_move(from, to, board_)) {
        std::cout << "Error: illegal move for that piece.\n";
        return false;
    }
    Piece* target = board_.get_piece(to);
    if (target && target->get_color() == to_move_) {
        std::cout << "Error: destination occupied by your own piece.\n";
        return false;
    }

    const bool is_ep =
        (p->piece_letter() == 'P') && (target == nullptr) && (from.col() != to.col());

    const bool is_promo_rank =
        (p->piece_letter() == 'P') && ((to_move_ == Color::White && to.row() == 7) ||
                                       (to_move_ == Color::Black && to.row() == 0));

    Move m(from, to);
    if (is_ep) m.is_en_passant = true;
    if (is_promo_rank) {
        m.is_promotion = true;
        char which = promo_inline;
        if (which == 0) {
            std::cout << "Promote to (Q/R/B/N)? ";
            std::cin >> which;
        }
        which = static_cast<char>(std::toupper(static_cast<unsigned char>(which)));
        if (which != 'Q' && which != 'R' && which != 'B' && which != 'N') which = 'Q';
        m.promo_piece = which;
    }

    MoveUndo u = board_.execute_move(m);
    if (board_.is_in_check(to_move_)) {
        board_.undo_move(u);
        std::cout << "Error: move leaves your king in check.\n";
        return false;
    }

    const std::string glyph = m.is_promotion ? board_.get_piece(to)->to_string() : p->to_string();
    std::cout << "-> move of " << glyph << " " << from.to_string() << to.to_string();
    if (m.is_promotion) std::cout << " (=" << m.promo_piece << ")";
    std::cout << "\n";
    return true;
}

bool Game::handle_castling(bool queenside) {
    const int row = (to_move_ == Color::White) ? 0 : 7;
    const int rook_col = queenside ? 0 : 7;

    Piece* king = board_.get_piece(Square(row, 4));
    Piece* rook = board_.get_piece(Square(row, rook_col));
    if (!king || king->piece_letter() != 'K' || king->has_moved()) {
        std::cout << "Error: castling impossible (king moved or missing).\n";
        return false;
    }
    if (!rook || rook->piece_letter() != 'R' || rook->has_moved()) {
        std::cout << "Error: castling impossible (rook moved or missing).\n";
        return false;
    }
    const int c_start = queenside ? 1 : 5;
    const int c_end = queenside ? 3 : 6;
    for (int c = c_start; c <= c_end; c++) {
        if (board_.get_piece(Square(row, c))) {
            std::cout << "Error: castling impossible (path blocked).\n";
            return false;
        }
    }
    if (board_.is_in_check(to_move_)) {
        std::cout << "Error: castling impossible (king in check).\n";
        return false;
    }

    Move m;
    m.from = Square(row, 4);
    m.to = Square(row, queenside ? 2 : 6);
    m.is_kingside_castle = !queenside;
    m.is_queenside_castle = queenside;

    MoveUndo u = board_.execute_move(m);
    if (board_.is_in_check(to_move_)) {
        board_.undo_move(u);
        std::cout << "Error: castling impossible (king ends in check).\n";
        return false;
    }
    std::cout << "-> castling " << (queenside ? "O-O-O" : "O-O") << "\n";
    return true;
}

void Game::check_end_of_game() {
    // Called right after the turn flip — to_move_ is now the side that has to play.
    const bool in_check = board_.is_in_check(to_move_);
    const bool has_a_move = board_.has_any_legal_move(to_move_);

    if (!has_a_move) {
        if (in_check) {
            result_ = (to_move_ == Color::White) ? "0-1" : "1-0";
            std::cout << "** Checkmate! " << (to_move_ == Color::White ? "Black" : "White")
                      << " wins.\n";
        } else {
            result_ = "1/2-1/2";
            std::cout << "** Stalemate. Draw.\n";
        }
        finished_ = true;
    } else if (in_check) {
        std::cout << "** Check!\n";
    }
}

void Game::print_final_result() {
    // Leading '\n' so the result line isn't glued onto a half-printed prompt.
    std::cout << '\n' << board_.canonical_position() << " " << result_ << "\n";
}

bool Game::play_external_move(const Move& m) {
    if (!m.from.is_valid() || !m.to.is_valid()) {
        // No legal move — game has already ended; emit final result.
        print_final_result();
        return true;
    }
    board_.execute_move(m);
    std::cout << "-> AI move " << m.from.to_string() << m.to.to_string();
    if (m.is_promotion) std::cout << " (=" << m.promo_piece << ")";
    if (m.is_kingside_castle) std::cout << " (O-O)";
    if (m.is_queenside_castle) std::cout << " (O-O-O)";
    std::cout << "\n";
    to_move_ = opposite(to_move_);
    check_end_of_game();
    if (finished_) {
        print_final_result();
        return true;
    }
    return false;
}

bool Game::play_turn() {
    std::string cmd;

    std::cout << ((to_move_ == Color::White) ? "[White] " : "[Black] ") << "Move (eg. a1a8) ? ";
    if (!(std::cin >> cmd)) {
        // stdin closed (EOF) — treat as quit.
        finished_ = true;
        print_final_result();
        return true;
    }

    if (cmd == "/quit") {
        result_ = "?-?";
        finished_ = true;
        print_final_result();
        return true;
    }
    if (cmd == "/resign") {
        result_ = (to_move_ == Color::White) ? "0-1" : "1-0";
        std::cout << "** " << (to_move_ == Color::White ? "White" : "Black") << " resigns.\n";
        finished_ = true;
        print_final_result();
        return true;
    }
    if (cmd == "/draw") {
        result_ = "1/2-1/2";
        std::cout << "** Draw agreed.\n";
        finished_ = true;
        print_final_result();
        return true;
    }

    if (is_queenside_castle_input(cmd)) {
        if (handle_castling(/*queenside=*/true)) {
            to_move_ = opposite(to_move_);
            check_end_of_game();
        }
    } else if (is_kingside_castle_input(cmd)) {
        if (handle_castling(/*queenside=*/false)) {
            to_move_ = opposite(to_move_);
            check_end_of_game();
        }
    } else if (is_valid_move_input(cmd)) {
        const std::string from_str = cmd.substr(0, 2);
        const std::string to_str = cmd.substr(2, 2);
        const char promo = (cmd.size() == 5) ? cmd[4] : 0;
        const Square from(from_str), to(to_str);
        if (try_move(from, to, promo)) {
            to_move_ = opposite(to_move_);
            check_end_of_game();
        }
    } else {
        std::cout << "Error: invalid input. Use e.g. e2e4, O-O, O-O-O, /quit, /resign, /draw.\n";
    }

    if (finished_) {
        print_final_result();
        return true;
    }
    return false;
}
