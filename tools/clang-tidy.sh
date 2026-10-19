#!/bin/bash

# Set clang-tidy binary from environment variable or use default
CLANG_TIDY=${CLANG_TIDY_BIN:-clang-tidy-19}

# Get subdirectories from command line arguments (supports multiple subdirectories)
SUBDIRS=("$@")

# File to record passed clang-tidy checks
PASSED_FILES_RECORD=".clang_tidy_passed"

# Directories to exclude from clang-tidy processing
EXCLUDE_DIRS=(
    "components/wireless/wl80211/src/net80211"
    "components/crypto/mbedtls/mbedtls"
)

# pre-process compile_commands.json
sed -i 's/-mtune=e907/-mtune=generic-rv32/' compile_commands.json
sed -i 's/rv32imafcpzpsfoperand_xtheade/rv32imafc/' compile_commands.json
sed -i 's/-march=rv32imafc_xtheade/-march=rv32imafc/' compile_commands.json
sed -i 's/-march=rv32imafcpzpsfoperand_xtheade/-march=rv32imafc/' compile_commands.json
sed -i '/-fstrict-volatile-bitfields/d' compile_commands.json

while IFS= read -r file; do
    if [[ $(basename "$file") == CMake* ]]; then
        continue
    fi

    # Skip assembly files - they are not meant to be processed by clang-tidy
    if [[ "$file" == *.S ]]; then
        continue
    fi

    # Filter by subdirectories or files if specified
    if [[ ${#SUBDIRS[@]} -gt 0 ]]; then
        # Check if the file matches any of the specified subdirectories or files
        matched=false
        for item in "${SUBDIRS[@]}"; do
            # Check if it's a direct file path match
            if [[ "$file" == *"$item" ]]; then
                matched=true
                break
            fi
            # Check if the file matches any of the specified subdirectories
            if [[ "$file" == */"$item"/* ]]; then
                matched=true
                break
            fi
        done
        if [[ "$matched" == false ]]; then
            continue
        fi
    fi

    # Skip files in excluded directories
    for excluded_dir in "${EXCLUDE_DIRS[@]}"; do
        if [[ "$file" == *"$excluded_dir"* ]]; then
            continue 2
        fi
    done

    # Skip files that already passed (only when no subdirectories/files specified)
    if [[ ${#SUBDIRS[@]} -eq 0 && -f "$PASSED_FILES_RECORD" ]]; then
        if grep -Fxq "$file" "$PASSED_FILES_RECORD" 2>/dev/null; then
            echo "Skipping already passed: $file"
            continue
        fi
    fi

    echo "clang-tidy: $file"

    # Run clang-tidy with warnings-as-errors and capture exit code
    output=$("$CLANG_TIDY" -header-filter=".*" -system-headers=0 --warnings-as-errors='*' "$file" 2>&1)
    exit_code=$?

    echo "$output"

    # Check if there are any remaining errors (not the known false positive)
    if [[ $exit_code -ne 0 ]]; then
        echo "clang-tidy failed for $file with exit code $exit_code"
        exit $exit_code
    fi

    # Record file as passed
    echo "$file" >> "$PASSED_FILES_RECORD"
done < <(cat compile_commands.json | jq -r '.[]|.file')

echo "All done"
