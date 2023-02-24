
makefile_files += $(shell find ./peripherals -name 'Makefile' -not -path "*/build/*")
makefile_dirs = $(sort $(dir $(makefile_files)))

ifeq ($(CHIP),bl602)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/emac/%
filter_out_dirs += ./peripherals/pwm_v2/%
filter_out_dirs += ./peripherals/iso11898/%
filter_out_dirs += ./peripherals/usbdev/%

endif

ifeq ($(CHIP),bl702)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/emac/%
filter_out_dirs += ./peripherals/pwm_v2/%
filter_out_dirs += ./peripherals/iso11898/%
endif

ifeq ($(CHIP),bl702L)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/emac/%
filter_out_dirs += ./peripherals/pwm_v2/%
filter_out_dirs += ./peripherals/iso11898/%
filter_out_dirs += ./peripherals/usbdev/%
endif

ifeq ($(CHIP),bl616)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/pwm_v1/%
endif

ifeq ($(CHIP),bl808)
filter_out_dirs += ./peripherals/pwm_v1/%
endif

build_dirs = $(filter-out $(filter_out_dirs), $(makefile_dirs))

listall:
	@for n in $(build_dirs); do echo $$n ; done

buildall:
	@for n in $(build_dirs); do make -C $$n || break ; done

cleanall:
	@for n in $(build_dirs); do make -C $$n clean; done

.PHONY: buildall cleanall