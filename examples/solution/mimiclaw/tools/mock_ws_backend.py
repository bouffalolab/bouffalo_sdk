#!/usr/bin/env python3
"""
Minimal WebSocket backend for mimiclaw bring-up.

Features:
- Accept device WS connections (ws://<pc-ip>:8080/)
- Interactive shell to send test frames to device
- Built-in `tool` command to trigger device-side `tool_exec`
"""

import argparse
import asyncio
import json
import time
from typing import Dict

from websockets.server import serve, WebSocketServerProtocol


class MockBackend:
    def __init__(self) -> None:
        self.clients_by_id: Dict[str, WebSocketServerProtocol] = {}
        self.ids_by_client: Dict[WebSocketServerProtocol, str] = {}
        self.next_client_id = 1
        self.req_id = 1

    def _alloc_client_id(self) -> str:
        cid = f"c{self.next_client_id}"
        self.next_client_id += 1
        return cid

    def _alloc_req_id(self) -> str:
        rid = f"req_{int(time.time())}_{self.req_id}"
        self.req_id += 1
        return rid

    async def _send(self, target: str, payload: str) -> None:
        if target == "all":
            if not self.clients_by_id:
                print("[backend] no clients")
                return
            dead = []
            for cid, ws in self.clients_by_id.items():
                try:
                    await ws.send(payload)
                except Exception:
                    dead.append(cid)
            for cid in dead:
                self._remove_client(cid)
            print(f"[backend] sent to all ({len(self.clients_by_id)} clients)")
            return

        ws = self.clients_by_id.get(target)
        if not ws:
            print(f"[backend] client not found: {target}")
            return
        try:
            await ws.send(payload)
            print(f"[backend] sent to {target}")
        except Exception as e:
            print(f"[backend] send failed to {target}: {e}")
            self._remove_client(target)

    def _remove_client(self, client_id: str) -> None:
        ws = self.clients_by_id.pop(client_id, None)
        if ws is not None:
            self.ids_by_client.pop(ws, None)
            print(f"[backend] client removed: {client_id}")

    async def ws_handler(self, websocket: WebSocketServerProtocol) -> None:
        client_id = self._alloc_client_id()
        self.clients_by_id[client_id] = websocket
        self.ids_by_client[websocket] = client_id
        peer = getattr(websocket, "remote_address", None)
        print(f"[backend] connected {client_id} from {peer}")

        hello = json.dumps({
            "type": "hello",
            "chat_id": client_id,
            "content": "mock backend ready",
        }, ensure_ascii=False)
        await self._send(client_id, hello)

        try:
            async for message in websocket:
                if isinstance(message, bytes):
                    print(f"[backend] {client_id} <= <{len(message)} bytes binary>")
                    continue

                print(f"[backend] {client_id} <= {message}")
                try:
                    obj = json.loads(message)
                except Exception:
                    continue

                mtype = obj.get("type")
                if mtype == "tool_result":
                    tool = obj.get("tool", "?")
                    rid = obj.get("id", "")
                    ret = obj.get("ret", "")
                    out = obj.get("output", "")
                    preview = out if len(out) < 180 else out[:180] + "..."
                    print(f"[backend] tool_result id={rid} tool={tool} ret={ret} output={preview}")
                elif mtype == "response":
                    chat_id = obj.get("chat_id", "")
                    out = obj.get("content", "")
                    preview = out if len(out) < 180 else out[:180] + "..."
                    print(f"[backend] response chat_id={chat_id} content={preview}")
                elif mtype == "pong":
                    print(f"[backend] pong from {client_id}")

        except Exception as e:
            print(f"[backend] {client_id} connection error: {e}")
        finally:
            self._remove_client(client_id)

    async def repl(self) -> None:
        help_text = (
            "\nCommands:\n"
            "  help\n"
            "  list\n"
            "  send <client|all> <text>\n"
            "  msg <client|all> <text>\n"
            "  json <client|all> <json_string>\n"
            "  tool <client|all> <tool_name> [input_json]\n"
            "  quit\n"
        )
        print(help_text)
        while True:
            line = await asyncio.to_thread(input, "mock_ws> ")
            line = line.strip()
            if not line:
                continue
            if line == "help":
                print(help_text)
                continue
            if line == "list":
                if not self.clients_by_id:
                    print("[backend] no clients")
                else:
                    print("[backend] clients:", ", ".join(sorted(self.clients_by_id.keys())))
                continue
            if line == "quit":
                print("[backend] bye")
                break

            parts = line.split(" ", 2)
            cmd = parts[0]
            if cmd in ("send", "msg", "json", "tool"):
                if len(parts) < 3:
                    print(f"[backend] usage: {cmd} <client|all> ...")
                    continue
                target = parts[1]
                rest = parts[2]

                if cmd == "send":
                    await self._send(target, rest)
                    continue

                if cmd == "msg":
                    frame = {
                        "type": "message",
                        "content": rest,
                    }
                    if target != "all":
                        frame["chat_id"] = target
                    await self._send(target, json.dumps(frame, ensure_ascii=False))
                    continue

                if cmd == "json":
                    try:
                        json.loads(rest)
                    except Exception as e:
                        print(f"[backend] invalid json: {e}")
                        continue
                    await self._send(target, rest)
                    continue

                # tool
                tparts = rest.split(" ", 1)
                tool_name = tparts[0]
                input_json = "{}" if len(tparts) == 1 else tparts[1]
                try:
                    input_obj = json.loads(input_json)
                except Exception as e:
                    print(f"[backend] invalid input_json: {e}")
                    continue

                frame = {
                    "type": "tool_exec",
                    "id": self._alloc_req_id(),
                    "tool": tool_name,
                    "input": input_obj,
                }
                await self._send(target, json.dumps(frame, ensure_ascii=False))
                continue

            print(f"[backend] unknown command: {cmd}")


async def main() -> None:
    parser = argparse.ArgumentParser(description="Mock WS backend for mimiclaw")
    parser.add_argument("--host", default="0.0.0.0", help="bind host")
    parser.add_argument("--port", default=8080, type=int, help="bind port")
    args = parser.parse_args()

    backend = MockBackend()

    async with serve(backend.ws_handler, args.host, args.port, max_size=1024 * 1024):
        print(f"[backend] listening on ws://{args.host}:{args.port}/")
        await backend.repl()


if __name__ == "__main__":
    asyncio.run(main())
