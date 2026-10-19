/* Common target configuration, mirroring bflb_flash/targets/common.py. */
(function (root, factory) {
  "use strict";
  const api = factory();
  root.BflbFlashTargetCommon = api;
  if (typeof module !== "undefined" && module.exports) module.exports = api;
})(typeof globalThis !== "undefined" ? globalThis : window, function () {
  "use strict";

  function uartConfig(options) {
    return Object.freeze({
      chip: options.chip,
      baudRate: options.baudRate ?? 2000000,
      writeChunkSize: options.writeChunkSize ?? 2048,
      eraseTimeoutMs: options.eraseTimeoutMs ?? 100000,
      handshakeRetries: options.handshakeRetries ?? (options.chip === "bl602" ? 2 : 3),
      handshakeDurationMs: options.handshakeDurationMs ?? 6,
      loadFunction: options.loadFunction ?? 2,
      helper: options.helper || null,
      clockParameters: options.clockParameters || null,
      flashClock: options.flashClock ?? 0x41,
      flashClockDelay: options.flashClockDelay ?? 0,
      flashIoMode: options.flashIoMode ?? 1,
      flashPin: options.flashPin ?? 0x80,
      flashParameters: options.flashParameters || `targets/${options.chip}/flash_para.bin`,
    });
  }

  return { uartConfig };
});
