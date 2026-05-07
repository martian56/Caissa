---
marp: true
theme: default
paginate: true
size: 16:9
header: 'Caissa: OOP2 final'
footer: 'Devlaner · 2026-05'
---

<!-- _class: lead -->

# Caissa

Text-mode chess in C++

OOP2 final · Presentation

---

## Goal

Two humans play chess in a terminal. Moves typed in algebraic notation
(`e2e4`). Game prints the board after each move and a final result line
the grading script can parse.

Four levels:

1. Display + basic moves
2. Check
3. Castling, en passant, promotion
4. Mate / stalemate

Bonus: an AI you can play against.

---

## How it's wired

```
   main.cpp
       │
       ▼
     Game ─────> Board ─────> Piece (abstract)
                                │
                                ├── Rook
                                ├── Knight
                                ├── Bishop
                                ├── Queen
                                ├── King
                                └── Pawn

   ── separate, optional ──────────────────────────

     Engine ──> Search ──> Evaluator
                  │
                  └─> MoveGenerator ──> Board
```

One-way dependency: engine knows about core, core doesn't know about engine.

---

## OOP

- `Piece` is abstract, two pure-virtual methods (`is_legal_move`, `piece_letter`).
- One subclass per piece type, each in its own `.cpp` file.
- `Board` only ever holds `Piece*` and calls virtual methods. No
  `switch` on type anywhere.
- `enum class Color { White, Black }` instead of a plain enum, so I
  can't accidentally treat it as an int.
- Pieces owned by `std::unique_ptr`. Destructor is defaulted.

---

## Move + undo

`Board::execute_move(Move)` returns a `MoveUndo`.
`Board::undo_move(MoveUndo)` reverses the move exactly.

Used in three places:

1. Game tries the move, undoes if it leaves my king in check.
2. `has_any_legal_move` (mate / stalemate detection).
3. The engine's alpha-beta search.

Captured pieces stay in the roster with a flag, so undo is allocation-free.
That's important for the search.

---

## Level 1: basic moves

- Movement rules per piece (rook on lines, bishop on diagonals, …)
- Pawn: 1-step, 2-step from start row, diagonal capture
- No jumping over pieces (path-clear check for sliders)
- Friendly capture rejected
- `/quit` → `?-?`, `/resign` → `0-1` or `1-0`, `/draw` → `1/2-1/2`
- Final stdout line: `<canonical_position> <result>`

```
[White] Move (eg. a1a8) ? e2e4
-> move of ♙ e2e4
```

---

## Level 2: check

Two things to add:

- Print `** Check!` after a move that attacks the opposing king.
- Reject any move that leaves my own king in check.

`Board::is_in_check(c)` loops over the opponent's pieces and asks each
one whether it could reach the king. That's literally what
`is_legal_move` already answers.

The self-check filter is just `execute_move → is_in_check → undo_move`.

---

## Level 3: castling, en passant, promotion

Castling
: `O-O` / `O-O-O`. King and rook unmoved, path clear, king not in check.

En passant
: `Board::last_move` records double pawn pushes. `Pawn::is_legal_move`
  treats a diagonal step to an empty square as a capture if it matches.

Promotion
: At the last rank the program asks `Promote to (Q/R/B/N)?`. The pawn
  gets replaced by a freshly-allocated piece in the roster.

---

## Level 4: mate / stalemate

After flipping the turn:

```
in_check    = is_in_check(side_to_move)
has_a_move  = has_any_legal_move(side_to_move)

if !has_a_move:
    if in_check:   1-0 or 0-1   (mate)
    else:          1/2-1/2      (stalemate)
```

`has_any_legal_move` walks every piece and every target square through
the same try-then-undo path Game uses for normal validation. It stops
on the first move that keeps the king safe.

---

## Bonus: AI

Files in `src/engine/`:

- `MoveGenerator` — collects legal moves
- `Evaluator` — material only (P=100, N=320, B=330, R=500, Q=900)
- `Search` — negamax alpha-beta, depth 3
- `Engine` — public entry point: `best_move(board, color)`

Linked only into the `chess-ai` binary. The plain `chess` binary doesn't
include any of this.

---

## Optional, for real

```bash
rm -rf src/engine
make chess        # still builds
make test         # 9 / 9 pass
```

How that's possible:

- `Game::play_external_move(Move)` is generic, no AI knowledge.
- The AI is wired in `main.cpp` behind `#ifdef WITH_AI`.

First version put an `Engine*` member on `Game` behind an ifdef, but
`game.cpp` only gets compiled once for both binaries — moving the hook
to `main.cpp` was the fix.

---

## Build / test / docs

```bash
make            # both binaries
make chess      # core only, what graders run
make chess-ai   # core + AI

make test       # 9 piped games, diff against `# expected:`
make format     # clang-format
make doc        # Doxygen → docs/api/
```

Same Makefile works in cmd.exe, PowerShell, Git Bash, Linux, macOS.
Shell detection at the top picks the right `rm` / `mkdir`.

Builds with `-Wall -Wextra -Wpedantic`, zero warnings.

---

## Tests

| Level | File | Covers |
|------:|------|--------|
| 1 | `smoke1.txt`        | Italian Game opening + `/quit` |
| 1 | `resign.txt`        | `/resign` → `0-1` |
| 1 | `draw.txt`          | `/draw` → `1/2-1/2` |
| 2 | `check_response.txt`| Forced response to check |
| 3 | `castling.txt`      | `O-O` |
| 3 | `enpassant.txt`     | En passant |
| 3 | `promote.txt`       | Capture-promotion to Queen |
| 4 | `scholars.txt`      | Scholar's Mate, `1-0` |
| 4 | `stalemate.txt`     | Sam Loyd's 10-move stalemate |


## Demo

```bash
# Scholar's mate
cat tests/games/4-mate/scholars.txt | ./bin/chess

# Sam Loyd stalemate
cat tests/games/4-mate/stalemate.txt | ./bin/chess

# Play the engine
./bin/chess-ai
> Play vs AI? (y/n) y
> Play as (w/b) ? w
```

---

<!-- _class: lead -->

# Questions?

Code: `src/`
Tests: `tests/games/`
Docs: `make doc`
