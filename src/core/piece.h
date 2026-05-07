#pragma once
#include "square.h"
#include <string>

/// Player color. Scoped enum so it can't silently convert to int.
enum class Color { White = 0, Black = 1 };

inline Color opposite(Color c) {
    return c == Color::White ? Color::Black : Color::White;
}
inline int to_index(Color c) {
    return static_cast<int>(c);
}

class Board;

/**
 * @brief Abstract base class for all chess pieces.
 *
 * Each subclass implements is_legal_move() with its movement rule and
 * piece_letter() with its PGN letter. Pieces are owned by Board via
 * std::unique_ptr; once captured they stay in the roster but get a flag
 * so I can iterate "live" pieces cheaply.
 */
class Piece {
protected:
    Color color_;
    std::string name_;  ///< UTF-8 glyph used for display
    int id_;
    Square position_;
    bool has_moved_;  ///< needed for castling and the pawn double-push
    bool captured_;

public:
    Piece(Color c, std::string name, int id, Square pos);
    virtual ~Piece() = default;

    Color get_color() const { return color_; }
    Square get_position() const { return position_; }
    void set_position(Square s) { position_ = s; }
    int get_id() const { return id_; }
    std::string to_string() const { return name_; }

    bool has_moved() const { return has_moved_; }
    void mark_moved() { has_moved_ = true; }
    void unmark_moved() { has_moved_ = false; }  ///< used by undo_move

    bool is_captured() const { return captured_; }
    void mark_captured() { captured_ = true; }
    void mark_alive() { captured_ = false; }

    /// Print the UTF-8 glyph to stdout.
    void display() const;

    /**
     * @brief Geometric legality of moving from `from` to `to`.
     * @param from   Source square (this->get_position()).
     * @param to     Destination.
     * @param board  Used for path-clear checks and en-passant lookup.
     * @return true if the move pattern is legal for this piece type.
     *
     * Does NOT check friendly capture or self-check — Game does that.
     */
    virtual bool is_legal_move(Square from, Square to, const Board& board) const = 0;

    /// PGN letter: 'P','N','B','R','Q','K'. Used by canonical_position().
    virtual char piece_letter() const = 0;
};
