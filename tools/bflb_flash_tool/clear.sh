find . -name "*.ini" | xargs rm -f
find . -type f | grep ".*\.log$" | xargs rm -f
find ./chips -type f | grep ".*\.pack$" | xargs rm -f
find . -name iot.toml | xargs rm -f
find . -name iot_linux.toml | xargs rm -f
find . -name mcu.toml | xargs rm -f
find . -name storage.toml | xargs rm -f
find . -name partition.bin | xargs rm -f
find . -name ro_params.dtb | xargs rm -f
find . -name efusedata.bin | xargs rm -f
find . -name efusedata_mask.bin | xargs rm -f
find . -name md_test_privatekey_ecc.pem | xargs rm -f
find . -name md_test_publickey_ecc.pem | xargs rm -f
rm -rf ./chips/bl56x/ota
rm -rf ./chips/bl60x/ota
rm -rf ./chips/bl562/ota
rm -rf ./chips/bl602/ota
rm -rf ./chips/bl702/ota
rm -rf ./chips/bl808/ota
rm -rf ./chips/bl606p/ota
rm -rf ./chips/bl602u/ota
rm -rf ./chips/bl808/ota
rm -rf ./chips/bl616/ota
rm -rf ./chips/wb03/ota
rm -rf ./chips/bl606p/test_bin
rm -rf ./chips/bl808/test_bin
find ./chips/bl56x/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl60x/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl562/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl602/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl702/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl602u/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl808/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl606p/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl616/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/wb03/img_create_iot -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl56x/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl60x/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl562/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl602/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl702/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl602u/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl808/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl606p/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl616/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/wb03/img_create_mcu -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl56x/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl60x/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl562/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl602/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl702/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl602u/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl808/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl606p/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/bl616/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
find ./chips/wb03/efuse_bootheader -type f | grep ".*\.bin$" | xargs rm -f
