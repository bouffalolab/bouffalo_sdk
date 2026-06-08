#!/bin/bash
#
# GCOV 覆盖率报告生成脚本
#
# 使用方法：
#   1. 将板端导出的 .gcda 文件复制到 gcov_data/ 目录
#   2. 运行此脚本：./gcov_generate_report.sh
#   3. 报告默认生成到 gcov_runs/full
#

set -e

# 配置
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"
ORIGINAL_PWD="$(pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
GCOV_DATA_DIR="${PROJECT_DIR}/gcov_data"
RUNS_DIR="${GCOV_DATA_DIR}/gcov_runs"
OUTPUT_DIR="${RUNS_DIR}/full"
GCOV_TOOLCHAIN="riscv64-unknown-elf"
CASE_NAME="full"
REPORT_TITLE="GCOV Coverage Report"
CLEAN_OUTPUT=1
DRY_RUN=0
INCLUDE_PATTERNS=(
    "*/components/wireless/wl80211/*"
    "*/components/wireless/macsw/*"
)
EXCLUDE_PATTERNS=(
    "*/components/wireless/wl80211/src/net80211/*"
)

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

usage()
{
    cat <<EOF
用法:
  $0 [选项]

常用用法:
  $0
  $0 --case-name sta_wpa2_connect

选项:
  --build-dir DIR      build 目录，默认: ${BUILD_DIR}
  --toolchain PREFIX   gcov 工具前缀，默认: ${GCOV_TOOLCHAIN}
  --case-name NAME     case 名称，用于报告标题，默认: ${CASE_NAME}
  --title TITLE        HTML 报告标题，默认: ${REPORT_TITLE}
  --keep-output        不清理输出目录中 gcno_files/gcda_files/html 等生成物
  --dry-run            只检查目录和 .gcda 输入数量，不生成报告
  --no-default-filter  不启用默认过滤，默认只保留 wl80211/macsw 并排除 wl80211/src/net80211
  -h, --help           显示帮助

输出:
  gcov_runs/full/coverage.info        完整覆盖率数据
  gcov_runs/full/html/                完整 HTML 报告
EOF
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --build-dir)
            [ "$#" -ge 2 ] || { echo -e "${RED}错误: --build-dir 需要参数${NC}"; exit 1; }
            BUILD_DIR="$2"
            shift 2
            ;;
        --toolchain)
            [ "$#" -ge 2 ] || { echo -e "${RED}错误: --toolchain 需要参数${NC}"; exit 1; }
            GCOV_TOOLCHAIN="$2"
            shift 2
            ;;
        --case-name)
            [ "$#" -ge 2 ] || { echo -e "${RED}错误: --case-name 需要参数${NC}"; exit 1; }
            CASE_NAME="$2"
            shift 2
            ;;
        --title)
            [ "$#" -ge 2 ] || { echo -e "${RED}错误: --title 需要参数${NC}"; exit 1; }
            REPORT_TITLE="$2"
            shift 2
            ;;
        --keep-output)
            CLEAN_OUTPUT=0
            shift
            ;;
        --dry-run)
            DRY_RUN=1
            shift
            ;;
        --no-default-filter|--no-default-exclude)
            INCLUDE_PATTERNS=()
            EXCLUDE_PATTERNS=()
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}错误: 未知参数 $1${NC}"
            usage
            exit 1
            ;;
    esac
done

find_gcda_files()
{
    local data_dir="$1"
    local report_dir="${data_dir%/}/gcov_report"

    # gcov_data/gcov_report 是本脚本的生成目录，里面的 gcda_files 是副本。
    # 这里必须排除它，否则重复运行脚本时会把旧报告里的 .gcda 也当作新输入。
    find "${data_dir}" \
        \( -path "${report_dir}" -o -path "${report_dir}/*" \
           -o -path "${OUTPUT_DIR}" -o -path "${OUTPUT_DIR}/*" \) -prune \
        -o -name "*.gcda" -type f -print0
}

count_gcda_files()
{
    local count=0

    while IFS= read -r -d '' _gcda_file; do
        count=$((count + 1))
    done < <(find_gcda_files "$1")

    echo "${count}"
}

validate_gcda_files()
{
    local invalid_count=0
    local max_errors=10
    local gcda_file
    local full_filename
    local decoded_path

    while IFS= read -r -d '' gcda_file; do
        full_filename=$(basename "${gcda_file}")
        decoded_path="$(gcov_encoded_to_path "${full_filename}")"
        if [[ "${full_filename}" != build#* || "${decoded_path}" != build/* ]]; then
            if [ "${invalid_count}" -lt "${max_errors}" ]; then
                echo -e "${RED}错误: ${full_filename} 不是 build#... 路径编码格式 .gcda${NC}"
            fi
            invalid_count=$((invalid_count + 1))
        fi
    done < <(find_gcda_files "$1")

    if [ "${invalid_count}" -gt 0 ]; then
        if [ "${invalid_count}" -gt "${max_errors}" ]; then
            echo -e "${RED}错误: 还有 $((invalid_count - max_errors)) 个 .gcda 文件不是 build#... 路径编码格式${NC}"
        fi
        echo "请重新从板端导出以 build# 开头的路径编码 .gcda 文件"
        return 1
    fi

    return 0
}

gcov_encoded_to_path()
{
    local name="$1"

    name="${name%.gcda}"
    name="${name%.gcno}"
    printf '%s' "${name//\#//}"
}

copy_to_unique_path()
{
    local src="$1"
    local root="$2"
    local rel="$3"
    local dst="${root}/${rel}"

    mkdir -p "$(dirname "${dst}")"
    if [ -f "${dst}" ]; then
        echo -e "  ${YELLOW}警告: 发现重复目标，覆盖: ${rel}${NC}"
    fi
    cp "${src}" "${dst}"
}

abs_path()
{
    case "$1" in
        /*)
            printf "%s\n" "$1"
            ;;
        *)
            printf "%s/%s\n" "${ORIGINAL_PWD}" "$1"
            ;;
    esac
}

BUILD_DIR="$(abs_path "${BUILD_DIR}")"
GCOV_DATA_DIR="$(abs_path "${GCOV_DATA_DIR}")"
RUNS_DIR="$(abs_path "${RUNS_DIR}")"
OUTPUT_DIR="${RUNS_DIR}/full"
OUTPUT_DIR="$(abs_path "${OUTPUT_DIR}")"
HTML_DIR="${OUTPUT_DIR}/html"

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}GCOV 覆盖率报告生成脚本${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Case: ${CASE_NAME}"
echo "Build dir: ${BUILD_DIR}"
echo "GCDA dir: ${GCOV_DATA_DIR}"
echo "Output dir: ${OUTPUT_DIR}"
[ "${DRY_RUN}" -eq 1 ] && echo "Mode: dry-run"
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
GCDA_COUNT=$(count_gcda_files "${GCOV_DATA_DIR}")
if [ "${GCDA_COUNT}" -eq 0 ]; then
    echo -e "${RED}错误: gcov_data 目录中没有 .gcda 文件${NC}"
    echo "请先将 SD 卡中的 .gcda 文件复制到: ${GCOV_DATA_DIR}"
    exit 1
fi

echo -e "${GREEN}找到 ${GCDA_COUNT} 个 .gcda 文件${NC}"
validate_gcda_files "${GCOV_DATA_DIR}"
echo ""

if [ "${DRY_RUN}" -eq 1 ]; then
    echo -e "${GREEN}Dry-run 完成，没有生成报告。${NC}"
    exit 0
fi

echo -e "${GREEN}清理 full 输出目录...${NC}"
[ "${CLEAN_OUTPUT}" -eq 1 ] && rm -rf "${OUTPUT_DIR}"
echo ""

# 创建输出目录
mkdir -p "${OUTPUT_DIR}"

# 复制 .gcno 文件到输出目录
echo -e "${GREEN}步骤 1: 复制 .gcno 文件...${NC}"
GCNO_OUTPUT_DIR="${OUTPUT_DIR}/gcno_files"
[ "${CLEAN_OUTPUT}" -eq 1 ] && rm -rf "${GCNO_OUTPUT_DIR}"
mkdir -p "${GCNO_OUTPUT_DIR}"

# 查找所有 .gcno 文件。保留 build 内相对路径，避免同名源文件互相覆盖。
while IFS= read -r -d '' gcno_file; do
    gcno_rel="build/${gcno_file#${BUILD_DIR}/}"
    copy_to_unique_path "${gcno_file}" "${GCNO_OUTPUT_DIR}" "${gcno_rel}"
done < <(find "${BUILD_DIR}" -name "*.gcno" -type f -print0)

GCNO_COUNT=$(find "${GCNO_OUTPUT_DIR}" -name "*.gcno" -type f | wc -l)
echo -e "  复制了 ${GCNO_COUNT} 个 .gcno 文件到 ${GCNO_OUTPUT_DIR}"
echo ""

# 复制 .gcda 文件到输出目录
echo -e "${GREEN}步骤 2: 复制 .gcda 文件...${NC}"
GCDA_OUTPUT_DIR="${OUTPUT_DIR}/gcda_files"
[ "${CLEAN_OUTPUT}" -eq 1 ] && rm -rf "${GCDA_OUTPUT_DIR}"
mkdir -p "${GCDA_OUTPUT_DIR}"

# 复制 .gcda 文件。文件名必须使用 build#... 路径编码。
# 注意：输入目录可能正是 gcov_data，输出目录在 gcov_data/gcov_report 下；
# find_gcda_files 会排除 gcov_report，避免重复使用上次报告的中间文件。
while IFS= read -r -d '' gcda_file; do
    if [ -f "${gcda_file}" ]; then
        full_filename=$(basename "$gcda_file")
        gcda_rel="$(gcov_encoded_to_path "${full_filename}").gcda"
        copy_to_unique_path "${gcda_file}" "${GCDA_OUTPUT_DIR}" "${gcda_rel}"
    fi
done < <(find_gcda_files "${GCOV_DATA_DIR}")
GCDA_COUNT=$(find "${GCDA_OUTPUT_DIR}" -name "*.gcda" -type f | wc -l)
echo "  复制了 ${GCDA_COUNT} 个 .gcda 文件到 ${GCDA_OUTPUT_DIR}"
echo ""

# 输出缺失 .gcda 的 .gcno 清单，便于排查哪些编译单元没有运行时覆盖数据。
MISSING_GCDA_LIST="${OUTPUT_DIR}/missing_gcda_from_gcno.txt"
comm -23 \
    <(find "${GCNO_OUTPUT_DIR}" -name "*.gcno" -type f | sed "s#^${GCNO_OUTPUT_DIR}/##; s#\\.gcno\$##" | sort) \
    <(find "${GCDA_OUTPUT_DIR}" -name "*.gcda" -type f | sed "s#^${GCDA_OUTPUT_DIR}/##; s#\\.gcda\$##" | sort) \
    > "${MISSING_GCDA_LIST}"
MISSING_GCDA_COUNT=$(wc -l < "${MISSING_GCDA_LIST}" | tr -d ' ')
if [ "${MISSING_GCDA_COUNT}" -gt 0 ]; then
    echo -e "${YELLOW}  有 ${MISSING_GCDA_COUNT} 个 .gcno 没有对应 .gcda，清单: ${MISSING_GCDA_LIST}${NC}"
    echo ""
fi

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
while IFS= read -r -d '' gcda_file; do
    gcda_rel="${gcda_file#${GCDA_OUTPUT_DIR}/}"
    gcno_file="${GCNO_OUTPUT_DIR}/${gcda_rel%.gcda}.gcno"

    if [ -f "${gcno_file}" ]; then
        echo "  Processing ${gcda_rel%.gcda}..."
        ${GCOV_CMD} --object-directory "$(dirname "${gcno_file}")" "${gcda_file}" 2>/dev/null || true
    else
        echo "  Warning: ${gcda_rel} 未找到对应的 .gcno 文件，跳过"
    fi
done < <(find "${GCDA_OUTPUT_DIR}" -name "*.gcda" -type f -print0)
echo ""

# 生成 lcov 数据文件
echo -e "${GREEN}步骤 4: 生成 lcov 数据文件 (.info)...${NC}"
INFO_FILE="${OUTPUT_DIR}/coverage.info"
INITIAL_INFO_FILE="${OUTPUT_DIR}/coverage.initial.info"
RUN_INFO_FILE="${OUTPUT_DIR}/coverage.run.info"
MERGED_INFO_FILE="${OUTPUT_DIR}/coverage.merged.info"
FILTERED_INCLUDE_INFO_FILE="${OUTPUT_DIR}/coverage.include.info"

# 先从所有 .gcno 生成 0% 初始覆盖，再叠加 .gcda 的实际运行覆盖。
# 这样即使某个目标文件没有生成 .gcda，也会在 HTML 报告中显示为 0%。
if command -v geninfo &> /dev/null; then
    geninfo --initial "${GCNO_OUTPUT_DIR}" \
        --output-filename "${INITIAL_INFO_FILE}" \
        --gcov-tool "${GCOV_CMD}" \
        --ignore-errors source,gcov,graph,path,inconsistent 2>&1 || true

    if [ -s "${INITIAL_INFO_FILE}" ]; then
        echo -e "  生成初始覆盖 ${INITIAL_INFO_FILE}"
    else
        echo -e "${YELLOW}  警告: 初始覆盖数据生成失败${NC}"
    fi

    geninfo "${GCDA_OUTPUT_DIR}" \
        --output-filename "${RUN_INFO_FILE}" \
        --gcov-tool "${GCOV_CMD}" \
        --build-directory "${GCNO_OUTPUT_DIR}" \
        --ignore-errors source,gcov,graph,path,inconsistent 2>&1 || true

    if [ -s "${RUN_INFO_FILE}" ]; then
        echo -e "  生成运行覆盖 ${RUN_INFO_FILE}"
    else
        echo -e "${YELLOW}  警告: 运行覆盖数据生成失败${NC}"
    fi

    if [ -s "${INITIAL_INFO_FILE}" ] && [ -s "${RUN_INFO_FILE}" ]; then
        if command -v lcov &> /dev/null; then
            lcov --add-tracefile "${INITIAL_INFO_FILE}" \
                --add-tracefile "${RUN_INFO_FILE}" \
                --output-file "${MERGED_INFO_FILE}" \
                --ignore-errors unused 2>&1 || true
        else
            echo -e "${YELLOW}  警告: lcov 未安装，无法合并初始覆盖和运行覆盖${NC}"
        fi
    elif [ -s "${INITIAL_INFO_FILE}" ]; then
        cp "${INITIAL_INFO_FILE}" "${MERGED_INFO_FILE}"
    elif [ -s "${RUN_INFO_FILE}" ]; then
        cp "${RUN_INFO_FILE}" "${MERGED_INFO_FILE}"
    fi

    if [ -s "${MERGED_INFO_FILE}" ]; then
        FILTER_SOURCE_INFO_FILE="${MERGED_INFO_FILE}"

        if [ "${#INCLUDE_PATTERNS[@]}" -gt 0 ]; then
            if command -v lcov &> /dev/null; then
                echo -e "  保留覆盖率路径:"
                printf '    - %s\n' "${INCLUDE_PATTERNS[@]}"
                lcov --extract "${FILTER_SOURCE_INFO_FILE}" "${INCLUDE_PATTERNS[@]}" \
                    --output-file "${FILTERED_INCLUDE_INFO_FILE}" \
                    --ignore-errors unused 2>&1 || true
                FILTER_SOURCE_INFO_FILE="${FILTERED_INCLUDE_INFO_FILE}"
            else
                echo -e "${YELLOW}  警告: lcov 未安装，无法按白名单过滤覆盖率路径${NC}"
            fi
        fi

        if [ ! -s "${FILTER_SOURCE_INFO_FILE}" ]; then
            echo -e "${YELLOW}  警告: 白名单过滤后没有覆盖率数据${NC}"
            FILTER_SOURCE_INFO_FILE="${MERGED_INFO_FILE}"
        fi

        if [ "${#EXCLUDE_PATTERNS[@]}" -gt 0 ]; then
            if command -v lcov &> /dev/null; then
                echo -e "  过滤覆盖率路径:"
                printf '    - %s\n' "${EXCLUDE_PATTERNS[@]}"
                lcov --remove "${FILTER_SOURCE_INFO_FILE}" "${EXCLUDE_PATTERNS[@]}" \
                    --output-file "${INFO_FILE}" \
                    --ignore-errors unused 2>&1 || true
            else
                echo -e "${YELLOW}  警告: lcov 未安装，无法过滤覆盖率路径${NC}"
                cp "${FILTER_SOURCE_INFO_FILE}" "${INFO_FILE}"
            fi
        else
            cp "${FILTER_SOURCE_INFO_FILE}" "${INFO_FILE}"
        fi
    fi

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
    [ "${CLEAN_OUTPUT}" -eq 1 ] && rm -rf "${HTML_DIR}"
    mkdir -p "${HTML_DIR}"

    genhtml "${INFO_FILE}" --output-directory "${HTML_DIR}" \
        --title "${REPORT_TITLE} - ${CASE_NAME}"

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
if [ -n "${INITIAL_INFO_FILE}" ] && [ -s "${INITIAL_INFO_FILE}" ]; then
    echo "  - 初始 .info 文件: ${INITIAL_INFO_FILE}"
fi
if [ -n "${RUN_INFO_FILE}" ] && [ -s "${RUN_INFO_FILE}" ]; then
    echo "  - 运行 .info 文件: ${RUN_INFO_FILE}"
fi
echo "  - 合并 .info 文件: ${INFO_FILE}"
if [ -d "${HTML_DIR}" ]; then
    echo "  - HTML 报告: ${HTML_DIR}/index.html"
fi
echo ""
if [ -d "${HTML_DIR}" ]; then
    echo "查看 HTML 报告:"
    echo "  firefox ${HTML_DIR}/index.html"
    echo "  或"
    echo "  xdg-open ${HTML_DIR}/index.html"
    echo ""
fi

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
