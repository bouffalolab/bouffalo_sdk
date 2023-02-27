cd pikapython && rust-msc-latest-win10.exe && cd ..
set PATH=%PATH%;%~dp0..\..\tools\make
set PATH=%PATH%;%~dp0..\..\tools\ninja
set PATH=%PATH%;%~dp0..\..\tools\toolchain_gcc_t-head_windows\bin
IF NOT EXIST %~dp0..\..\tools\toolchain_gcc_t-head_windows (
    git clone https://gitee.com/bouffalolab/toolchain_gcc_t-head_windows %~dp0..\..\tools\toolchain_gcc_t-head_windows
    )
make ninja CHIP=bl616 BOARD=bl616dk