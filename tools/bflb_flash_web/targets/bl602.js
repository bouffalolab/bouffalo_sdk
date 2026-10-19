/* BL602 target definition, mirroring bflb_flash/targets/__init__.py. */
(function (root, factory) {
  "use strict";
  const isNode = typeof module !== "undefined" && module.exports;
  const common = isNode ? require("./common.js") : root.BflbFlashTargetCommon;
  const api = factory(common);
  root.BflbFlashTargetBL602 = api;
  if (isNode) module.exports = api;
})(typeof globalThis !== "undefined" ? globalThis : window, function (common) {
  "use strict";
  const CONFIG = common.uartConfig({
    chip: "bl602", loadFunction: 1, helper: "targets/bl602/eflash_loader_40m.bin",
    flashClock: 0x01, flashPin: 0xff, flashClockDelay: 0,
    eraseTimeoutMs: 15000,
  });
  const VERSION_IDS = Object.freeze({ "01000000": "a0" });
  function detectVersion(bootInfo) {
    const id = bytesToHex(bootInfo.slice(0, 4));
    const version = VERSION_IDS[id];
    if (!version) throw new Error(`不支持的 BL602 BootROM，芯片标识为 0x${id || "????????"}`);
    return version;
  }
  function bytesToHex(bytes) { return Array.from(bytes, (value) => value.toString(16).padStart(2, "0")).join(""); }
  return Object.freeze({ CHIP_NAME: "bl602", CONFIG, VERSION_IDS, detectVersion, flashPinFromBootInfo: () => 0xff });
});
