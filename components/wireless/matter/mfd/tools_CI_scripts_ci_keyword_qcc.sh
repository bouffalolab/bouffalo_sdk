#!/usr/bin/env bash

echo -e "\033[31mAlmost need 1 minute, start at `date`\033[0m"

# delete files/folders for BL702/BL702L
find components/wireless/matter/mfd -name *bl702* -o -name *bl602* -o -name bouffalo.mk | xargs -I {} rm {} -rf
rm components/wireless/matter/mfd/bl_mfd_decrypt.c -f
rm examples/wifi/sta/matter/lwipopts_user.h

# ChangeLog.md file modify
sed -i '/BL702/d' components/wireless/matter/mfd/ChangeLog.md
sed -i '/BL602/d' components/wireless/matter/mfd/ChangeLog.md
sed -i 's/BL616/QCC743/g' components/wireless/matter/mfd/ChangeLog.md

# file rename
find components/wireless/matter/mfd -type f \
| while read x; do
rename 's/((?<=[\/_])|(?<=lib))bl616(?=[^\/]*$)/qcc743/g;
        s/((?<=[\/_])|(?<=lib))bl_(?=[^\/]*$)/qcc_/g;' $x;
done
echo "mfd file renamed at `date`"

echo "sdk_generate_library()
sdk_add_include_directories(./include)
sdk_add_private_compile_definitions(-DBL_MFD_PLAT_H=\"qcc_mfd_qcc743.h\")
sdk_library_add_sources(qcc_mfd.c qcc_mfd_decrypt_qcc743.c)" > components/wireless/matter/mfd/CMakeLists.txt

# code file modify
sed -i 's/bl_mfd\.h/qcc_mfd\.h/g' components/wireless/matter/mfd/qcc_mfd.c
find components/wireless/matter/mfd examples/wifi/sta/matter -type f \! \
\( -name "*.bin" -o -name "*.a" -o -name "*.apk" -o -name "tydb_linux" -o -name "*.exe" -o -name "*.svg" -o -name "*.pem" -o -name "*.gif" \) \
| while read x; do
perl -i -pe 's/((?<=[^a-zA-Z])|(?<=lib)|^)bl616/qcc743/g;
             s/((?<=[^a-zA-Z])|(?<=-D)|^)BL616\/BL618/qcc743\/qcc744/g;
             s/((?<=[^a-zA-Z])|(?<=-D)|^)BL616/QCC743/g;
             s/\/(602|606p|628|702|702l|808)/\/qcc74x_undef/g;
             s/\/(616)/\/qcc743/g;
             s/BL616_/QCC743_/g;
             s/((?<=[^a-zA-Z])|(?<=-D)|^)BL([0-9][0-9][0-9xX])/QCC74x_undef/g;
             s/((?<=[^a-zA-Z])|^)bl([0-9][0-9][0-9xX])/qcc74x_undef/g;
             s/((?<=[^a-zA-Z])|(?<=-D)|^)WB03/QCC74x_undef/g;
             s/((?<=[^a-zA-Z])|^)wb03/qcc74x_undef/g;
             s/((?<=[^a-zA-Z])|^)bl_/qcc74x_/g;
             s/((?<=[^a-zA-Z])|(?<=-D)|^)BL_(?!MAX)/QCC74x_/g;
             s/((?<=[^a-zA-Z])|(?<=lib)|^)bl6_/qcc74x_/g;
             s/bflb(?=[_ .])/qcc74x/g;
             s/BFLB(?=[_ ])/QCC74x/g;
             s/bfl(?=[_ ])/qcc74x/g;
             s/BFL(?=[_ ])/QCC74x/g;
             s/bouffalolab/qcc74x/g;
             s/bouffalo/qcc74x/g;
             s/bflog/qcc74xlog/g;
             s/BFLOG/QCC74xLOG/g;
             s/blsp/qcc74xsp/g;
             s/BLSP/QCC74xSP/g;
             s/blsync/qcc74xsync/g;
             s/BLSYNC/QCC74xSYNC/g;
             s/blfb_/qcc74x_/g;
             s/BLFB_/QCC74x_/g;
             s/blog\.h/qcc74x_log\.h/g;
             s/blverinf/qcc74xverinf/g;
             s/Bouffalo Lab|Bouffalolab|BouffaloLab/qcc74x/g;
             s/Bouffalo|BOUFFALO/qcc74x/g;
             s/_BL6_/_QCC74x_/g' $x;
done
echo "mfd code file modified `date`"

rm $0