#!/bin/bash
#
# Build script for nethub kernel modules and userspace apps
# Usage:
#   ./build.sh build
#   ./build.sh <clean|load|unload>

set -e

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KERNEL_DIR="${SCRIPT_DIR}/../../kernel"
MODULE_NAME="mr_sdio"
OUTPUT_DIR="${SCRIPT_DIR}/output"
VCHAN_DIR="${SCRIPT_DIR}/virtualchan"
BFLBWIFICTRL_DIR="${SCRIPT_DIR}/bflbwifictrl"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Print usage
usage() {
    echo "Usage:"
    echo "  $0 build"
    echo "  $0 <clean|load|unload>"
    echo ""
    echo "Commands:"
    echo "  build  - Build kernel modules and userspace apps to output/"
    echo "  clean  - Clean build artifacts in kernel/, virtualchan/, bflbwifictrl/ and output/"
    echo "  load   - Load kernel module from output/ (sudo insmod)"
    echo "  unload - Unload kernel module (sudo rmmod)"
    echo "Examples:"
    echo "  $0 build"
    echo "  $0 clean"
    echo "  $0 load"
    echo "  FORCE=1 $0 load          # Reload module without interactive prompt"
    echo "  $0 unload"
    echo ""
    echo "Output directory: ${OUTPUT_DIR}/"
    echo ""
    exit 1
}

# Check if directories exist
check_dirs() {
    if [ ! -d "${KERNEL_DIR}" ]; then
        echo -e "${RED}Error: Kernel directory not found: ${KERNEL_DIR}${NC}"
        exit 1
    fi
    if [ ! -d "${VCHAN_DIR}" ]; then
        echo -e "${RED}Error: Virtualchan directory not found: ${VCHAN_DIR}${NC}"
        exit 1
    fi
    if [ ! -d "${BFLBWIFICTRL_DIR}" ]; then
        echo -e "${RED}Error: Bflbwifictrl directory not found: ${BFLBWIFICTRL_DIR}${NC}"
        exit 1
    fi
}

copy_artifact() {
    local src="$1"
    local dst="$2"
    local tmp_dst="${dst}.tmp.$$"

    if ! cp "${src}" "${tmp_dst}"; then
        rm -f "${tmp_dst}"
        return 1
    fi

    if ! mv -f "${tmp_dst}" "${dst}"; then
        rm -f "${tmp_dst}"
        return 1
    fi

    echo -e "${GREEN}    Copied: ${dst}${NC}"
    ls -lh "${dst}" | awk '{print "    Size: " $5}'
    return 0
}

run_make_step() {
    local step_name="$1"
    shift

    if ! "$@"; then
        echo -e "${RED}    Error: ${step_name} failed${NC}"
        exit 1
    fi
}

stop_process_by_pidfile() {
    local pid_file="$1"
    local proc_name="$2"
    local pid=""

    if [ -z "${pid_file}" ] || [ -z "${proc_name}" ]; then
        return 0
    fi

    if sudo test -f "${pid_file}"; then
        pid="$(sudo cat "${pid_file}" 2>/dev/null || true)"
    fi

    if [ -n "${pid}" ] && ps -p "${pid}" -o comm= 2>/dev/null | grep -qx "${proc_name}"; then
        echo -e "${YELLOW}    Stopping ${proc_name} (PID=${pid}) before unload${NC}"
        sudo kill "${pid}" 2>/dev/null || true
    fi
}

stop_processes_by_name() {
    local proc_name="$1"

    if [ -z "${proc_name}" ]; then
        return 0
    fi

    if pgrep -x "${proc_name}" >/dev/null 2>&1; then
        echo -e "${YELLOW}    Stopping ${proc_name} before unload${NC}"
        sudo pkill -x "${proc_name}" 2>/dev/null || true
    fi
}

stop_processes_by_pattern() {
    local pattern="$1"
    local display_name="$2"

    if [ -z "${pattern}" ] || [ -z "${display_name}" ]; then
        return 0
    fi

    if pgrep -f "${pattern}" >/dev/null 2>&1; then
        echo -e "${YELLOW}    Stopping ${display_name} before unload${NC}"
        sudo pkill -f "${pattern}" 2>/dev/null || true
    fi
}

wait_process_gone() {
    local pattern="$1"
    local timeout_sec="${2:-5}"
    local i=0

    if [ -z "${pattern}" ]; then
        return 0
    fi

    for ((i = 0; i < timeout_sec; i++)); do
        if ! pgrep -f "${pattern}" >/dev/null 2>&1; then
            return 0
        fi
        sleep 1
    done

    return 1
}

stop_known_module_users() {
    stop_process_by_pidfile "/var/run/bflbwifid.pid" "bflbwifid"
    stop_processes_by_name "bflbwifid"
    stop_processes_by_pattern "/nethub_vchan_app([[:space:]]|\$)" "nethub_vchan_app"

    if ! wait_process_gone "/bflbwifid([[:space:]]|\$)" 5; then
        echo -e "${YELLOW}    Warning: bflbwifid is still exiting${NC}"
    fi
    if ! wait_process_gone "/nethub_vchan_app([[:space:]]|\$)" 5; then
        echo -e "${YELLOW}    Warning: nethub_vchan_app is still exiting${NC}"
    fi
}

dump_unload_diagnostics() {
    echo -e "${YELLOW}    Current module users:${NC}"
    lsmod | grep "^${MODULE_NAME} " || true

    if pgrep -ax bflbwifid >/dev/null 2>&1; then
        echo -e "${YELLOW}    Active bflbwifid processes:${NC}"
        pgrep -ax bflbwifid || true
    fi

    if pgrep -f "/nethub_vchan_app([[:space:]]|\$)" >/dev/null 2>&1; then
        echo -e "${YELLOW}    Active nethub_vchan_app processes:${NC}"
        pgrep -af "/nethub_vchan_app([[:space:]]|\$)" || true
    fi
}

# Build kernel modules and userspace apps
do_build() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${GREEN}==> Building all artifacts to output/${NC}"
    echo -e "${CYAN}==> Kernel/userspace support: tty + vchan${NC}"
    echo -e "${BLUE}========================================${NC}"
    check_dirs

    # Create output directory
    mkdir -p "${OUTPUT_DIR}"
    rm -f "${OUTPUT_DIR}/.nethub_build.conf"
    echo -e "${GREEN}==> Created output directory: ${OUTPUT_DIR}${NC}"

    # Build kernel module
    echo ""
    echo -e "${YELLOW}==> [1/4] Building kernel module...${NC}"
    cd "${KERNEL_DIR}" || exit 1

    # Always build both control backends. Runtime selection happens in bflbwifid.
    make \
        CONFIG_MR_NETDEV=y \
        CONFIG_MR_NETDEV_WIFI=y \
        CONFIG_MR_VIRTUALCHAN=y \
        CONFIG_MR_TTY=y \
        CONFIG_MR_TTY_CMD=y
    cd - || exit 1

    # Copy kernel module to output
    if [ -f "${KERNEL_DIR}/${MODULE_NAME}.ko" ]; then
        copy_artifact "${KERNEL_DIR}/${MODULE_NAME}.ko" "${OUTPUT_DIR}/${MODULE_NAME}.ko"
    else
        echo -e "${RED}    Error: ${KERNEL_DIR}/${MODULE_NAME}.ko not found${NC}"
        exit 1
    fi

    # Build virtualchan userspace app
    echo ""
    echo -e "${YELLOW}==> [2/4] Building virtualchan userspace app...${NC}"
    cd "${VCHAN_DIR}" || exit 1
    run_make_step "virtualchan userspace build" make
    cd - || exit 1

    # Copy virtualchan app to output
    if [ -f "${VCHAN_DIR}/nethub_vchan_app" ]; then
        copy_artifact "${VCHAN_DIR}/nethub_vchan_app" "${OUTPUT_DIR}/nethub_vchan_app"
    else
        echo -e "${YELLOW}    Warning: ${VCHAN_DIR}/nethub_vchan_app not found${NC}"
    fi

    # Copy virtualchan library (always needed by unified userspace build)
    echo ""
    echo -e "${YELLOW}==> [3/4] Preparing virtualchan library...${NC}"
    if [ -f "${VCHAN_DIR}/libnethub_vchan.a" ]; then
        copy_artifact "${VCHAN_DIR}/libnethub_vchan.a" "${OUTPUT_DIR}/libnethub_vchan.a"
    else
        echo -e "${YELLOW}    Warning: ${VCHAN_DIR}/libnethub_vchan.a not found${NC}"
    fi

    # Build bflbwifictrl userspace app
    echo ""
    echo -e "${YELLOW}==> [4/4] Building bflbwifictrl userspace app (unified backend build)...${NC}"
    cd "${BFLBWIFICTRL_DIR}" || exit 1
    run_make_step "bflbwifictrl userspace build" make ENABLE_NETIF_AUTO_CONFIG=1
    cd - || exit 1

    # Copy bflbwifictrl apps from app/ subdirectory to output
    copied_count=0
    if [ -f "${BFLBWIFICTRL_DIR}/app/bflbwifictrl" ]; then
        copy_artifact "${BFLBWIFICTRL_DIR}/app/bflbwifictrl" "${OUTPUT_DIR}/bflbwifictrl"
        copied_count=$((copied_count + 1))
    else
        echo -e "${YELLOW}    Warning: ${BFLBWIFICTRL_DIR}/app/bflbwifictrl not found${NC}"
    fi

    if [ -f "${BFLBWIFICTRL_DIR}/app/bflbwifid" ]; then
        copy_artifact "${BFLBWIFICTRL_DIR}/app/bflbwifid" "${OUTPUT_DIR}/bflbwifid"
        copied_count=$((copied_count + 1))
    else
        echo -e "${YELLOW}    Warning: ${BFLBWIFICTRL_DIR}/app/bflbwifid not found${NC}"
    fi

    if [ $copied_count -eq 0 ]; then
        echo -e "${YELLOW}    No bflbwifictrl apps found to copy${NC}"
    fi

    # Summary
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${GREEN}==> Build complete!${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
    echo -e "${GREEN}Output files in ${OUTPUT_DIR}:${NC}"
    ls -lh "${OUTPUT_DIR}" | grep -E "${MODULE_NAME}.ko|nethub_vchan_app|bflbwifictrl|bflbwifid|libnethub_vchan.a" || echo "    (no files found)"
    echo ""
    echo -e "${YELLOW}Build Configuration:${NC}"
    echo -e "  Kernel backend support: ${CYAN}tty + vchan${NC}"
    echo -e "  Userspace backend support: ${CYAN}tty + vchan${NC}"
    echo -e "  Default daemon backend: ${CYAN}tty (/dev/ttyAT0)${NC}"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo -e "  1. Load module: ${GREEN}sudo $0 load${NC}"
    echo -e "  2. Run bflbwifid daemon (default tty): ${GREEN}sudo ${OUTPUT_DIR}/bflbwifid${NC}"
    echo -e "     Or select vchan explicitly: ${GREEN}sudo ${OUTPUT_DIR}/bflbwifid -c vchan${NC}"
    echo -e "     If tty device is not /dev/ttyAT0, use: ${GREEN}sudo ${OUTPUT_DIR}/bflbwifid -c tty -p <dev>${NC}"
    echo -e "  3. Run bflbwifictrl cli: ${GREEN}sudo ${OUTPUT_DIR}/bflbwifictrl${NC}"
    echo ""
}

# Clean build artifacts
do_clean() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${YELLOW}==> Cleaning build artifacts${NC}"
    echo -e "${BLUE}========================================${NC}"
    check_dirs

    # Clean kernel module build
    echo ""
    echo -e "${YELLOW}==> [1/5] Cleaning kernel module build...${NC}"
    cd "${KERNEL_DIR}" || exit 1
    make clean 2>/dev/null || true
    cd - || exit 1
    echo -e "${GREEN}    Done${NC}"

    # Clean virtualchan build
    echo ""
    echo -e "${YELLOW}==> [2/5] Cleaning virtualchan build...${NC}"
    cd "${VCHAN_DIR}" || exit 1
    make clean 2>/dev/null || echo -e "${YELLOW}    (No Makefile, skipping)${NC}"
    cd - || exit 1
    echo -e "${GREEN}    Done${NC}"

    # Clean bflbwifictrl build
    echo ""
    echo -e "${YELLOW}==> [3/5] Cleaning bflbwifictrl build...${NC}"
    cd "${BFLBWIFICTRL_DIR}" || exit 1
    make clean 2>/dev/null || echo -e "${YELLOW}    (No Makefile, skipping)${NC}"
    cd - || exit 1
    echo -e "${GREEN}    Done${NC}"

    echo ""
    echo -e "${YELLOW}==> [4/5] Cleaning generated libraries...${NC}"
    echo -e "${GREEN}    Done${NC}"

    # Clean output directory
    echo ""
    echo -e "${YELLOW}==> [5/5] Cleaning output directory...${NC}"
    if [ -d "${OUTPUT_DIR}" ]; then
        rm -rf "${OUTPUT_DIR}"
        echo -e "${GREEN}    Removed: ${OUTPUT_DIR}/*${NC}"
    fi
    echo -e "${GREEN}    Done${NC}"

    echo ""
    echo -e "${GREEN}==> Clean complete${NC}"
    echo ""
}

# Load kernel module
do_load() {
    echo -e "${GREEN}==> Loading kernel module from output/${NC}"

    # Check if module file exists in output
    if [ ! -f "${OUTPUT_DIR}/${MODULE_NAME}.ko" ]; then
        echo -e "${RED}Error: Module file not found: ${OUTPUT_DIR}/${MODULE_NAME}.ko${NC}"
        echo -e "${YELLOW}Hint: Run '$0 build' first${NC}"
        exit 1
    fi

    # Check if module is already loaded
    if lsmod | grep -q "^${MODULE_NAME} "; then
        echo -e "${YELLOW}Warning: Module ${MODULE_NAME} is already loaded${NC}"
        echo -e "${YELLOW}Hint: Run '$0 unload' first if you want to reload${NC}"
        if [ "${FORCE:-0}" = "1" ]; then
            echo -e "${YELLOW}FORCE=1 detected, reloading module automatically${NC}"
        elif [ ! -t 0 ]; then
            echo -e "${RED}Error: Non-interactive shell cannot confirm reload, use FORCE=1${NC}"
            exit 1
        else
            read -p "Reload module? [y/N] " -n 1 -r
            echo
            if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                echo "Cancelled"
                exit 0
            fi
        fi
        do_unload
    fi

    # Load module from output directory
    if sudo insmod "${OUTPUT_DIR}/${MODULE_NAME}.ko"; then
        echo -e "${GREEN}==> Module loaded successfully${NC}"
        lsmod | grep "^${MODULE_NAME} "
    else
        echo -e "${RED}==> Failed to load module${NC}"
        exit 1
    fi
}

# Unload kernel module
do_unload() {
    echo -e "${YELLOW}==> Unloading kernel module: ${MODULE_NAME}${NC}"

    # Check if module is loaded
    if ! lsmod | grep -q "^${MODULE_NAME} "; then
        echo -e "${YELLOW}Warning: Module ${MODULE_NAME} is not loaded${NC}"
        return 0
    fi

    stop_known_module_users

    # Unload module
    if sudo rmmod "${MODULE_NAME}"; then
        echo -e "${GREEN}==> Module unloaded successfully${NC}"
    else
        echo -e "${RED}==> Failed to unload module${NC}"
        dump_unload_diagnostics
        echo -e "${YELLOW}Hint: Check 'dmesg | tail' for errors${NC}"
        exit 1
    fi
}

# Main script logic
main() {
    local command=""
    local warned_legacy_channel=0

    # Show usage if no argument
    if [ $# -eq 0 ]; then
        usage
    fi

    for arg in "$@"; do
        case "$arg" in
            CHANNEL=*)
                if [ "${warned_legacy_channel}" -eq 0 ]; then
                    echo -e "${YELLOW}Warning: CHANNEL=... is deprecated and ignored; tty + vchan are always built${NC}"
                    warned_legacy_channel=1
                fi
                ;;
            *)
                if [ -z "${command}" ]; then
                    command="$arg"
                else
                    echo -e "${RED}Error: Unexpected argument '$arg'${NC}"
                    echo ""
                    usage
                fi
                ;;
        esac
    done

    # Execute command
    case "$command" in
        build)
            do_build
            ;;
        clean)
            do_clean
            ;;
        load)
            do_load
            ;;
        unload)
            do_unload
            ;;
        "")
            echo -e "${RED}Error: No command specified${NC}"
            echo ""
            usage
            ;;
        *)
            echo -e "${RED}Error: Unknown command '$command'${NC}"
            echo ""
            usage
            ;;
    esac
}

# Run main with all arguments
main "$@"
