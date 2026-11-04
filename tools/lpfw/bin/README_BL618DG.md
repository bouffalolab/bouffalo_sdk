Release Date: 2026-10-23
Release Author: whuan
Git Commit Version: dc95dd702df08cd2fd5f21602694d14aab1853dd

Submodule Information:
-3bc6321603b0ab07c39ebdbe6ad1c65e0f4beb45 ../../../../components/crypto/mbedtls/mbedtls
-4c3acca7109deb4dca6956dd5f3ceaf0947dee58 ../../../../components/crypto/mbedtls/mbedtls_v3
-8a1758f6f0d659d21701db966059e69cd9897b12 ../../../../components/fs
-b6edf14d79d23ba6974e56730b0f936177cac782 ../../../../components/graphics
-8431fe111a8d0f09a463a8d1f3eb42a7de753892 ../../../../components/ipc/libmetal/libmetal
-05bfa4d771d42132a585fa322498d369232216ec ../../../../components/ipc/open-amp/open-amp
-e7f98d6324f3f3878d2ee5d6265bed609e62c174 ../../../../components/multimedia
-d53624f592fc3be7a2d9c16920330650f68874fa ../../../../components/net/lwip/lwip
-9124bfbb188994882489b6020982f7b1308f79e7 ../../../../components/net/netbus/atmodule
-8d7374a3ffa44812590b0f75b36498f8dff0a0f9 ../../../../components/net/netbus/host_router
-47765b8917a01ff6e2bfbab891f151291b63e028 ../../../../components/net/nethub/host_linux
-a5c3e0808e3dc188c49e75b5fe482459c29447da ../../../../components/usb/cherryusb
-ab3e6d96e00da02beee1372f00d3f53811c8c48e ../../../../components/wireless/bl3141_drv
-e4d0f519c96703e40f8a341e3d0ab45e3aa436dd ../../../../components/wireless/bluetooth
-0dc5f1307c5993383b058bae0848583e6d2dbcba ../../../../components/wireless/lmac154
-86fd9436447caa1c8d0c0ae51eb649bb795d5d64 ../../../../components/wireless/macsw
-f15f28f97477c5e60e685ca33d1711ac321e1d1c ../../../../components/wireless/matter/mfd
-37990b5c2a0265f7c5f793bb97869b16d30411d4 ../../../../components/wireless/thread
-820badeede80b2f95c271b0446af4725f8edcc0a ../../../../components/wireless/wifi4/firmware
-558df9c7539a7699d34fc4b0c4d71af74ca1129a ../../../../components/wireless/wifi4/manager
-00dfd793126c49b0ea5aff23283875291aa660fe ../../../../components/wireless/wifi6
-d2b7a9e423805995bb99ecd64b44df0687bfe027 ../../../../components/wireless/wl80211
-17b4b0c515858338b8ac46ad6f8e7419480a4597 ../../../../components/wireless/wlan/linux_driver
-84ce1ab9070ece27a0599a93a56d80be7ff4ef93 ../../../../components/wireless/wlan/wlan_mac
-f1ddbc3714305080c3d4f43401be972a52bec071 ../../../../components/wireless/zigbee
-911501a52fe09e454b95c6236678e338eb138e36 ../../../../drivers/lhal
-25ffb329a3b97c5d4fc79d66c60e0b988882d996 ../../../../drivers/soc/bl602/phyrf
-1d253d273aa044ec80c3b0a0f17a7a6f0a0531e1 ../../../../drivers/soc/bl602/std
-450aef3d18fe1eb9333f833f9e3cde8ba056e3a3 ../../../../drivers/soc/bl616/phyrf
-7a68447cd346187d13b0de287e316c826eaaf6e1 ../../../../drivers/soc/bl616/std
-216bdf53aca8dce7989c51d0f3ca9d0b4642cbc3 ../../../../drivers/soc/bl616cl/phyrf
-366ff34dce5b652d7b761bd3c740cc2bfe264677 ../../../../drivers/soc/bl616cl/std
-bd10e387618e4f91d2c7ca5b643e86743917c42f ../../../../drivers/soc/bl618dg/phyrf
-2e04e7f418d047bf803468fc953fdf72058a9ac9 ../../../../drivers/soc/bl618dg/std
-712e35f8da9fc43c920cd0259a04ed9a78276d01 ../../../../drivers/soc/bl702/phyrf
-0eb29ac433b54e2c55fdf99f29cc036982f6e1d2 ../../../../drivers/soc/bl702/std
-89a6a4072e1f48b1d245deced6c0cd44f65b77fb ../../../../drivers/soc/bl702l/phyrf
-e5a4a460bfbea8036dc9638e7b371ebaed69827b ../../../../drivers/soc/bl702l/std
-1297ba7d0d257be5ffeaaced0612bc114d204fee ../../../../drivers/sys
-3a2c64f662b1c6d74302326645ad41a41f810684 ../../../../tools/autotest
-14fcd4a6c694825bb04d3dc943ab6fa2c3daaca3 ../../../../tools/bflb_tools

Validation Baseline:
- Target: BL618DG B0
- Build command: make CHIP=bl618dg BOARD=bl618dg_lpfw_board CPU_ID=ap CPU_MODEL=b0 MAKE_LPFW=y LOG_SEL=2 CONFIG_ROMAPI=y
- LPFW binary: lpfw_bin/bl618dg_lp_fw.bin
- Binary size: 48960 bytes
- SHA256: cf486b4a35f088b54494180a31029ad491981cb0a06158ff963e4a0bffd3c425
- Link memory: check build output and build/build_out/lp_fw_bl618dg_ap.map
- Sync target: tools/lpfw/bin/bl618dg_lp_fw.bin
- Consumer app: examples/pmu/wl_lp
- App build command: make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0

Board validation still needs to record boot logs, PDS entry, wakeup logs, Wi-Fi low-power RX behavior, stress result, and current measurement.
