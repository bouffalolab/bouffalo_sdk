if [ $# == 0 ] ; then
	CMD=""
fi
if [ $# == 1 ] ; then
	if [ $1 == "bl618" ] ; then
		CMD="CHIP=bl616 BOARD=bl616dk"
	fi
	if [ $1 == "bl616" ] ; then
		CMD="CHIP=bl616 BOARD=bl616dk"
	fi
fi
cd pikapython && wine rust-msc-latest-win10.exe && cd -
make -j $CMD
cp build/build_out/*.bin /mnt/d
