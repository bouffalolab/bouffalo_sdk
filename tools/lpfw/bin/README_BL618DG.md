Release Date: 2026-10-15
Release Author: whuan
Git Commit Version: 88f89f2b888a3acf9b074be3e4b83a6e89c60875

Submodule Information:
-3bc6321603b0ab07c39ebdbe6ad1c65e0f4beb45 ../../../../components/crypto/mbedtls/mbedtls
-4c3acca7109deb4dca6956dd5f3ceaf0947dee58 ../../../../components/crypto/mbedtls/mbedtls_v3
-060ca0b860ab93ff09a207fb80449a51bf6755d9 ../../../../components/fs
-b6edf14d79d23ba6974e56730b0f936177cac782 ../../../../components/graphics
-8431fe111a8d0f09a463a8d1f3eb42a7de753892 ../../../../components/ipc/libmetal/libmetal
-76a12961fceda27871eb9338ae802c2797babad9 ../../../../components/ipc/open-amp/open-amp
-7bb2b423f2d923c28b243aa9af8245891e74d32d ../../../../components/multimedia
-d53624f592fc3be7a2d9c16920330650f68874fa ../../../../components/net/lwip/lwip
-5e8f7953c4bcc6c2f091eb60759f6628f8968455 ../../../../components/net/netbus/atmodule
-8d7374a3ffa44812590b0f75b36498f8dff0a0f9 ../../../../components/net/netbus/host_router
-47765b8917a01ff6e2bfbab891f151291b63e028 ../../../../components/net/nethub/host_linux
-a5c3e0808e3dc188c49e75b5fe482459c29447da ../../../../components/usb/cherryusb
-ab3e6d96e00da02beee1372f00d3f53811c8c48e ../../../../components/wireless/bl3141_drv
-b31f1532687b2c6f77491928d7b4997b8d675fa4 ../../../../components/wireless/bluetooth
-bd46ba1de2a7a92ed86c58cd5f01ad6050a13026 ../../../../components/wireless/lmac154
-37c92036ea14e00f92b7295e054cd52052546e0e ../../../../components/wireless/macsw
-f15f28f97477c5e60e685ca33d1711ac321e1d1c ../../../../components/wireless/matter/mfd
-47eca7301e213d161c3a98bd39e0c194a07b354d ../../../../components/wireless/thread
-820badeede80b2f95c271b0446af4725f8edcc0a ../../../../components/wireless/wifi4/firmware
-558df9c7539a7699d34fc4b0c4d71af74ca1129a ../../../../components/wireless/wifi4/manager
-54c8b89868daacf92ea53a7146d959d6af89c233 ../../../../components/wireless/wifi6
-0107692faf67fe399491df87040ce460782a23b0 ../../../../components/wireless/wl80211
-e3c9a1806290fb5cedfee7f3129444bb33bc1d7b ../../../../components/wireless/wlan/linux_driver
-3af9e726eee18f820d72f3fcad45365664770f8a ../../../../components/wireless/wlan/wlan_mac
-0293dd07256c8f8f9b2e8e93897114dc748ed489 ../../../../components/wireless/zigbee
-86c6fc5068d6d37aad7fb5959370ea1504e6ca73 ../../../../drivers/lhal
-25ffb329a3b97c5d4fc79d66c60e0b988882d996 ../../../../drivers/soc/bl602/phyrf
-1d253d273aa044ec80c3b0a0f17a7a6f0a0531e1 ../../../../drivers/soc/bl602/std
-450aef3d18fe1eb9333f833f9e3cde8ba056e3a3 ../../../../drivers/soc/bl616/phyrf
-7a68447cd346187d13b0de287e316c826eaaf6e1 ../../../../drivers/soc/bl616/std
-216bdf53aca8dce7989c51d0f3ca9d0b4642cbc3 ../../../../drivers/soc/bl616cl/phyrf
-19c175719ee4b567999b58e8f8507f006b1f5ab4 ../../../../drivers/soc/bl616cl/std
-6711ec4b5174af367746bd64afeb8b8834ea9aaf ../../../../drivers/soc/bl618dg/phyrf
-61e747f04595b9240e6ad7e69943b022767caf48 ../../../../drivers/soc/bl618dg/std
-712e35f8da9fc43c920cd0259a04ed9a78276d01 ../../../../drivers/soc/bl702/phyrf
-0eb29ac433b54e2c55fdf99f29cc036982f6e1d2 ../../../../drivers/soc/bl702/std
-89a6a4072e1f48b1d245deced6c0cd44f65b77fb ../../../../drivers/soc/bl702l/phyrf
-e5a4a460bfbea8036dc9638e7b371ebaed69827b ../../../../drivers/soc/bl702l/std
-5e61af5721cee088a00e6bcf6bc428444bdd0a23 ../../../../drivers/sys
-3a2c64f662b1c6d74302326645ad41a41f810684 ../../../../tools/autotest
-14fcd4a6c694825bb04d3dc943ab6fa2c3daaca3 ../../../../tools/bflb_tools

Validation Baseline:
- Target: BL618DG B0
- Build command: make CHIP=bl618dg BOARD=bl618dg_lpfw_board CPU_ID=ap CPU_MODEL=b0 MAKE_LPFW=y LOG_SEL=2 CONFIG_ROMAPI=y
- LPFW binary: lpfw_bin/bl618dg_lp_fw.bin
- Binary size: 46048 bytes
- SHA256: 3b265e632323f604daa32028022ca821cc852099e47dfc13d00258af571030a4
- Link memory: check build output and build/build_out/lp_fw_bl618dg_ap.map
- Sync target: tools/lpfw/bin/bl618dg_lp_fw.bin
- Consumer app: examples/pmu/wl_lp
- App build command: make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0

Board validation still needs to record boot logs, PDS entry, wakeup logs, Wi-Fi low-power RX behavior, stress result, and current measurement.
