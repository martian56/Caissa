#!/usr/bin/env bash
# Wrapper that runs the course's test-level.sh against the chess binary
# for all four levels. Captures each level's stdout and fails (exit 1) if
# any level reports a "failed tests:" line.
#
# Usage:  bash tests/run_all_levels.sh path/to/chess[.exe]
#
# Designed to be invoked from the project root, but it `cd`s to its own
# directory so the test-level.sh script can find ./data/.

set -u

if [ $# -lt 1 ]; then
    echo "usage: $0 path/to/chess[.exe]" >&2
    exit 2
fi
BIN="$1"

# Resolve to an absolute path BEFORE we cd, so a relative argument keeps
# pointing at the right file.
case "$BIN" in
    /*|?:*|?:[/\\]*) ABS_BIN="$BIN" ;;
    *)               ABS_BIN="$(pwd)/$BIN" ;;
esac

# test-level.sh requires its working directory to contain data/.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

if [ ! -x "$ABS_BIN" ]; then
    echo "Binary not found or not executable: $ABS_BIN" >&2
    exit 2
fi

fail=0
for level in 1 2 3 4; do
    echo ""
    echo "===== Level $level ====="
    out=$(bash test-level.sh "$level" "$ABS_BIN" 2>&1)
    echo "$out"
    if echo "$out" | grep -q "failed tests:"; then
        fail=1
    fi
done

echo ""
if [ "$fail" -ne 0 ]; then
    echo "*** Some tests FAILED."
    exit 1
fi
echo "All levels passed."
