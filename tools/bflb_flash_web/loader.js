/* Shared multi-chip flashing workflow for browsers and Node.js. */
(function (root, factory) {
  "use strict";
  const isNode = typeof module !== "undefined" && module.exports;
  const protocol = isNode ? require("./protocol.js") : root.BflbFlashProtocol;
  const targets = isNode ? require("./targets/index.js") : root.BflbFlashTargets;
  const nodeCrypto = isNode ? require("node:crypto") : null;
  const api = factory(protocol, targets, nodeCrypto, isNode);
  root.BflbFlasher = api;
  if (isNode) module.exports = api;
})(typeof globalThis !== "undefined" ? globalThis : window, function (protocol, targets, nodeCrypto, isNode) {
  "use strict";
  const { COMMAND, buildFrame, bytesToHex, concatBytes, flashSizeFromJedec, uint32LE } = protocol;
  const TARGETS = targets.CHIP_DEFS;
  const DEFAULT_BAUD_RATE = 2000000;
  const WRITE_CHUNK_SIZE = 2048;
  const CHECKSUM_RETRIES = 3;
  function delay(ms) { return new Promise((resolve) => setTimeout(resolve, ms)); }
  async function sha256(data) {
    if (nodeCrypto) return new Uint8Array(nodeCrypto.createHash("sha256").update(data).digest());
    if (!globalThis.crypto?.subtle) throw new Error("当前环境不支持 SHA-256");
    return new Uint8Array(await globalThis.crypto.subtle.digest("SHA-256", data));
  }
  async function loadResource(resourcePath) {
    if (isNode) {
      const fs = require("node:fs/promises");
      const path = require("node:path");
      return new Uint8Array(await fs.readFile(path.join(__dirname, resourcePath)));
    }
    const response = await fetch(resourcePath);
    if (!response.ok) throw new Error(`无法读取目标资源 ${resourcePath}（HTTP ${response.status}）`);
    return new Uint8Array(await response.arrayBuffer());
  }
  function targetForChip(chip) {
    const key = String(chip || "").toLowerCase();
    const target = TARGETS[key];
    if (!target) throw new Error(`不支持的芯片：${chip}`);
    return target;
  }
  function targetFromBootInfo(bootInfo, requestedChip) {
    if (requestedChip) {
      const target = targetForChip(requestedChip);
      target.detectVersion(bootInfo);
      return target;
    }
    for (const target of Object.values(TARGETS)) {
      try { target.detectVersion(bootInfo); return target; } catch (_) { /* try next target */ }
    }
    throw new Error(`不支持的 BootROM 芯片标识 0x${bytesToHex(bootInfo.slice(0, 4))}`);
  }
  class BflbFlasher {
    constructor(transport, callbacks = {}) {
      this.transport = transport;
      this.onLog = callbacks.onLog || (() => {});
      this.onProgress = callbacks.onProgress || (() => {});
      this.isCancelled = callbacks.isCancelled || (() => false);
      this.target = null; this.version = null; this.bootInfo = null; this.needsHandshake = false;
    }
    log(message, level = "info") { this.onLog(message, level); }
    progress(value, stage, detail) { this.onProgress(value, stage, detail); }
    ensureNotCancelled() { if (this.isCancelled()) { const error = new Error("用户取消烧写"); error.name = "AbortError"; throw error; } }
    async resetChip(boot) {
      await this.transport.setSignals({ dataTerminalReady: boot }); await delay(10);
      await this.transport.setSignals({ dataTerminalReady: !boot }); await delay(10);
      await this.transport.setSignals({ requestToSend: true }); await delay(10);
      await this.transport.setSignals({ requestToSend: false }); await delay(50);
    }
    async autoResetIntoBootrom() {
      try {
        await this.resetChip(true);
      } catch (error) { throw new Error(`自动复位失败，请关闭自动复位并手动进入 BootROM：${error.message}`); }
    }
    async handshake(autoReset, retries, durationMs) {
      const ok = new TextEncoder().encode("OK");
      const baudRate = this.transport.baudRate || DEFAULT_BAUD_RATE;
      const syncLength = Math.min(512, Math.floor((durationMs / 1000) * baudRate / 10));
      for (let attempt = 1; attempt <= retries; attempt += 1) {
        this.ensureNotCancelled(); if (autoReset) await this.autoResetIntoBootrom();
        this.transport.clearInput(); await this.transport.write(new Uint8Array(syncLength).fill(0x55));
        try { await this.transport.readUntil(ok, 1200); this.transport.clearInput(); this.log(`BootROM 握手成功（第 ${attempt} 次）`, "success"); return; }
        catch (error) {
          if (this.transport.readError) throw new Error(`串口读取失败：${error.message}`);
          this.log(`BootROM 握手第 ${attempt} 次未响应：${error.message}`, "warn");
          if (!autoReset) await delay(500);
        }
      }
      throw new Error("BootROM 握手失败。请按住 BOOT，按一下 RESET，再松开 BOOT 后重试");
    }
    async sendBootCommand(command, payload = new Uint8Array(), response = false, timeoutMs = 3000) {
      await this.transport.write(buildFrame(command, payload, false));
      return response ? this.transport.readResponse(timeoutMs) : this.transport.readAck(timeoutMs);
    }
    async sendFlashCommand(command, payload = new Uint8Array(), response = false, timeoutMs = 3000) {
      await this.transport.write(buildFrame(command, payload, true));
      return response ? this.transport.readResponse(timeoutMs) : this.transport.readAck(timeoutMs);
    }
    async sendBootRaw(packet) { await this.transport.write(packet); return this.transport.readAck(3000); }
    async getBootInfo() {
      const bootInfo = await this.sendBootCommand(COMMAND.GET_BOOT_INFO, new Uint8Array(), true);
      this.bootInfo = bootInfo; this.target = targetFromBootInfo(bootInfo, this.requestedChip); this.version = this.target.detectVersion(bootInfo);
      this.log(`检测到 ${this.target.CHIP_NAME.toUpperCase()} ${this.version.toUpperCase()}，BootROM 0x${bytesToHex(bootInfo.slice(0, 4))}`);
      this.log(`BootInfo 0x${bytesToHex(bootInfo)}`); return bootInfo;
    }
    async setDeviceTimeout(timeoutMs) {
      if (this.target.CHIP_NAME !== "bl616") return;
      if (this.version === "a0") await this.sendBootCommand(COMMAND.MEMORY_WRITE, concatBytes(uint32LE(0x6102df04), uint32LE((timeoutMs << 16) | 0x1200)));
      else await this.sendBootCommand(COMMAND.SET_TIMEOUT, uint32LE(timeoutMs));
    }
    async loadHelper() {
      const helper = await loadResource(this.target.CONFIG.helper);
      if (helper.length < 0xb0) throw new Error("eflash_loader.bin 头部无效");
      const header = helper.slice(0, 0xb0); const segmentCount = new DataView(header.buffer, header.byteOffset).getUint32(120, true);
      await this.sendBootCommand(COMMAND.LOAD_BOOT_HEADER, header); let offset = 0xb0;
      for (let index = 0; index < segmentCount; index += 1) {
        const segmentHeader = helper.slice(offset, offset + 0x10); offset += 0x10;
        if (segmentHeader.length !== 0x10) throw new Error("eflash_loader.bin 段头部无效");
        const response = await this.sendBootCommand(COMMAND.LOAD_SEG_HEADER, segmentHeader, true);
        if (response.length < 8) throw new Error("eflash_loader 段头部响应无效");
        const segmentLength = new DataView(response.buffer, response.byteOffset + 4).getUint32(0, true);
        for (let sent = 0; sent < segmentLength; ) {
          const chunk = helper.slice(offset + sent, offset + Math.min(sent + 4080, segmentLength));
          if (!chunk.length) throw new Error("eflash_loader.bin 内容不完整");
          await this.sendBootCommand(COMMAND.LOAD_SEG_DATA, chunk); sent += chunk.length;
        }
        offset += segmentLength;
      }
      await this.sendBootCommand(COMMAND.CHECK_IMAGE);
      if (this.target.runImagePreamble) await this.sendBootRaw(this.target.runImagePreamble()); else await this.sendBootCommand(COMMAND.RUN_IMAGE);
      this.needsHandshake = true; this.log(`已加载 ${this.target.CONFIG.helper}`, "success");
    }
    async configureBaudRate() {
      if (this.target.CONFIG.loadFunction !== 2) return;
      const parameters = this.target.CONFIG.clockParameters ? await loadResource(this.target.CONFIG.clockParameters) : new Uint8Array();
      const baudRate = this.transport.baudRate || DEFAULT_BAUD_RATE;
      await this.sendFlashCommand(COMMAND.CLOCK_SET, concatBytes(uint32LE(1), uint32LE(baudRate), parameters)); this.transport.clearInput(); await delay(10);
      this.log(`串口使用 ${baudRate.toLocaleString()} baud`);
    }
    async retryFlash(command, payload = new Uint8Array(), response = false, timeoutMs = 3000) {
      for (let attempt = 0; attempt <= CHECKSUM_RETRIES; attempt += 1) {
        try { return await this.sendFlashCommand(command, payload, response, timeoutMs); }
        catch (error) { if (attempt === CHECKSUM_RETRIES) throw error; this.log(`命令 ${command.toString(16)} 重试`, "warn"); }
      }
      throw new Error("unreachable");
    }
    flashPin() { return this.target.CONFIG.flashPin === 0x80 ? this.target.flashPinFromBootInfo(this.bootInfo) : this.target.CONFIG.flashPin; }
    async configureFlash() {
      if (this.needsHandshake) { await this.handshake(false, 2, this.target.CONFIG.handshakeDurationMs); this.needsHandshake = false; }
      const pin = this.flashPin();
      const config = (pin & 0xff) | (this.target.CONFIG.flashClock << 8) | (this.target.CONFIG.flashIoMode << 16) | (this.target.CONFIG.flashClockDelay << 24);
      this.log(`Flash pin ${pin}，配置 0x${config.toString(16).padStart(8, "0")}`);
      if (config !== 0x0101ff || this.target.CONFIG.loadFunction === 2) await this.retryFlash(COMMAND.FLASH_SET_PARA, uint32LE(config));
      const jedec = await this.retryFlash(COMMAND.FLASH_READ_JEDEC_ID, new Uint8Array(), true);
      if (jedec.length < 4) throw new Error(`Flash JEDEC ID 响应长度无效：${jedec.length}`);
      if (jedec[0] === 0 && jedec[1] === 0 && jedec[2] === 0) {
        throw new Error(`Flash JEDEC ID 无效：返回 0x${bytesToHex(jedec)}，pin ${pin}，配置 0x${config.toString(16).padStart(8, "0")}`);
      }
      const flashSize = flashSizeFromJedec(jedec); this.log(`Flash JEDEC ID 0x${bytesToHex(jedec.slice(0, 3))}，容量 ${flashSize} 字节`);
      if (jedec[3] !== 0x80) {
        const parameters = await loadResource(this.target.CONFIG.flashParameters);
        await this.retryFlash(COMMAND.FLASH_SET_PARA, concatBytes(uint32LE(config), parameters));
      }
      return { config, pin, jedec, flashSize };
    }
    async eraseFlash(address, length) {
      await this.transport.write(buildFrame(COMMAND.FLASH_ERASE, concatBytes(uint32LE(address), uint32LE(address + length - 1)), true));
      while (true) { const ack = await this.transport.readAck(this.target.CONFIG.eraseTimeoutMs); if (ack.status === "OK") return; if (ack.status === "PD") this.log("Flash 正在擦除", "warn"); }
    }
    async writeFlash(data, address) {
      await this.setDeviceTimeout(2000);
      for (let offset = 0; offset < data.length; offset += WRITE_CHUNK_SIZE) {
        this.ensureNotCancelled(); const chunk = data.subarray(offset, Math.min(offset + WRITE_CHUNK_SIZE, data.length));
        await this.retryFlash(COMMAND.FLASH_WRITE, concatBytes(uint32LE(address + offset), chunk));
        this.progress(15 + ((offset + chunk.length) / data.length) * 70, "write", { written: offset + chunk.length, total: data.length });
      }
      await this.retryFlash(COMMAND.FLASH_WRITE_CHECK);
    }
    async verifyFlash(data, address) {
      this.progress(88, "hash"); const localHash = await sha256(data); await this.retryFlash(COMMAND.FLASH_XIP_READ_START); let deviceHash;
      try { deviceHash = await this.retryFlash(COMMAND.FLASH_XIP_READ_SHA, concatBytes(uint32LE(address), uint32LE(data.length)), true, Math.max(5000, Math.ceil(data.length / (1024 * 1024)) * 5000)); }
      finally { await this.retryFlash(COMMAND.FLASH_XIP_READ_FINISH); }
      if (bytesToHex(localHash) !== bytesToHex(deviceHash)) throw new Error(`SHA-256 校验失败：本地 ${bytesToHex(localHash)}，设备 ${bytesToHex(deviceHash)}`);
      this.log(`SHA-256 ${bytesToHex(localHash)}`, "success"); return localHash;
    }
    async resetToRun() {
      try { await this.resetChip(false); }
      catch (error) { this.log(`无法通过 DTR/RTS 复位，请手动按 RESET：${error.message}`, "warn"); }
    }
    async flash(data, options = {}) {
      if (!(data instanceof Uint8Array)) data = new Uint8Array(data); if (!data.length) throw new Error("文件为空");
      this.requestedChip = options.chip || null; const address = options.address >>> 0 || 0; const startedAt = Date.now();
      const initialTarget = this.requestedChip ? targetForChip(this.requestedChip) : null;
      this.progress(1, "handshake"); await this.handshake(options.autoReset !== false, initialTarget?.CONFIG.handshakeRetries || 3, initialTarget?.CONFIG.handshakeDurationMs || 6); this.progress(4, "boot-info"); await delay(100); await this.getBootInfo();
      if (this.target.CONFIG.loadFunction === 1) await this.loadHelper(); else { await this.setDeviceTimeout(10000); await this.configureBaudRate(); }
      this.progress(7, "flash-config"); const flash = await this.configureFlash(); if (address + data.length > flash.flashSize) throw new Error(`镜像范围超过 Flash 容量 ${flash.flashSize} 字节`);
      this.progress(10, "erase"); await this.eraseFlash(address, data.length); this.progress(15, "write", { written: 0, total: data.length }); await this.writeFlash(data, address); this.progress(87, "verify");
      const hash = await this.verifyFlash(data, address); this.progress(100, "done"); if (options.runAfterFlash !== false) await this.resetToRun();
      return { chip: this.target.CHIP_NAME, version: this.version, bootInfo: this.bootInfo, jedec: flash.jedec, flashSize: flash.flashSize, flashConfig: flash.config, flashPin: flash.pin, sha256: hash, elapsedMs: Date.now() - startedAt };
    }
    async flashWhole(data, options = {}) {
      if (!(data instanceof Uint8Array)) data = new Uint8Array(data);
      if (data.length < 256) throw new Error("文件太小，不是有效的 whole.bin");
      if (String.fromCharCode(...data.slice(0, 4)) !== "BFNP") throw new Error("文件 0x0 处没有 Bouffalo Boot Header (BFNP)");
      return this.flash(data, { ...options, address: 0 });
    }
  }
  return { BflbFlasher, BL616Flasher: BflbFlasher, BAUD_RATE: DEFAULT_BAUD_RATE, WRITE_CHUNK_SIZE, sha256, targetForChip };
});
