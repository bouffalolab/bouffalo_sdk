#!/bin/bash
# Pack example-local LittleFS content into build/build_out/littlefs.bin.

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
SDK_BASE=$(cd "${SCRIPT_DIR}/../.." && pwd)
DEFCONFIG="${SCRIPT_DIR}/defconfig"
BUILD_OUT="${SCRIPT_DIR}/build/build_out"
LFS_CONTENT_DIR="${SCRIPT_DIR}/lfs"
OUTPUT_IMAGE="${BUILD_OUT}/littlefs.bin"
DEFAULT_PTFILE="${SDK_BASE}/bsp/board/bl616dk/config/partition_cfg_4M.toml"
FLASH_COMX=""
SOURCE_DIRS=()

MKLFS="${SDK_BASE}/tools/genlfs/mklfs-ubuntu"
FLASH_TOOL="${SDK_BASE}/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu"

info() { echo "[INFO] $1"; }
error() { echo "[ERROR] $1" >&2; exit 1; }

while [[ $# -gt 0 ]]; do
    case "$1" in
        --flash)
            [[ $# -ge 2 ]] || error "--flash requires a serial port"
            FLASH_COMX="$2"
            shift 2
            ;;
        -h|--help)
            cat <<'USAGE'
Usage: ./pack_js_media.sh [DIR...] [--flash COMX]

Pack directories from examples/mquickjs/lfs into build/build_out/littlefs.bin.

Arguments:
  DIR             Directory name under lfs to include. Defaults to all.

Options:
  --flash COMX    Flash only the media partition after packing
  -h, --help      Show this help message

Examples:
  ./pack_js_media.sh
  ./pack_js_media.sh tests2
  ./pack_js_media.sh tests tests2 --flash /dev/ttyUSB0
USAGE
            exit 0
            ;;
        -*)
            error "Unknown option: $1"
            ;;
        *)
            SOURCE_DIRS+=("$1")
            shift
            ;;
    esac
done

find_tool() {
    local path="$1"
    shift
    if [[ -x "$path" ]]; then
        printf '%s\n' "$path"
        return 0
    fi
    local candidate
    for candidate in "$@"; do
        if [[ -x "$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

MKLFS=$(find_tool "$MKLFS" \
    "${SDK_BASE}/tools/genlfs/mklfs-ubuntu" \
    "${SDK_BASE}/tools/genlfs/mklfs-macos" \
    "${SDK_BASE}/tools/genlfs/mklfs.exe") || error "mklfs tool not found"

FLASH_TOOL=$(find_tool "$FLASH_TOOL" \
    "${SDK_BASE}/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu" \
    "${SDK_BASE}/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-macos" \
    "${SDK_BASE}/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-arm" \
    "${SDK_BASE}/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand.exe" || true)

[[ -d "$LFS_CONTENT_DIR" ]] || error "LittleFS source directory not found: $LFS_CONTENT_DIR"

if [[ ${#SOURCE_DIRS[@]} -eq 0 ]]; then
    while IFS= read -r entry; do
        SOURCE_DIRS+=("$(basename "$entry")")
    done < <(find "$LFS_CONTENT_DIR" -mindepth 1 -maxdepth 1 -type d | sort)
fi

[[ ${#SOURCE_DIRS[@]} -gt 0 ]] || error "No source directories found under $LFS_CONTENT_DIR"

PTFILE_REL=$(sed -n 's/^CONFIG_OVERRIDE_PTFILE[[:space:]]*=[[:space:]]*//p' "$DEFCONFIG" | tail -n1 | tr -d '[:space:]')
if [[ -n "$PTFILE_REL" ]]; then
    PTFILE="${SCRIPT_DIR}/${PTFILE_REL}"
    PTFILE_LABEL="${PTFILE_REL}"
else
    PTFILE="${DEFAULT_PTFILE}"
    PTFILE_LABEL="bsp/board/bl616dk/config/partition_cfg_4M.toml"
fi
[[ -f "$PTFILE" ]] || error "Partition file not found: $PTFILE"

partition_field() {
    local section="$1"
    local key="$2"
    awk -v section="$section" -v key="$key" '
        /^\[\[pt_entry\]\]/ { active=0; next }
        $1 == "name" {
            gsub(/"/, "", $3)
            active = ($3 == section)
            next
        }
        active && $1 == key { print $3; exit }
    ' "$PTFILE"
}

MEDIA_ADDR=$(partition_field media address0)
MEDIA_SIZE_HEX=$(partition_field media size0)

[[ -n "$MEDIA_ADDR" ]] || error "media.address0 not found in $PTFILE"
[[ -n "$MEDIA_SIZE_HEX" ]] || error "media.size0 not found in $PTFILE"

mkdir -p "$BUILD_OUT"

TMP_DIR=$(mktemp -d)
FLASH_CFG=""
cleanup() {
    rm -rf "$TMP_DIR"
    if [[ -n "$FLASH_CFG" ]]; then
        rm -f "$FLASH_CFG"
    fi
}
trap cleanup EXIT

STAGE_DIR="${TMP_DIR}/lfs"
mkdir -p "$STAGE_DIR"

for dir in "${SOURCE_DIRS[@]}"; do
    SRC="${LFS_CONTENT_DIR}/${dir}"
    [[ -d "$SRC" ]] || error "Directory not found under lfs: ${dir}"
    cp -a "$SRC" "$STAGE_DIR/"
    info "Included ${dir}/"
done

info "Using partition file: ${PTFILE_LABEL}"
info "Media address: ${MEDIA_ADDR}"
info "Media size: ${MEDIA_SIZE_HEX} ($((MEDIA_SIZE_HEX / 1024)) KB)"
info "Generating ${OUTPUT_IMAGE}"

(
    cd "$TMP_DIR"
    "$MKLFS" -c lfs \
        -b 4096 \
        -r 256 \
        -p 256 \
        -s "$((MEDIA_SIZE_HEX))" \
        -i "$OUTPUT_IMAGE"
)

IMAGE_SIZE=$(stat -c%s "$OUTPUT_IMAGE" 2>/dev/null || stat -f%z "$OUTPUT_IMAGE")
info "Generated ${OUTPUT_IMAGE} (${IMAGE_SIZE} bytes)"

if [[ -n "$FLASH_COMX" ]]; then
    [[ -n "$FLASH_TOOL" && -x "$FLASH_TOOL" ]] || error "Flash tool not found"
    FLASH_CFG=$(mktemp)
    cat > "$FLASH_CFG" <<CFG
[cfg]
erase = 1
skip_mode = 0x0, 0x0
boot2_isp_mode = 0

[media]
filedir = ${OUTPUT_IMAGE}
address = ${MEDIA_ADDR}
CFG
    info "Flashing media to ${FLASH_COMX}"
    "$FLASH_TOOL" --interface=uart --baudrate=2000000 \
        --port="$FLASH_COMX" --chipname=bl616 --config="$FLASH_CFG"
fi
