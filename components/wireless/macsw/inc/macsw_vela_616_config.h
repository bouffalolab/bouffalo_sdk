#ifndef __VELA_616_WIFI_CONFIG_H__
#define __VELA_616_WIFI_CONFIG_H__

/*
 * 本文件用于覆盖默认的 config.h 配置。
 * 编译系统应确保此文件在默认 config.h 之后被包含，
 * 或者通过 CONFIG_MACSW_SELECT 机制包含。
 *
 * 修正说明：
 * - 仅对默认配置中启用（值为1或无值）的功能进行 #undef 以禁用。
 * - 对于默认配置中已禁用（值为0）的功能，无需任何操作。
 */

// 最大 STA 数量

#undef CFG_STA_MAX
#define CFG_STA_MAX 2

// TX PBD 数量
// #define CFG_TX_PBD_CNT 5

/* --- 显式禁用项 --- */
// 以下功能在默认配置中被启用（值为1或无值），但在你的 CFLAGS 中未出现。
// 为确保它们被禁用，在此处显式取消定义。

#endif // __VELA_616_WIFI_CONFIG_H__

