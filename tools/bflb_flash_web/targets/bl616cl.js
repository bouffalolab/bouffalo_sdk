/* BL616CL target definition, mirroring bflb_flash/targets/__init__.py. */
(function (root, factory) {
  "use strict";
  const isNode = typeof module !== "undefined" && module.exports;
  const common = isNode ? require("./common.js") : root.BflbFlashTargetCommon;
  const api = factory(common);
  root.BflbFlashTargetBL616CL = api;
  if (isNode) module.exports = api;
})(typeof globalThis !== "undefined" ? globalThis : window, function (common) {
  "use strict";
  const CONFIG = common.uartConfig({ chip: "bl616cl", flashClock: 0x10, flashClockDelay: 1 });
  const VERSION_IDS = Object.freeze({ "01a01606": "a0", "02a01606": "a1" });
  function bytesToHex(bytes) { return Array.from(bytes, (value) => value.toString(16).padStart(2, "0")).join(""); }
  function detectVersion(bootInfo) {
    const id = bytesToHex(bootInfo.slice(0, 4));
    const version = VERSION_IDS[id];
    if (!version) throw new Error(`不支持的 BL616CL BootROM，芯片标识为 0x${id || "????????"}`);
    return version;
  }
  function flashPinFromBootInfo(bootInfo) {
    if (bootInfo.length < 24) throw new Error("BL616CL BootInfo 长度无效");
    const usage = new DataView(bootInfo.buffer, bootInfo.byteOffset + 20, 4).getUint32(0, true);
    return (usage >>> 5) & 0x3f;
  }
  function flashConfigFromBootInfo(bootInfo) {
    return (0x01 << 24) | (0x01 << 16) | (0x10 << 8) | flashPinFromBootInfo(bootInfo);
  }
  return Object.freeze({ CHIP_NAME: "bl616cl", CONFIG, VERSION_IDS, detectVersion, flashPinFromBootInfo, flashConfigFromBootInfo });
});
