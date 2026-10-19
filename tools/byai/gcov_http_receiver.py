#!/usr/bin/env python3
import argparse
import os
import re
import threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlparse


class UploadStats:
    def __init__(self) -> None:
        self.lock = threading.Lock()
        self.files = 0
        self.bytes = 0

    def add(self, file_size: int) -> None:
        with self.lock:
            self.files += 1
            self.bytes += file_size

    def snapshot(self) -> tuple[int, int]:
        with self.lock:
            return self.files, self.bytes


def sanitize_filename(name: str) -> str:
    base = os.path.basename(name.strip())
    if not base:
        return ""
    # Keep common gcov dump names while blocking traversal or shell chars.
    if not re.fullmatch(r"[A-Za-z0-9._#\-]+", base):
        return ""
    return base


class ReceiverHandler(BaseHTTPRequestHandler):
    server_version = "GcovReceiver/1.0"

    def do_GET(self) -> None:
        if self.path.startswith("/healthz"):
            files, total_bytes = self.server.stats.snapshot()  # type: ignore[attr-defined]
            body = f"ok files={files} bytes={total_bytes}\n".encode()
            self.send_response(200)
            self.send_header("Content-Type", "text/plain; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
            return
        self.send_error(404, "Not Found")

    def do_POST(self) -> None:
        parsed = urlparse(self.path)
        if parsed.path != "/upload":
            self.send_error(404, "Not Found")
            return

        params = parse_qs(parsed.query)
        filename = sanitize_filename(params.get("filename", [""])[0])
        if not filename:
            self.send_error(400, "Invalid filename")
            return

        raw_len = self.headers.get("Content-Length", "").strip()
        if not raw_len.isdigit():
            self.send_error(411, "Missing or invalid Content-Length")
            return
        content_length = int(raw_len)
        if content_length <= 0:
            self.send_error(400, "Empty body")
            return

        out_dir = self.server.out_dir  # type: ignore[attr-defined]
        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.join(out_dir, filename)

        remaining = content_length
        with open(out_path, "wb") as f:
            while remaining > 0:
                chunk = self.rfile.read(min(65536, remaining))
                if not chunk:
                    self.send_error(400, "Truncated request body")
                    return
                f.write(chunk)
                remaining -= len(chunk)

        self.server.stats.add(content_length)  # type: ignore[attr-defined]
        body = f"saved {filename} {content_length}\n".encode()
        self.send_response(200)
        self.send_header("Content-Type", "text/plain; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt: str, *args) -> None:
        # Keep stdout concise; orchestration script redirects to log file.
        return


def main() -> None:
    parser = argparse.ArgumentParser(description="Receive gcda files over HTTP POST.")
    parser.add_argument("--bind", default="0.0.0.0", help="Bind address (default: 0.0.0.0)")
    parser.add_argument("--port", type=int, default=18080, help="Listen port (default: 18080)")
    parser.add_argument("--out-dir", required=True, help="Output directory for .gcda files")
    args = parser.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)
    server = ThreadingHTTPServer((args.bind, args.port), ReceiverHandler)
    server.out_dir = os.path.abspath(args.out_dir)  # type: ignore[attr-defined]
    server.stats = UploadStats()  # type: ignore[attr-defined]
    print(f"[gcov_http_receiver] listening on {args.bind}:{args.port}, out={server.out_dir}", flush=True)
    server.serve_forever()


if __name__ == "__main__":
    main()
