#!/bin/bash

readonly TARGET="$1"

# Known good version for PR testing
ZEPHYR_SDK_VERSION=v0.17.0
ZEPHYR_VERSION=v4.1.0

ZEPHYR_TOOLCHAIN_VARIANT=zephyr
ZEPHYR_SDK_API_FOLDER=https://api.github.com/repos/zephyrproject-rtos/sdk-ng/releases
ZEPHYR_SDK_VER_SELECT="tags/$ZEPHYR_SDK_VERSION"
ZEPHYR_SDK_SETUP_TAR=zephyr-sdk-.*linux-x86_64.tar.xz

FREERTOS_ZIP_URL=https://sourceforge.net/projects/freertos/files/FreeRTOS/V10.0.1/FreeRTOSv10.0.1.zip

pre_build(){
	# fix issue related to tzdata install, not needed for 24.04 but kept for 22.04 and earlier
	echo 'Etc/UTC' > /etc/timezone || exit 1
	ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime || exit 1

	#Create a new virtual environment
	python3 -m venv ./.venv
	source ./.venv/bin/activate

	# add make and cmake
	# cmake from packages will work for 22.04 and later but use pip3 to get the latest on
	# any distro
	apt update || exit 1
   	apt-get install -y make curl || exit 1
	pip3 install cmake || exit 1
}

build_linux(){
	echo  " Build for linux"
	apt-get install -y libsysfs-dev libhugetlbfs-dev gcc &&
	mkdir -p build-linux &&
	cd build-linux &&
	cmake .. -DWITH_TESTS_EXEC=on &&
	make VERBOSE=1 all test &&
	exit 0
}

build_generic(){
	echo  " Build for generic platform "
	apt-get install -y gcc-arm-none-eabi &&
	mkdir -p build-generic &&
	cd build-generic &&
	cmake .. -DCMAKE_TOOLCHAIN_FILE=template-generic || exit 1
	make VERBOSE=1 || exit 1
	exit 0
}

build_freertos(){
	echo  " Build for freertos OS "
      	apt-get install -y gcc-arm-none-eabi unzip &&
      	wget $FREERTOS_ZIP_URL --dot-style=giga > /dev/null &&
      	unzip FreeRTOSv10.0.1.zip > /dev/null &&
	mkdir -p build-freertos &&
	cd build-freertos && export &&
	cmake .. -DCMAKE_TOOLCHAIN_FILE=template-freertos \
		-DCMAKE_C_FLAGS="-I$PWD/../FreeRTOSv10.0.1/FreeRTOS/Source/include/ \
		-I$PWD/../FreeRTOSv10.0.1/FreeRTOS/Demo/CORTEX_STM32F107_GCC_Rowley \
		-I$PWD/../FreeRTOSv10.0.1/FreeRTOS/Source/portable/GCC/ARM_CM3" || exit 1
	make VERBOSE=1 || exit 1
	exit 0
}

build_zephyr(){
	ZEPHYR_SDK_DOWNLOAD_URL=`curl -s ${ZEPHYR_SDK_API_FOLDER}/${ZEPHYR_SDK_VER_SELECT} | \
		grep -e "browser_download_url.*${ZEPHYR_SDK_SETUP_TAR}"| cut -d : -f 2,3 | tr -d \"`
	ZEPHYR_SDK_TAR=`basename  $ZEPHYR_SDK_DOWNLOAD_URL`
	ZEPHYR_SDK_SETUP_DIR=`echo $ZEPHYR_SDK_TAR | cut -d_ -f1`
	echo  " Build for Zephyr OS "
	sudo apt-get install -y git cmake ninja-build gperf || exit 1
	sudo apt-get install -y ccache dfu-util device-tree-compiler wget pv || exit 1
	sudo apt-get install -y python3-dev python3-setuptools python3-tk python3-wheel xz-utils \
		file || exit 1
	sudo apt-get install -y make gcc gcc-multilib g++-multilib libsdl2-dev || exit 1
	sudo apt-get install -y libc6-dev-i386 gperf g++ python3-ply python3-yaml \
		device-tree-compiler ncurses-dev uglifyjs -qq || exit 1
	pip3 install pyelftools || exit 1
	pip3 install west || exit 1


	wget $ZEPHYR_SDK_DOWNLOAD_URL --dot-style=giga || exit 1
	echo "Extracting $ZEPHYR_SDK_TAR"
	pv $ZEPHYR_SDK_TAR -i 3 -ptebr -f | tar xJ || exit 1
	rm -rf $ZEPHYR_SDK_TAR || exit 1
	yes | ./$ZEPHYR_SDK_SETUP_DIR/setup.sh || exit 1
	west init --mr $ZEPHYR_VERSION ./zephyrproject || exit 1
	cd ./zephyrproject || exit 1
	west update --narrow || exit 1
	west zephyr-export || exit 1
	pip3 install -r ./zephyr/scripts/requirements.txt || exit 1

	cd ./zephyr &&
	source zephyr-env.sh &&
	cd ../.. &&
	# The prj.conf is mandatory for cmake execution, create a file.
	# The base level of the test framework intentionally uses constructors
	#   so we must define this config item in v3.7.0 or later
	#   HOWEVER, this define will cause cmake to fail for v3.6 or earlier
	echo "CONFIG_STATIC_INIT_GNU=y" >prj.conf &&
	# Add dummy fields in the VERSION file to fix compatibility with Zephyr
	# version.cmake file
 	echo "PATCHLEVEL = 0" >>VERSION &&
 	echo "VERSION_TWEAK = 0" >>VERSION &&
	echo  "###### Build for qemu_cortex_m3 ######" &&
	cmake . -DWITH_ZEPHYR=on -DBOARD=qemu_cortex_m3 -DWITH_TESTS=on -Bbuild-zephyr-m3 || exit 1
	cd build-zephyr-m3  || exit 1
	make VERBOSE=1 || exit 1
	cd .. &&
	echo  "###### Build for qemu_cortex_a53 ######" &&
	cmake . -DWITH_ZEPHYR=on -DBOARD=qemu_cortex_a53 -DWITH_TESTS=on -Bbuild-zephyr-a53 \
		|| exit 1
	cd build-zephyr-a53 &&
	make VERBOSE=1  || exit 1
	cd .. &&
	echo  "###### Build for qemu_xtensa ######" &&
	cmake . -DWITH_ZEPHYR=on -DBOARD=qemu_xtensa -DWITH_TESTS=on -Bbuild-zephyr-xtensa &&
	cd build-zephyr-xtensa &&
	cd .. &&
	echo  "###### Build for qemu_riscv64 ######" &&
	cmake . -DWITH_ZEPHYR=on -DBOARD=qemu_riscv64 -DWITH_TESTS=on -Bbuild-zephyr-rscv64 \
		|| exit 1
	cd build-zephyr-rscv64 &&
	make VERBOSE=1  || exit 1
	cd .. &&
	echo  "###### Build for qemu_riscv32 ######" &&
	cmake . -DWITH_ZEPHYR=on -DBOARD=qemu_riscv32 -DWITH_TESTS=on -Bbuild-zephyr-rscv32 \
		|| exit 1
	cd build-zephyr-rscv32 &&
	make VERBOSE=1  || exit 1
	exit 0
}

main(){
	pre_build;

	if [[ "$TARGET" == "linux" ]]; then
		build_linux
	fi
	if [[ "$TARGET" == "generic" ]]; then
		build_generic
	fi
	if [[ "$TARGET" == "freertos" ]]; then
		build_freertos
	fi
	if [[ "$TARGET" == "zephyr" ]]; then
		build_zephyr
	fi
	if [[ "$TARGET" == "zephyr-latest" ]]; then
		ZEPHYR_SDK_VER_SELECT=latest
		ZEPHYR_VERSION=main
		build_zephyr
	fi
}

main
