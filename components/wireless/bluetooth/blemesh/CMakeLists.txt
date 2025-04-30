# include $(COMPONENT_PATH)/../ble_common.mk
# Add by bouffalo sdk
sdk_generate_library()
# 
# # Component Makefile
# #
# ## These include paths would be exported to project level
# COMPONENT_ADD_INCLUDEDIRS    += src \
#                                 src/include  \
#                                 src/mesh_cli_cmds  \
#                                 src/mem_slab					
sdk_add_include_directories(
	./
	src
	src/include
	src/mesh_cli_cmds
	src/mem_slab
)
# 
#ifeq ($(CONFIG_BT_MESH_MODEL), 0)
#COMPONENT_ADD_INCLUDEDIRS    += src/mesh_models/include  \
#                                src/mesh_models/server/include
#endif
if(NOT CONFIG_BT_MESH_MODEL)
	sdk_add_include_directories(
	src/mesh_models/include
	src/mesh_models/server/include
)
endif()
# 
# ## not be exported to project level
# COMPONENT_PRIV_INCLUDEDIRS   :=
# 
# ## This component's src 
# COMPONENT_SRCS   := src/access.c \
# 					src/adv.c \
# 					src/beacon.c \
# 					src/cfg_cli.c \
# 					src/cfg_srv.c \
# 					src/crypto.c \
# 					src/health_cli.c \
# 					src/health_srv.c \
# 					src/main.c \
# 					src/net.c \
# 					src/prov.c \
# 					src/proxy.c \
# 					src/settings.c \
# 					src/transport.c \
# 					src/local_operation.c \
# 					src/mem_slab/slab.c
sdk_library_add_sources(
	src/access.c
	src/adv.c
	src/beacon.c
	src/cfg_cli.c
	src/cfg_srv.c
	src/crypto.c 
	src/health_cli.c
	src/health_srv.c
	src/main.c
	src/net.c
	src/prov.c
	src/proxy.c
	src/settings.c
	src/transport.c
	src/local_operation.c
	src/mem_slab/slab.c
)
# 
# ifneq ($(CONFIG_AUTO_PTS),1)
# COMPONENT_SRCS += src/mesh_cli_cmds/mesh_cli_cmds.c
# endif
if(NOT CONFIG_AUTO_PTS AND CONFIG_SHELL)
    sdk_library_add_sources(src/mesh_cli_cmds/mesh_cli_cmds.c)
endif()
# 
# ifeq ($(CONFIG_BT_MESH_FRIEND),1)
# COMPONENT_SRCS   += src/friend.c
# endif
sdk_library_add_sources_ifdef(CONFIG_BT_MESH_FRIEND src/friend.c)
# 
# ifeq ($(CONFIG_BT_MESH_LOW_POWER),1)
# COMPONENT_SRCS   += src/lpn.c
# endif
sdk_library_add_sources_ifdef(CONFIG_BT_MESH_LOW_POWER src/lpn.c)
# 
# ifeq ($(CONFIG_BT_MESH_PB_ADV),1)
# COMPONENT_SRCS   += src/pb_adv.c
# endif
sdk_library_add_sources_ifdef(CONFIG_BT_MESH_PB_ADV src/pb_adv.c)
# 
# ifeq ($(CONFIG_BT_MESH_PB_GATT),1)
# COMPONENT_SRCS   += src/pb_gatt.c
# endif
sdk_library_add_sources_ifdef(CONFIG_BT_MESH_PB_GATT src/pb_gatt.c)
# 
# ifeq ($(CONFIG_BT_MESH_CDB),1)
# COMPONENT_SRCS   += src/cdb.c
# endif
sdk_library_add_sources_ifdef(CONFIG_BT_MESH_CDB src/cdb.c)
# 
# ifeq ($(CONFIG_BT_MESH_MODEL), 0)
# ifeq ($(CONFIG_BT_MESH_MODEL_GEN_SRV),1)
# COMPONENT_SRCS   += src/mesh_models/server/common_srv.c \
#                     src/mesh_models/server/gen_srv.c 
# endif
# endif
if(NOT CONFIG_BT_MESH_MODEL AND CONFIG_BT_MESH_MODEL_GEN_SRV)
sdk_library_add_sources(
	src/mesh_models/server/common_srv.c
	src/mesh_models/server/gen_srv.c
)
endif()
# 
# COMPONENT_OBJS   := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
# COMPONENT_SRCDIRS:= src \
#                     src/mesh_cli_cmds \
# 					src/mem_slab \
# 
# ifeq ($(CONFIG_BT_MESH_MODEL), 0)
# COMPONENT_SRCDIRS+= src/mesh_models/server
# endif