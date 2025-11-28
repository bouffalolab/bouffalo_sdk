#!/bin/bash

# NetHub Host Linux 构建和管理脚本
# 支持 build、clean、load、unload 功能

# 脚本配置
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
KERNEL_DIR="$SCRIPT_DIR/kernel"
USERSPACE_DIR="$SCRIPT_DIR/userspace"
MODULE_NAME="hd_sdio"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查目录是否存在
check_directories() {
    if [ ! -d "$KERNEL_DIR" ]; then
        log_error "kernel 目录不存在: $KERNEL_DIR"
        exit 1
    fi

    if [ ! -d "$USERSPACE_DIR" ]; then
        log_error "userspace 目录不存在: $USERSPACE_DIR"
        exit 1
    fi

    if [ ! -f "$KERNEL_DIR/Makefile" ]; then
        log_error "kernel Makefile 不存在: $KERNEL_DIR/Makefile"
        exit 1
    fi

    if [ ! -f "$USERSPACE_DIR/Makefile" ]; then
        log_error "userspace Makefile 不存在: $USERSPACE_DIR/Makefile"
        exit 1
    fi
}

# 检查root权限
check_root() {
    if [ "$EUID" -ne 0 ]; then
        log_error "此操作需要 root 权限"
        echo "请使用: sudo $0 $1"
        exit 1
    fi
}

# 检查内核开发环境
check_kernel_dev() {
    KVERSION=$(uname -r)
    KDIR="/lib/modules/$KVERSION/build"

    if [ ! -d "$KDIR" ]; then
        log_error "内核开发头文件不存在: $KDIR"
        log_error "请安装内核开发包: apt-get install linux-headers-$(uname -r)"
        exit 1
    fi
}

# build 功能
build_project() {
    log_info "开始构建 NetHub Host Linux 项目..."

    # 检查环境
    check_directories
    check_kernel_dev

    # 构建内核模块
    log_info "构建内核模块 $MODULE_NAME..."
    cd "$KERNEL_DIR"
    if make all; then
        log_success "内核模块构建成功"
    else
        log_error "内核模块构建失败"
        exit 1
    fi

    # 检查模块文件
    if [ -f "$KERNEL_DIR/$MODULE_NAME.ko" ]; then
        log_success "内核模块文件生成: $KERNEL_DIR/$MODULE_NAME.ko"
    else
        log_error "内核模块文件未找到: $KERNEL_DIR/$MODULE_NAME.ko"
        exit 1
    fi

    # 构建用户空间应用
    log_info "构建用户空间应用..."
    cd "$USERSPACE_DIR"
    if make all; then
        log_success "用户空间应用构建成功"
    else
        log_error "用户空间应用构建失败"
        exit 1
    fi

    log_success "NetHub Host Linux 项目构建完成！"
}

# clean 功能
clean_project() {
    log_info "清理构建文件..."

    check_directories

    # 清理内核模块
    log_info "清理内核模块..."
    cd "$KERNEL_DIR"
    if make clean; then
        log_success "内核模块清理完成"
    else
        log_warning "内核模块清理时出现警告"
    fi

    # 清理用户空间应用
    log_info "清理用户空间应用..."
    cd "$USERSPACE_DIR"
    if make clean; then
        log_success "用户空间应用清理完成"
    else
        log_warning "用户空间应用清理时出现警告"
    fi

    log_success "所有构建文件清理完成！"
}

# load 功能
load_module() {
    log_info "加载内核模块 $MODULE_NAME..."

    check_root "load"
    check_directories

    # 检查模块文件是否存在
    MODULE_FILE="$KERNEL_DIR/$MODULE_NAME.ko"
    if [ ! -f "$MODULE_FILE" ]; then
        log_error "内核模块文件不存在: $MODULE_FILE"
        log_error "请先运行: $0 build"
        exit 1
    fi

    # 检查模块是否已经加载
    if lsmod | grep -q "^$MODULE_NAME "; then
        log_warning "模块 $MODULE_NAME 已经加载"
        return 0
    fi

    # 加载模块
    log_info "正在加载模块: $MODULE_FILE"
    if insmod "$MODULE_FILE"; then
        log_success "模块 $MODULE_NAME 加载成功"

        # 显示模块信息
        log_info "模块信息:"
        lsmod | grep "^$MODULE_NAME "

        # 检查设备文件
        if [ -e "/dev/ttyHD0" ]; then
            log_success "设备文件已创建: /dev/ttyHD0"
        fi
    else
        log_error "模块 $MODULE_NAME 加载失败"
        exit 1
    fi
}

# unload 功能
unload_module() {
    log_info "卸载内核模块 $MODULE_NAME..."

    check_root "unload"

    # 检查模块是否已加载
    if ! lsmod | grep -q "^$MODULE_NAME "; then
        log_warning "模块 $MODULE_NAME 未加载"
        return 0
    fi

    # 卸载模块
    log_info "正在卸载模块: $MODULE_NAME"
    if rmmod "$MODULE_NAME"; then
        log_success "模块 $MODULE_NAME 卸载成功"
    else
        log_error "模块 $MODULE_NAME 卸载失败"
        exit 1
    fi
}

# 显示帮助信息
show_help() {
    echo "NetHub Host Linux 构建和管理脚本"
    echo ""
    echo "用法: $0 {build|clean|load|unload|help|status}"
    echo ""
    echo "功能:"
    echo "  build   - 编译内核模块和用户空间应用"
    echo "  clean   - 清理所有构建文件"
    echo "  load    - 加载 hd_sdio 内核模块 (需要 root 权限)"
    echo "  unload  - 卸载 hd_sdio 内核模块 (需要 root 权限)"
    echo "  help    - 显示此帮助信息"
    echo "  status  - 显示模块状态"
    echo ""
    echo "使用示例:"
    echo "  $0 build                    # 构建项目"
    echo "  $0 load                     # 加载模块"
    echo "  sudo $0 load                # 使用sudo加载模块"
    echo "  sudo $0 unload              # 使用sudo卸载模块"
    echo ""
    echo "注意事项:"
    echo "  - load/unload 功能需要 root 权限"
    echo "  - 首次使用前请确保已安装内核开发包"
    echo "  - 建议在执行前检查系统依赖"
}

# 显示状态
show_status() {
    log_info "检查项目状态..."

    check_directories

    # 检查内核模块文件
    MODULE_FILE="$KERNEL_DIR/$MODULE_NAME.ko"
    if [ -f "$MODULE_FILE" ]; then
        log_success "内核模块文件存在: $MODULE_FILE"
        ls -lh "$MODULE_FILE"
    else
        log_warning "内核模块文件不存在，请先运行 build"
    fi

    # 检查模块加载状态
    if lsmod | grep -q "^$MODULE_NAME "; then
        log_success "模块 $MODULE_NAME 已加载"
        lsmod | grep "^$MODULE_NAME "
    else
        log_warning "模块 $MODULE_NAME 未加载"
    fi

    # 检查设备文件
    if [ -e "/dev/ttyHD0" ]; then
        log_success "设备文件存在: /dev/ttyHD0"
        ls -lh /dev/ttyHD0 2>/dev/null || log_warning "无法获取设备文件权限信息"
    else
        log_warning "设备文件不存在: /dev/ttyHD0"
    fi
}

# 主逻辑
case "$1" in
    "build")
        build_project
        ;;
    "clean")
        clean_project
        ;;
    "load")
        load_module
        ;;
    "unload")
        unload_module
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    "status"|"-s"|"--status")
        show_status
        ;;
    "")
        log_error "请指定操作命令"
        show_help
        exit 1
        ;;
    *)
        log_error "未知命令: $1"
        echo ""
        show_help
        exit 1
        ;;
esac