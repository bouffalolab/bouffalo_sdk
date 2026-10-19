#!/bin/bash
#
# GCOV 覆盖率报告生成脚本
#
# 使用方法：
#   1. 将 SD 卡中的 .gcda 文件复制到 gcov_data/ 目录
#   2. 运行此脚本：./gcov_generate_report.sh
#

set -e

# 配置
PROJECT_DIR=`realpath .`
BUILD_DIR="${PROJECT_DIR}/build/build_macsw"
GCOV_DATA_DIR="${PROJECT_DIR}/gcov_data"
OUTPUT_DIR="${PROJECT_DIR}/gcov_report"
GCOV_TOOLCHAIN="riscv64-zephyr-elf"

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}GCOV 覆盖率报告生成脚本${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""

# 检查目录
if [ ! -d "${BUILD_DIR}" ]; then
    echo -e "${RED}错误: build 目录不存在: ${BUILD_DIR}${NC}"
    echo "请先编译项目"
    exit 1
fi

if [ ! -d "${GCOV_DATA_DIR}" ]; then
    echo -e "${YELLOW}警告: gcov_data 目录不存在，正在创建...${NC}"
    mkdir -p "${GCOV_DATA_DIR}"
    echo ""
    echo -e "${YELLOW}请将 SD 卡中的 .gcda 文件复制到: ${GCOV_DATA_DIR}${NC}"
    echo -e "${YELLOW}然后重新运行此脚本${NC}"
    echo ""
    echo "示例命令（SD 卡挂载在 /mnt/sd）:"
    echo "  mkdir -p ${GCOV_DATA_DIR}"
    echo "  cp /mnt/sd/*.gcda ${GCOV_DATA_DIR}/"
    echo ""
    exit 0
fi

# 检查 .gcda 文件
GCDA_COUNT=$(find "${GCOV_DATA_DIR}" -name "*.gcda" -type f 2>/dev/null | wc -l)
if [ "${GCDA_COUNT}" -eq 0 ]; then
    echo -e "${RED}错误: gcov_data 目录中没有 .gcda 文件${NC}"
    echo "请先将 SD 卡中的 .gcda 文件复制到: ${GCOV_DATA_DIR}"
    exit 1
fi

echo -e "${GREEN}找到 ${GCDA_COUNT} 个 .gcda 文件${NC}"
echo ""

# 创建输出目录
mkdir -p "${OUTPUT_DIR}"

# 复制 .gcno 文件到输出目录
echo -e "${GREEN}步骤 1: 复制 .gcno 文件...${NC}"
GCNO_OUTPUT_DIR="${OUTPUT_DIR}/gcno_files"
mkdir -p "${GCNO_OUTPUT_DIR}"

# 查找所有 .gcno 文件
find "${BUILD_DIR}" -name "*.gcno" -type f -exec cp {} "${GCNO_OUTPUT_DIR}/" \;
GCNO_COUNT=$(find "${GCNO_OUTPUT_DIR}" -name "*.gcno" -type f | wc -l)
echo -e "  复制了 ${GCNO_COUNT} 个 .gcno 文件到 ${GCNO_OUTPUT_DIR}"
echo ""

# 复制 .gcda 文件到输出目录
echo -e "${GREEN}步骤 2: 复制 .gcda 文件...${NC}"
GCDA_OUTPUT_DIR="${OUTPUT_DIR}/gcda_files"
mkdir -p "${GCDA_OUTPUT_DIR}"

# 直接复制 .gcda 文件并重命名（去掉路径前缀，只保留基本文件名）
for gcda_file in "${GCOV_DATA_DIR}"/*.gcda; do
    if [ -f "$gcda_file" ]; then
        full_filename=$(basename "$gcda_file")
        # 提取基本文件名（最后一个 # 之后的部分）
        base_filename=$(echo "$full_filename" | sed 's/.*#//')
        # 复制并重命名
        cp "$gcda_file" "${GCDA_OUTPUT_DIR}/${base_filename}"
    fi
done
GCDA_COUNT=$(find "${GCDA_OUTPUT_DIR}" -name "*.gcda" -type f | wc -l)
echo "  复制并重命名了 ${GCDA_COUNT} 个 .gcda 文件到 ${GCDA_OUTPUT_DIR}"
echo ""

# 生成覆盖率数据
echo -e "${GREEN}步骤 3: 生成覆盖率数据 (.gcov)...${NC}"
cd "${OUTPUT_DIR}"

# 使用交叉编译的 gcov 工具
GCOV_CMD="${GCOV_TOOLCHAIN}-gcov"

if ! command -v "${GCOV_CMD}" &> /dev/null; then
    echo -e "${RED}错误: 找不到 gcov 工具: ${GCOV_CMD}${NC}"
    echo "请确保工具链在 PATH 中"
    exit 1
fi

# 为每个 .gcda 文件生成对应的 .gcov 文件
for gcda_file in "${GCDA_OUTPUT_DIR}"/*.gcda; do
    if [ -f "$gcda_file" ]; then
        base_filename=$(basename "$gcda_file" .gcda)

        # 查找对应的 .gcno 文件
        gcno_file="${GCNO_OUTPUT_DIR}/${base_filename}.gcno"

        if [ -f "$gcno_file" ]; then
            echo "  Processing ${base_filename}..."
            ${GCOV_CMD} --object-directory "${GCNO_OUTPUT_DIR}" "${gcda_file}" 2>/dev/null || true
        else
            echo "  Warning: ${base_filename}.gcda 未找到对应的 .gcno 文件，跳过"
        fi
    fi
done
echo ""

# 生成 lcov 数据文件
echo -e "${GREEN}步骤 4: 生成 lcov 数据文件 (.info)...${NC}"
INFO_FILE="${OUTPUT_DIR}/coverage.info"

# 使用 geninfo 从 .gcda 文件生成 .info 文件
if command -v geninfo &> /dev/null; then
    geninfo "${GCDA_OUTPUT_DIR}" --output-filename "${INFO_FILE}" \
        --gcov-tool "${GCOV_CMD}" \
        --build-directory "${GCNO_OUTPUT_DIR}" 2>&1 || true

    if [ -s "${INFO_FILE}" ]; then
        echo -e "  生成 ${INFO_FILE}"
    else
        echo -e "${YELLOW}  警告: lcov 数据文件生成失败${NC}"
        INFO_FILE=""
    fi
else
    echo -e "${YELLOW}警告: geninfo 未安装，跳过 .info 文件生成${NC}"
    echo "  安装命令: sudo apt-get install lcov"
    INFO_FILE=""
fi
echo ""

# 生成 HTML 报告
if command -v genhtml &> /dev/null && [ -n "${INFO_FILE}" ] && [ -s "${INFO_FILE}" ]; then
    echo -e "${GREEN}步骤 5: 生成 HTML 报告...${NC}"
    HTML_DIR="${OUTPUT_DIR}/html"
    mkdir -p "${HTML_DIR}"

    genhtml "${INFO_FILE}" --output-directory "${HTML_DIR}" \
        --title "GCOV Coverage Report"

    echo -e "  ${GREEN}HTML 报告已生成: ${HTML_DIR}/index.html${NC}"
    echo ""
else
    echo -e "${YELLOW}步骤 5: genhtml 未安装或 .info 文件不存在，跳过 HTML 报告生成${NC}"
    echo "  安装命令: sudo apt-get install lcov"
    echo ""
fi

# 显示摘要
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}生成完成！${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "输出目录: ${OUTPUT_DIR}"
echo "  - .gcov 文件: ${OUTPUT_DIR}/*.gcov"
echo "  - .info 文件: ${INFO_FILE}"
echo "  - HTML 报告: ${HTML_DIR}/index.html"
echo ""
echo "查看 HTML 报告:"
echo "  firefox ${HTML_DIR}/index.html"
echo "  或"
echo "  xdg-open ${HTML_DIR}/index.html"
echo ""

# 使用 gcov 直接显示覆盖率统计
if [ -n "$(ls ${OUTPUT_DIR}/*.gcov 2>/dev/null)" ]; then
    echo -e "${GREEN}覆盖率统计摘要:${NC}"
    ${GCOV_CMD} --version | head -1
    echo ""

    for gcov_file in "${OUTPUT_DIR}"/*.gcov; do
        if [ -f "$gcov_file" ]; then
            filename=$(basename "$gcov_file" .gcov)
            echo "  - ${filename}:"
            grep -A 5 "File.*" "$gcov_file" | head -6 || true
        fi
    done | head -50
    echo ""
fi

echo -e "${GREEN}完成！${NC}"
