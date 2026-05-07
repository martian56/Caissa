#!/usr/bin/env bash
# Runs every .txt game file under tests/games/, pipes it into the chess
# binary (with comment lines stripped), and compares the final stdout line
# to the expected line (declared as `# expected: <line>` inside the file).
#
# Usage:  tests/run_tests.sh [path/to/binary]
# Default binary: ./bin/chess (or ./bin/chess.exe on Windows).

set -u

BIN="${1:-./bin/chess}"
if [ ! -x "$BIN" ] && [ -x "${BIN}.exe" ]; then
    BIN="${BIN}.exe"
fi
if [ ! -x "$BIN" ]; then
    echo "Binary not found or not executable: $BIN" >&2
    exit 2
fi

PASS=0
FAIL=0
SKIP=0
FAILED_FILES=()

# Use `find` rather than globstar — portable across older bash builds.
GAMES=$(find tests/games -type f -name '*.txt' | sort)

for game in $GAMES; do
    [ -f "$game" ] || continue

    expected=$(grep -E '^# expected: ' "$game" | sed 's/^# expected: //' | head -1)
    if [ -z "$expected" ]; then
        printf 'SKIP  %s  (no "# expected:" line)\n' "$game"
        SKIP=$((SKIP + 1))
        continue
    fi

    actual=$(grep -v '^#' "$game" | "$BIN" 2>/dev/null | tail -1)

    if [ "$actual" = "$expected" ]; then
        printf 'PASS  %s\n' "$game"
        PASS=$((PASS + 1))
    else
        printf 'FAIL  %s\n' "$game"
        printf '      expected: %s\n' "$expected"
        printf '      actual:   %s\n' "$actual"
        FAIL=$((FAIL + 1))
        FAILED_FILES+=("$game")
    fi
done

echo "----------------------------------------"
echo "PASS: $PASS    FAIL: $FAIL    SKIP: $SKIP"

if [ $FAIL -gt 0 ]; then
    echo "Failed files:"
    for f in "${FAILED_FILES[@]}"; do echo "  $f"; done
    exit 1
fi
exit 0
