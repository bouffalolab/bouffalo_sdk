#!/bin/bash
#
# Build script for nethub kernel modules and userspace apps
# Usage: ./build.sh <build|clean|load|unload>
#

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
NC='\033[0m' # No Color

# Print usage
usage() {
    echo "Usage: $0 <build|clean|load|unload>"
    echo ""
    echo "Commands:"
    echo "  build  - Build kernel modules and userspace apps to output/"
    echo "  clean  - Clean build artifacts in kernel/, virtualchan/, bflbwifictrl/ and output/"
    echo "  load   - Load kernel module from output/ (sudo insmod)"
    echo "  unload - Unload kernel module (sudo rmmod)"
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

# Build kernel modules and userspace apps
do_build() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${GREEN}==> Building all artifacts to output/${NC}"
    echo -e "${BLUE}========================================${NC}"
    check_dirs

    # Create output directory
    mkdir -p "${OUTPUT_DIR}"
    echo -e "${GREEN}==> Created output directory: ${OUTPUT_DIR}${NC}"

    # Build kernel module
    echo ""
    echo -e "${YELLOW}==> [1/4] Building kernel module...${NC}"
    cd "${KERNEL_DIR}" || exit 1
    make \
        CONFIG_MR_NETDEV=y \
        CONFIG_MR_NETDEV_WIFI=y \
        CONFIG_MR_VIRTUALCHAN=y \
        CONFIG_MR_TTY=y \
        CONFIG_MR_TTY_CMD=y
    cd - || exit 1

    # Copy kernel module to output
    if [ -f "${KERNEL_DIR}/${MODULE_NAME}.ko" ]; then
        cp "${KERNEL_DIR}/${MODULE_NAME}.ko" "${OUTPUT_DIR}/"
        echo -e "${GREEN}    Copied: ${OUTPUT_DIR}/${MODULE_NAME}.ko${NC}"
        ls -lh "${OUTPUT_DIR}/${MODULE_NAME}.ko" | awk '{print "    Size: " $5}'
    else
        echo -e "${RED}    Error: ${KERNEL_DIR}/${MODULE_NAME}.ko not found${NC}"
        exit 1
    fi

    # Build virtualchan userspace app
    echo ""
    echo -e "${YELLOW}==> [2/4] Building virtualchan userspace app...${NC}"
    cd "${VCHAN_DIR}" || exit 1
    make 2>/dev/null || echo -e "${YELLOW}    Warning: make failed or no Makefile${NC}"
    cd - || exit 1

    # Copy virtualchan app to output
    if [ -f "${VCHAN_DIR}/nethub_vchan_app" ]; then
        cp "${VCHAN_DIR}/nethub_vchan_app" "${OUTPUT_DIR}/"
        echo -e "${GREEN}    Copied: ${OUTPUT_DIR}/nethub_vchan_app${NC}"
        ls -lh "${OUTPUT_DIR}/nethub_vchan_app" | awk '{print "    Size: " $5}'
    else
        echo -e "${YELLOW}    Warning: ${VCHAN_DIR}/nethub_vchan_app not found${NC}"
    fi

    # Build bflbwifictrl userspace app
    echo ""
    echo -e "${YELLOW}==> [3/4] Building bflbwifictrl userspace app...${NC}"
    cd "${BFLBWIFICTRL_DIR}" || exit 1
    make ENABLE_NETIF_AUTO_CONFIG=1 2>/dev/null || echo -e "${YELLOW}    Warning: make failed or no Makefile${NC}"
    cd - || exit 1

    # Copy bflbwifictrl apps from app/ subdirectory to output
    copied_count=0
    if [ -f "${BFLBWIFICTRL_DIR}/app/bflbwifictrl" ]; then
        cp "${BFLBWIFICTRL_DIR}/app/bflbwifictrl" "${OUTPUT_DIR}/"
        echo -e "${GREEN}    Copied: ${OUTPUT_DIR}/bflbwifictrl${NC}"
        ls -lh "${OUTPUT_DIR}/bflbwifictrl" | awk '{print "    Size: " $5}'
        copied_count=$((copied_count + 1))
    else
        echo -e "${YELLOW}    Warning: ${BFLBWIFICTRL_DIR}/app/bflbwifictrl not found${NC}"
    fi

    if [ -f "${BFLBWIFICTRL_DIR}/app/bflbwifid" ]; then
        cp "${BFLBWIFICTRL_DIR}/app/bflbwifid" "${OUTPUT_DIR}/"
        echo -e "${GREEN}    Copied: ${OUTPUT_DIR}/bflbwifid${NC}"
        ls -lh "${OUTPUT_DIR}/bflbwifid" | awk '{print "    Size: " $5}'
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
    ls -lh "${OUTPUT_DIR}" | grep -E "${MODULE_NAME}.ko|nethub_vchan_app|bflbwifictrl|bflbwifid" || echo "    (no files found)"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo -e "  1. Load module: ${GREEN}sudo ./$0 load${NC}"
    echo -e "  2. Run virtualchan app: ${GREEN}sudo ${OUTPUT_DIR}/nethub_vchan_app${NC}"
    echo -e "  3. Run bflbwifid daemon: ${GREEN}sudo ${OUTPUT_DIR}/bflbwifid${NC}"
    echo -e "  4. Run bflbwifictrl cli: ${GREEN}sudo ${OUTPUT_DIR}/bflbwifictrl${NC}"
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
    echo -e "${YELLOW}==> [1/4] Cleaning kernel module build...${NC}"
    cd "${KERNEL_DIR}" || exit 1
    make clean 2>/dev/null || true
    cd - || exit 1
    echo -e "${GREEN}    Done${NC}"

    # Clean virtualchan build
    echo ""
    echo -e "${YELLOW}==> [2/4] Cleaning virtualchan build...${NC}"
    cd "${VCHAN_DIR}" || exit 1
    make clean 2>/dev/null || echo -e "${YELLOW}    (No Makefile, skipping)${NC}"
    cd - || exit 1
    echo -e "${GREEN}    Done${NC}"

    # Clean bflbwifictrl build
    echo ""
    echo -e "${YELLOW}==> [3/4] Cleaning bflbwifictrl build...${NC}"
    cd "${BFLBWIFICTRL_DIR}" || exit 1
    make clean 2>/dev/null || echo -e "${YELLOW}    (No Makefile, skipping)${NC}"
    cd - || exit 1
    echo -e "${GREEN}    Done${NC}"

    # Clean output directory
    echo ""
    echo -e "${YELLOW}==> [4/4] Cleaning output directory...${NC}"
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
        read -p "Reload module? [y/N] " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Cancelled"
            exit 0
        fi
        do_unload
    fi

    # Load module from output directory
    sudo insmod "${OUTPUT_DIR}/${MODULE_NAME}.ko"
    if [ $? -eq 0 ]; then
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
        exit 0
    fi

    # Unload module
    sudo rmmod "${MODULE_NAME}"
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}==> Module unloaded successfully${NC}"
    else
        echo -e "${RED}==> Failed to unload module${NC}"
        echo -e "${YELLOW}Hint: Check 'dmesg | tail' for errors${NC}"
        exit 1
    fi
}

# Main script logic
main() {
    # Show usage if no argument
    if [ $# -eq 0 ]; then
        usage
    fi

    # Parse command
    case "$1" in
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
        *)
            echo -e "${RED}Error: Unknown command '$1'${NC}"
            echo ""
            usage
            ;;
    esac
}

# Run main with all arguments
main "$@"
