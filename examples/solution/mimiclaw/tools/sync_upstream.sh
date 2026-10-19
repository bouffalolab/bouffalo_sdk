#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./tools/sync_upstream.sh                   # sync main (auto mode)
#   ./tools/sync_upstream.sh v0.2.0            # sync specific tag/branch
#   ./tools/sync_upstream.sh main --snapshot   # force snapshot mode
#   ./tools/sync_upstream.sh main --subtree    # force subtree mode

UPSTREAM_URL="${MIMICLAW_UPSTREAM_URL:-https://github.com/memovai/mimiclaw.git}"
UPSTREAM_REF="${1:-main}"
MODE="${2:-auto}"
REMOTE_NAME="mimiclaw-upstream"
PREFIX="examples/solution/mimiclaw/upstream"
TMP_CLONE="/tmp/mimiclaw_upstream_clone"

if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    echo "error: run inside git work tree"
    exit 1
fi

sync_by_snapshot() {
    rm -rf "$TMP_CLONE"
    git clone --depth 1 --branch "$UPSTREAM_REF" "$UPSTREAM_URL" "$TMP_CLONE"
    rm -rf "${PREFIX:?}/"*
    cp -a "$TMP_CLONE"/. "$PREFIX"/
    rm -rf "$PREFIX/.git"
}

sync_by_subtree() {
    if git remote get-url "$REMOTE_NAME" >/dev/null 2>&1; then
        git remote set-url "$REMOTE_NAME" "$UPSTREAM_URL"
    else
        git remote add "$REMOTE_NAME" "$UPSTREAM_URL"
    fi

    git fetch "$REMOTE_NAME" "$UPSTREAM_REF"

    if [ -z "$(find "$PREFIX" -mindepth 1 -maxdepth 1 -print -quit 2>/dev/null || true)" ]; then
        git subtree add --prefix="$PREFIX" "$REMOTE_NAME" "$UPSTREAM_REF" --squash
    else
        git subtree pull --prefix="$PREFIX" "$REMOTE_NAME" "$UPSTREAM_REF" --squash
    fi
}

case "$MODE" in
    --snapshot)
        sync_by_snapshot
        ;;
    --subtree)
        sync_by_subtree
        ;;
    auto)
        if git diff --quiet && git diff --cached --quiet; then
            sync_by_subtree
        else
            echo "working tree is dirty, fallback to snapshot sync"
            sync_by_snapshot
        fi
        ;;
    *)
        echo "error: unknown mode '$MODE'"
        echo "valid modes: auto | --snapshot | --subtree"
        exit 1
        ;;
esac

cat <<MSG
sync done.
local adaptation stays in:
  examples/solution/mimiclaw/app
  examples/solution/mimiclaw/port
MSG
