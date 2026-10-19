#!/usr/bin/env python3

from __future__ import annotations

import argparse
import re
import sys
import time
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path

try:
    import serial
    from serial import SerialException
except ImportError as exc:
    raise SystemExit(
        "pyserial is required. Install it with: python3 -m pip install pyserial"
    ) from exc


ROOT_DIR = Path(__file__).resolve().parent
LFS_DIR = ROOT_DIR / "lfs"
DEFAULT_BAUDRATE = 2_000_000
DEFAULT_CASE_TIMEOUT = 30.0
DEFAULT_BOOT_TIMEOUT = 8.0
PROMPT = "bouffalolab />"
ANSI_RE = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")
ERROR_MARKERS = (
    "Exception:",
    "assertion failed",
    "JS execution failed",
    "Error:",
)


@dataclass
class TestCase:
    label: str
    command: str
    success_marker: str
    timeout: float


@dataclass
class TestResult:
    case: TestCase
    passed: bool
    reason: str
    output: str


class SerialRunner:
    def __init__(self, port: str, baudrate: int, boot_timeout: float) -> None:
        self.port = port
        self.baudrate = baudrate
        self.boot_timeout = boot_timeout
        self.ser = serial.Serial(port, baudrate, timeout=0.1, exclusive=True)

    def close(self) -> None:
        if self.ser.is_open:
            self.ser.close()

    def _clean(self, text: str) -> str:
        text = ANSI_RE.sub("", text)
        text = text.replace("\r", "")
        return text

    def _read_available(self) -> str:
        try:
            chunk = self.ser.read(4096)
        except SerialException as exc:
            raise RuntimeError(f"serial read failed: {exc}") from exc
        if not chunk:
            return ""
        return self._clean(chunk.decode("utf-8", errors="ignore"))

    def _drain_until_idle(self, quiet_s: float = 0.4, max_wait_s: float = 2.0) -> str:
        end_time = time.monotonic() + max_wait_s
        last_data = time.monotonic()
        chunks: list[str] = []
        while time.monotonic() < end_time:
            data = self._read_available()
            if data:
                chunks.append(data)
                last_data = time.monotonic()
                continue
            if time.monotonic() - last_data >= quiet_s:
                break
            time.sleep(0.05)
        return "".join(chunks)

    def _read_until(self, predicate, timeout: float, settle_s: float = 0.5) -> str:
        start = time.monotonic()
        matched_at: float | None = None
        chunks: list[str] = []

        while time.monotonic() - start < timeout:
            data = self._read_available()
            if data:
                chunks.append(data)
                current = "".join(chunks)
                if predicate(current):
                    if matched_at is None:
                        matched_at = time.monotonic()
                else:
                    matched_at = None
                continue

            if matched_at is not None and time.monotonic() - matched_at >= settle_s:
                break
            time.sleep(0.05)

        return "".join(chunks)

    def reset_to_shell(self) -> str:
        try:
            self.ser.dtr = True
            self.ser.rts = True
            time.sleep(0.05)
            self.ser.rts = False
        except SerialException as exc:
            raise RuntimeError(f"serial reset failed: {exc}") from exc

        time.sleep(0.2)
        self.ser.reset_input_buffer()

        boot_log = self._read_until(
            lambda text: "LittleFS mounted at /lfs" in text or PROMPT in text,
            self.boot_timeout,
            settle_s=0.3,
        )

        if PROMPT not in boot_log:
            self.send_raw("\r\n")
            boot_log += self._read_until(lambda text: PROMPT in text, 2.0, settle_s=0.2)

        if PROMPT not in boot_log:
            raise RuntimeError("shell prompt not detected after reset")

        return boot_log

    def send_raw(self, text: str) -> None:
        try:
            self.ser.write(text.encode("utf-8"))
            self.ser.flush()
        except SerialException as exc:
            raise RuntimeError(f"serial write failed: {exc}") from exc

    def run_shell_command(self, command: str, timeout: float = 5.0) -> str:
        self.ser.reset_input_buffer()
        self.send_raw(command + "\r\n")
        output = self._read_until(lambda text: PROMPT in text, timeout, settle_s=0.2)
        if PROMPT not in output:
            raise RuntimeError(f"prompt not seen after command: {command}")
        return output

    def run_js_case(self, case: TestCase) -> TestResult:
        self.ser.reset_input_buffer()
        self.send_raw(case.command + "\r\n")

        output = self._read_until(
            lambda text: case.success_marker in text or any(marker in text for marker in ERROR_MARKERS),
            case.timeout,
            settle_s=0.4,
        )
        output += self._drain_until_idle(quiet_s=0.3, max_wait_s=1.0)

        if case.success_marker in output and not any(marker in output for marker in ERROR_MARKERS):
            return TestResult(case=case, passed=True, reason="completed", output=output)

        if any(marker in output for marker in ERROR_MARKERS):
            marker = next(marker for marker in ERROR_MARKERS if marker in output)
            return TestResult(case=case, passed=False, reason=f"detected {marker}", output=output)

        return TestResult(case=case, passed=False, reason="timeout waiting for completion", output=output)


def build_cases(tests2_mode: str, case_timeout: float) -> list[TestCase]:
    cases: list[TestCase] = []

    for path in sorted((LFS_DIR / "tests").glob("*.js")):
        device_path = f"/lfs/tests/{path.name}"
        cases.append(
            TestCase(
                label=f"tests/{path.name}",
                command=f"js_run {device_path}",
                success_marker=f"JS completed: {device_path}",
                timeout=case_timeout,
            )
        )

    tests2_dir = LFS_DIR / "tests2"
    suite = tests2_dir / "run_es5.js"

    if tests2_mode in ("suite", "both") and suite.exists():
        device_path = f"/lfs/tests2/{suite.name}"
        cases.append(
            TestCase(
                label=f"tests2/{suite.name}",
                command=f"js_run {device_path}",
                success_marker=f"JS completed: {device_path}",
                timeout=max(case_timeout, 45.0),
            )
        )

    if tests2_mode in ("individual", "both"):
        for path in sorted(tests2_dir.glob("*.js")):
            if path.name == "run_es5.js":
                continue
            device_path = f"/lfs/tests2/{path.name}"
            cases.append(
                TestCase(
                    label=f"tests2/{path.name}",
                    command=f"js_run {device_path}",
                    success_marker=f"JS completed: {device_path}",
                    timeout=case_timeout,
                )
            )

    return cases


def write_log(log_path: Path, header: str, boot_log: str, preflight: list[tuple[str, str]], results: list[TestResult]) -> None:
    sections = [header, "=== Boot ===", boot_log.strip() or "(no boot log)"]

    sections.append("=== Preflight ===")
    for command, output in preflight:
        sections.append(f"$ {command}")
        sections.append(output.strip() or "(no output)")

    sections.append("=== Cases ===")
    for result in results:
        status = "PASS" if result.passed else "FAIL"
        sections.append(f"[{status}] {result.case.label} ({result.reason})")
        sections.append(result.output.strip() or "(no output)")

    log_path.parent.mkdir(parents=True, exist_ok=True)
    log_path.write_text("\n\n".join(sections) + "\n", encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run mquickjs tests and tests2 over serial")
    parser.add_argument("port", help="serial port, for example /dev/ttyACM1")
    parser.add_argument("--baudrate", type=int, default=DEFAULT_BAUDRATE, help="serial baudrate")
    parser.add_argument(
        "--tests2-mode",
        choices=("suite", "individual", "both"),
        default="suite",
        help="how to execute tests2",
    )
    parser.add_argument(
        "--case-timeout",
        type=float,
        default=DEFAULT_CASE_TIMEOUT,
        help="timeout in seconds for each individual JS case",
    )
    parser.add_argument(
        "--boot-timeout",
        type=float,
        default=DEFAULT_BOOT_TIMEOUT,
        help="timeout in seconds waiting for shell after reset",
    )
    parser.add_argument(
        "--log-file",
        default=None,
        help="optional path for the full run log",
    )
    return parser.parse_args()


def main() -> int:
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(line_buffering=True)
    if hasattr(sys.stderr, "reconfigure"):
        sys.stderr.reconfigure(line_buffering=True)

    args = parse_args()
    cases = build_cases(args.tests2_mode, args.case_timeout)
    if not cases:
        print("No test cases found under lfs/tests or lfs/tests2", file=sys.stderr)
        return 2

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    default_log = ROOT_DIR / "build" / "serial_test_logs" / f"mquickjs_serial_tests_{timestamp}.log"
    log_path = Path(args.log_file).resolve() if args.log_file else default_log

    header = (
        f"port={args.port}\n"
        f"baudrate={args.baudrate}\n"
        f"tests2_mode={args.tests2_mode}\n"
        f"case_timeout={args.case_timeout}\n"
        f"boot_timeout={args.boot_timeout}"
    )

    preflight: list[tuple[str, str]] = []
    results: list[TestResult] = []
    runner: SerialRunner | None = None
    boot_log = ""

    try:
        runner = SerialRunner(args.port, args.baudrate, args.boot_timeout)
        boot_log = runner.reset_to_shell()

        for command in ("lfs_ls /lfs", "js_status"):
            output = runner.run_shell_command(command)
            preflight.append((command, output))

        for case in cases:
            print(f"[RUN ] {case.label}")
            result = runner.run_js_case(case)
            results.append(result)
            print(f"[{ 'PASS' if result.passed else 'FAIL' }] {case.label} - {result.reason}")

    except Exception as exc:  # noqa: BLE001
        failure_case = TestCase(label="runner", command="(internal)", success_marker="", timeout=0.0)
        results.append(TestResult(case=failure_case, passed=False, reason=str(exc), output=""))
        print(f"[FAIL] runner - {exc}", file=sys.stderr)
    finally:
        if runner is not None:
            runner.close()

    write_log(log_path, header, boot_log, preflight, results)

    total = len([result for result in results if result.case.label != "runner"]) or len(results)
    passed = sum(1 for result in results if result.passed)
    failed = len(results) - passed

    print()
    print(f"Log file: {log_path}")
    print(f"Total: {total}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")

    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())