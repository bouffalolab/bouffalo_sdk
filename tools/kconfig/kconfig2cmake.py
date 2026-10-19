#!/usr/bin/env python3
"""
Convert Kconfig .config file to CMake config.cmake format

This script reads a .config file in Kconfig format and generates
a config.cmake file with equivalent CMake set() commands.

Usage:
    python3 kconfig2cmake.py [--config .config] [--output config.cmake]

If not specified, defaults to .config and config.cmake in current directory.
"""

import argparse
import re
import sys
import os


def render_cmake(values, source):
    lines = [
        "# Auto-generated config file from {}".format(source),
        "# Do not edit manually",
        "",
    ]
    for name, value in values.items():
        lines.append("set({} {})".format(name, value))
    return "\n".join(lines) + "\n"


def parse_kconfig_value(value_str):
    """
    Parse Kconfig value and convert to appropriate CMake format.
    
    Args:
        value_str: Value string from .config (e.g., 'y', 'n', '123', '"text"')
        
    Returns:
        Tuple of (cmake_value, is_enabled)
    """
    value_str = value_str.strip()

    if value_str == 'y':
        return 'y', True
    elif value_str == 'n':
        return 'n', False
    elif value_str.startswith('"') and value_str.endswith('"'):
        # String value
        return value_str, True
    elif value_str.isdigit() or (value_str.startswith('-') and value_str[1:].isdigit()):
        # Numeric value
        return value_str, True
    else:
        # Other values (like hex, etc.)
        return value_str, True


def convert_kconfig_to_cmake(config_file, output_file):
    """
    Convert Kconfig .config file to CMake config.cmake format.
    
    Args:
        config_file: Path to input .config file
        output_file: Path to output config.cmake file
    """
    try:
        with open(config_file, 'r', encoding='utf-8') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: Input file '{config_file}' not found", file=sys.stderr)
        return False
    except Exception as e:
        print(f"Error reading input file: {e}", file=sys.stderr)
        return False

    values = {}

    for line_num, line in enumerate(lines, 1):
        line = line.strip()

        # "not set" lines must be handled before the generic comment skip,
        # otherwise disabling a symbol would leave its Make/CMake default in
        # place.
        match = re.match(r'# CONFIG_([A-Za-z0-9_]+) is not set$', line)
        if match:
            config_name = match.group(1)
            values[f"CONFIG_{config_name}"] = 'n'
            continue

        # Skip empty lines and comments
        if not line or line.startswith('#'):
            continue
            
        # Parse CONFIG_* entries
        match = re.match(r'CONFIG_([A-Za-z0-9_]+)=(.+)$', line)
        if match:
            config_name = match.group(1)
            config_value = match.group(2)
            
            # Parse the value
            cmake_value, is_enabled = parse_kconfig_value(config_value)
            
            # Generate CMake set command
            values[f"CONFIG_{config_name}"] = cmake_value
            continue

    try:
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(render_cmake(values, config_file))
    except Exception as e:
        print(f"Error writing output file: {e}", file=sys.stderr)
        return False
    
    print(f"Successfully converted {config_file} to {output_file}")
    return True


def main():
    parser = argparse.ArgumentParser(description='Convert Kconfig .config to CMake config.cmake')
    parser.add_argument('--config', '-c', 
                       default='.config',
                       help='Input .config file (default: .config)')
    parser.add_argument('--output', '-o',
                       default='config.cmake',
                       help='Output config.cmake file (default: config.cmake)')
    args = parser.parse_args()
    
    if not os.path.exists(args.config):
        print(f"Warning: {args.config} does not exist, creating empty {args.output}")
        # Create empty config.cmake
        with open(args.output, 'w') as f:
            f.write("# Auto-generated config file from .config\n")
            f.write("# No configurations found in .config\n\n")
        return
    
    success = convert_kconfig_to_cmake(args.config, args.output)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
