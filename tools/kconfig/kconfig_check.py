#!/usr/bin/env python3
"""Validate the Kconfig tree used by an application.

The check is intentionally fast and structural:

* the Kconfig entry file parses without warnings (undefined dependencies,
  duplicate symbols, syntax errors);
* every defined symbol has a prompt and help text;
* no symbol name carries the ``CONFIG_`` prefix (which would produce
  double-prefixed .config entries when Kconfiglib writes the file).

Usage from an example directory:

    make kconfig-check CHIP=<chip> BOARD=<board>
"""

import argparse
import os
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--kconfig", default="Kconfig",
                        help="Kconfig entry file (default: Kconfig)")
    args = parser.parse_args()

    kconfig_dir = os.path.dirname(os.path.abspath(__file__))
    if kconfig_dir not in sys.path:
        sys.path.insert(0, kconfig_dir)

    try:
        from kconfiglib import Kconfig, KconfigError
    except ImportError as exc:
        print("kconfiglib import failed: {0}".format(exc), file=sys.stderr)
        return 1

    if not os.path.exists(args.kconfig):
        print("Error: Kconfig entry file not found: {0}".format(args.kconfig),
              file=sys.stderr)
        return 1

    try:
        kconf = Kconfig(args.kconfig, warn_to_stderr=False,
                        suppress_traceback=True)
    except KconfigError as exc:
        print("Kconfig parse error in {0}:\n{1}".format(args.kconfig, exc),
              file=sys.stderr)
        return 1

    problems = []
    for warning in kconf.warnings:
        problems.append("Kconfig warning: {0}".format(warning))

    for sym in kconf.unique_defined_syms:
        if sym.name.startswith("CONFIG_"):
            problems.append(
                "{0}: symbol names must not contain the CONFIG_ prefix"
                .format(sym.name))
        if not any(node.prompt for node in sym.nodes):
            problems.append("{0}: missing prompt".format(sym.name))
        if not any(getattr(node, "help", None) for node in sym.nodes):
            problems.append("{0}: missing help text".format(sym.name))

    if problems:
        print("Kconfig check FAILED ({0} problem(s)):".format(len(problems)),
              file=sys.stderr)
        for problem in problems:
            print("  " + problem, file=sys.stderr)
        return 1

    print("Kconfig check passed: {0} symbols, 0 warnings, all documented"
          .format(len(kconf.unique_defined_syms)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
