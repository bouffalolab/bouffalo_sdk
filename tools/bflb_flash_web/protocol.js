/* BL616 BootROM flash protocol helpers. No external dependencies. */
(function (root) {
  "use strict";

  const COMMAND = Object.freeze({
    GET_BOOT_INFO: 0x10,
    LOAD_BOOT_HEADER: 0x11,
    LOAD_SEG_HEADER: 0x17,
    LOAD_SEG_DATA: 0x18,
    CHECK_IMAGE: 0x19,
    RUN_IMAGE: 0x1a,
    CLOCK_SET: 0x22,
    SET_TIMEOUT: 0x23,
    FLASH_ERASE: 0x30,
    FLASH_WRITE: 0x31,
    FLASH_BOOT: 0x33,
    FLASH_READ_JEDEC_ID: 0x36,
    FLASH_WRITE_CHECK: 0x3a,
    FLASH_SET_PARA: 0x3b,
    FLASH_XIP_READ_SHA: 0x3e,
    MEMORY_WRITE: 0x50,
    FLASH_XIP_READ_START: 0x60,
    FLASH_XIP_READ_FINISH: 0x61,
  });

  const ERROR_NAMES = Object.freeze({
    0x0001: "FLASH_INIT_ERROR",
    0x0002: "FLASH_ERASE_PARA_ERROR",
    0x0003: "FLASH_ERASE_ERROR",
    0x0004: "FLASH_WRITE_PARA_ERROR",
    0x0005: "FLASH_WRITE_ADDR_ERROR",
    0x0006: "FLASH_WRITE_ERROR",
    0x0008: "FLASH_SET_PARA_ERROR",
    0x0101: "CMD_ID_ERROR",
    0x0102: "CMD_LEN_ERROR",
    0x0103: "CMD_CRC_ERROR",
    0x0104: "CMD_SEQ_ERROR",
  });

  function concatBytes(...parts) {
    const length = parts.reduce((sum, part) => sum + part.length, 0);
    const output = new Uint8Array(length);
    let offset = 0;
    for (const part of parts) {
      output.set(part, offset);
      offset += part.length;
    }
    return output;
  }

  function uint32LE(value) {
    const output = new Uint8Array(4);
    new DataView(output.buffer).setUint32(0, value >>> 0, true);
    return output;
  }

  function bytesToHex(bytes) {
    return Array.from(bytes, (value) => value.toString(16).padStart(2, "0")).join("");
  }

  function buildFrame(command, payload = new Uint8Array(), checksumEnabled = true) {
    if (!(payload instanceof Uint8Array)) {
      payload = new Uint8Array(payload);
    }
    if (payload.length > 0xffff) {
      throw new RangeError("Command payload is too large");
    }

    const output = new Uint8Array(payload.length + 4);
    output[0] = command;
    output[2] = payload.length & 0xff;
    output[3] = payload.length >>> 8;
    output.set(payload, 4);
    if (checksumEnabled) {
      let checksum = output[2] + output[3];
      for (const value of payload) checksum += value;
      output[1] = checksum & 0xff;
    }
    return output;
  }

  function flashSizeFromJedec(jedec) {
    if (jedec.length < 3) throw new Error("Flash JEDEC ID 长度无效");
    const capacityId = jedec[2];
    const level = [0x20, 0x21, 0x22].includes(capacityId)
      ? capacityId - 0x19
      : (capacityId & 0x1f) - 0x13;
    if (level < 0 || level > 8) throw new Error(`无法识别 Flash 容量 ID 0x${capacityId.toString(16)}`);
    return (2 ** level) * 512 * 1024;
  }

  function formatErrorCode(code) {
    const hex = code.toString(16).padStart(4, "0");
    return `0x${hex}${ERROR_NAMES[code] ? ` (${ERROR_NAMES[code]})` : ""}`;
  }

  const api = {
    COMMAND,
    buildFrame,
    bytesToHex,
    concatBytes,
    flashSizeFromJedec,
    formatErrorCode,
    uint32LE,
  };

  root.BflbFlashProtocol = api;
  if (typeof module !== "undefined" && module.exports) module.exports = api;
})(typeof globalThis !== "undefined" ? globalThis : window);
