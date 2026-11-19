#!/bin/sh

# Source this file to make the host-side WAMR tools available:
#   . ./wasm_env.sh

WASI_SDK="${WASI_SDK:-$HOME/.local/opt/wasi-sdk}"
export WASI_SDK

case ":$PATH:" in
    *":$HOME/.local/bin:"*) ;;
    *)
        PATH="$HOME/.local/bin:$PATH"
        export PATH
        ;;
esac

if [ -z "${WAMRC:-}" ]; then
    WAMRC=$(command -v wamrc 2>/dev/null || true)
fi
if [ -n "${WAMRC:-}" ]; then
    export WAMRC
fi
