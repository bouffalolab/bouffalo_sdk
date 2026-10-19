
phy_top_dirs := plf/refip/src/driver/phy
phyif_top_dirs := plf/refip/src/driver/phyif

phy_inc_dirs := $(phy_top_dirs) \
				$(phy_top_dirs)/bl602_phy_rf/include \
				$(phy_top_dirs)/bl602_phy_rf/include/reg \
				$(phyif_top_dirs) 

rf_inc_dirs  := $(phy_top_dirs)/bl602_phy_rf/rf/Inc \

phy_src_dirs := $(phy_top_dirs) \
				$(phyif_top_dirs) 

plf_phy	:= $(phy_top_dirs)/rf.c \
		   $(phyif_top_dirs)/phyif_utils.c \

phy_ldflags := -L$(COMPONENT_PATH)/$(phy_top_dirs)/bl602_phy_rf/lib-gcc-10.2.0-rv32imfc-ilp32f -lbl602_phyrf
