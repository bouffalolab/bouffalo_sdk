/* BL616 target definition, mirroring bflb_flash/targets/bl616.py. */
(function (root, factory) {
  "use strict";
  const isNode = typeof module !== "undefined" && module.exports;
  const common = isNode ? require("./common.js") : root.BflbFlashTargetCommon;
  const api = factory(common);
  root.BflbFlashTargetBL616 = api;
  if (isNode) module.exports = api;
})(typeof globalThis !== "undefined" ? globalThis : window, function (common) {
  "use strict";

  const CONFIG = common.uartConfig({ chip: "bl616", flashClock: 0x41, flashClockDelay: 1 });
  const VERSION_IDS = Object.freeze({
    "01001606": "a0",
    "02001606": "a1",
  });

  function bytesToHex(bytes) {
    return Array.from(bytes, (value) => value.toString(16).padStart(2, "0")).join("");
  }

  function detectVersion(bootInfo) {
    const id = bytesToHex(bootInfo.slice(0, 4));
    const version = VERSION_IDS[id];
    if (!version) throw new Error(`不是支持的 BL616 BootROM，芯片标识为 0x${id || "????????"}`);
    return version;
  }

  function flashPinFromBootInfo(bootInfo) {
    if (bootInfo.length < 12) throw new Error("BL616 BootInfo 长度无效");
    const swUsage = new DataView(bootInfo.buffer, bootInfo.byteOffset + 8, 4).getUint32(0, true);
    return (swUsage >>> 14) & 0x3f;
  }

  function flashConfigFromBootInfo(bootInfo) {
    const flashPin = flashPinFromBootInfo(bootInfo);
    return (0x01 << 24) | (0x01 << 16) | (0x41 << 8) | flashPin;
  }

  return Object.freeze({
    CHIP_NAME: "bl616",
    CONFIG,
    VERSION_IDS,
    detectVersion,
    flashConfigFromBootInfo,
    flashPinFromBootInfo,
  });
});
