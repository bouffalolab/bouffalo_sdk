# # PRIV_CONFIG_GEN_BLE is and should only be defined in bouffalo.mk in the 'blecontroller_*' component
# ifeq ($(CONFIG_CHIP_NAME),BL702)
# PRIV_CONFIG_GEN_BLE ?= $(CONFIG_BLECONTROLLER_LIB)
# else ifeq ($(CONFIG_CHIP_NAME),BL602)
# PRIV_CONFIG_GEN_BLE ?= $(CONFIG_BLECONTROLLER_LIB)
# else
# PRIV_CONFIG_GEN_BLE ?= $(CONFIG_BTBLECONTROLLER_LIB)
# endif
if(NOT CONFIG_BT_CONTROLLER_DISABLE)
if("${CHIP}" STREQUAL "bl702")
	sdk_ifndef(PRIV_CONFIG_GEN_BLE ${CONFIG_BLECONTROLLER_LIB})
elseif("${CHIP}" STREQUAL "bl602")
	sdk_ifndef(PRIV_CONFIG_GEN_BLE ${CONFIG_BLECONTROLLER_LIB})
else()
	sdk_ifndef(PRIV_CONFIG_GEN_BLE ${CONFIG_BTBLECONTROLLER_LIB})
endif()
endif()

####################################################################################################################################
#Note:
#Below flags are also used by bluetooth controller, SDK user shall not change it because bluetooth controller is provided as library.
#CONFIG_BT_CONN
#CONFIG_BT_ALLROLES
#CONFIG_BT_CENTRAL
#CONFIG_BT_PERIPHERAL
#CONFIG_BT_OBSERVER
#CONFIG_BT_BROADCASTER
#CONFIG_BT_BREDR
####################################################################################################################################

macro(_check_conflict name value)
  if(DEFINED ${name} AND NOT "${${name}}" STREQUAL "${value}")
    message(FATAL_ERROR
      "BLE controller config conflict: ${name} expected '${value}', but current value is '${${name}}'")
  endif()
  set(${name} ${value})
endmacro()


# 
# CHIP_LIST := BL702 BL602
# ifeq ($(filter $(CONFIG_CHIP_NAME), $(CHIP_LIST)),$(CONFIG_CHIP_NAME))
# ifeq ($(PRIV_CONFIG_GEN_BLE),m1s1)
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BT_MESH := 0
# CONFIG_BT_ALLROLES := 1
# CONFIG_BT_CENTRAL := 1
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BLE_TP_SERVER := 1
# CONFIG_BT_OAD_SERVER := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# endif
if(("${CHIP}" STREQUAL "bl702") OR ("${CHIP}" STREQUAL "bl602"))
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m1s1")
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BT_MESH n)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_BT_CENTRAL y)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BLE_TP_SERVER y)
		_check_conflict(CONFIG_BT_OAD_SERVER y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s1)
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BT_MESH := 0
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_OBSERVER := 0
# CONFIG_BT_BROADCASTER := 0
# CONFIG_DISABLE_BT_SMP := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BT_ECC := 1
# CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_DISABLE_BLE_CONTROLLER_SEC_CON := 1
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s1")
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BT_MESH n)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_BT_BROADCASTER n)
		_check_conflict(CONFIG_DISABLE_BT_SMP y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BT_ECC y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_SEC_CON y)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s1s)
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BT_MESH := 0
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_BT_BROADCASTER := 0
# CONFIG_DISABLE_BT_SMP := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BT_ECC := 1
# CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_DISABLE_BLE_CONTROLLER_SEC_CON := 1
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s1s")
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BT_MESH n)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_BT_BROADCASTER n)
		_check_conflict(CONFIG_DISABLE_BT_SMP y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BT_ECC y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_SEC_CON y)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s1p)
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BT_MESH := 0
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_OBSERVER := 0
# CONFIG_BT_BROADCASTER := 0
# CONFIG_DISABLE_BT_SMP := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_BT_GATT_CLIENT := 0
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s1p")
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BT_MESH n)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_BT_BROADCASTER n)
		_check_conflict(CONFIG_DISABLE_BT_SMP y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_BT_GATT_CLIENT n)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s0sp)
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 0
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BT_MESH := 0
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 0
# CONFIG_BT_OBSERVER := 1
# CONFIG_BT_BROADCASTER := 0
# CONFIG_DISABLE_BT_SMP := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s0sp")
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 0)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BT_MESH n)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL n)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_BT_BROADCASTER n)
		_check_conflict(CONFIG_DISABLE_BT_SMP y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m8s1)
# CONFIG_BT_CONN := 8
# CONFIG_BLE_TX_BUFF_DATA := 8
# CONFIG_BT_ALLROLES := 1
# CONFIG_BT_CENTRAL := 1
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m8s1")
		_check_conflict(CONFIG_BT_CONN 8)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 8)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_BT_CENTRAL y)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_PHY_UPDATE y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m16s1)
# CONFIG_BT_CONN := 16
# CONFIG_BLE_TX_BUFF_DATA := 16
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 1
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_BT_BROADCASTER := 0
# CONFIG_BLE_STACK_DBG_PRINT := 0
# CONFIG_DISABLE_BT_SMP := 1
# CONFIG_SIMPLE_MASTER := 1
# CONFIG_EM_16K := 1
# endif
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m16s1")
		_check_conflict(CONFIG_BT_CONN 16)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 16)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL y)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_BT_BROADCASTER n)
		_check_conflict(CONFIG_BLE_STACK_DBG_PRINT n)
		_check_conflict(CONFIG_DISABLE_BT_SMP y)
		_check_conflict(CONFIG_SIMPLE_MASTER y)
		_check_conflict(CONFIG_EM_16K y)
	endif()
endif()
# 
# 
# ifeq ($(CONFIG_CHIP_NAME),BL702L)
# CONFIG_BT := 0
# CONFIG_CIS := 0
# CONFIG_ADV_EXTENSION := 0
# CONFIG_BIS := 0
# CONFIG_LE_PWR_CTRL := 0
# CONFIG_CTE := 0
# CONFIG_EM_SIZE ?= 16
if("${CHIP}" STREQUAL "bl702l")
	_check_conflict(CONFIG_BT n)
	_check_conflict(CONFIG_CIS n)
	_check_conflict(CONFIG_ADV_EXTENSION n)
	_check_conflict(CONFIG_BIS n)
	_check_conflict(CONFIG_LE_PWR_CTRL n)
	_check_conflict(CONFIG_CTE n)
	_check_conflict(CONFIG_EM_SIZE 16)
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s1rp)
# CONFIG_BL702L_A1 := 1
# CONFIG_BUILD_BLE_ROM_CODE := 0
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 5
# #CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_OBSERVER := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 0
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s1rp")
		_check_conflict(CONFIG_BL702L_A1 y)
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE n)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 5)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER n)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m1s1p)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m1s1p")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m2s1p)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 2
# CONFIG_BLE_TX_BUFF_DATA := 4
# CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m2s1p")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 2)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 4)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m4s1p)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 4
# CONFIG_BLE_TX_BUFF_DATA := 6
# CONFIG_BLE_ACT_MAX := 6
# CONFIG_BT_ALLROLES := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m4s1p")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 4)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 6)
		_check_conflict(CONFIG_BLE_ACT_MAX 6)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m8s1p)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 8
# CONFIG_BLE_TX_BUFF_DATA := 8
# CONFIG_BLE_ACT_MAX := 10
# CONFIG_BT_ALLROLES := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m8s1p")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 8)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 8)
		_check_conflict(CONFIG_BLE_ACT_MAX 10)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s1p)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 0
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s1p")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY  y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG  n)
		_check_conflict(CONFIG_CLK_ACC  n)
		_check_conflict(CONFIG_LE_PING  n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s1sp)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s1sp")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY  y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG  n)
		_check_conflict(CONFIG_CLK_ACC  n)
		_check_conflict(CONFIG_LE_PING  n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s2p)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 2
# CONFIG_BLE_TX_BUFF_DATA := 4
# CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 0
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
# 
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s2p")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 2)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 4)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
#
# ifeq ($(PRIV_CONFIG_GEN_BLE),m0s4p)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 4
# CONFIG_BLE_TX_BUFF_DATA := 6
# CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 0
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
# endif
#
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "m0s4p")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 4)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 6)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
#
#ifeq ($(PRIV_CONFIG_GEN_BLE),uarthci)
#CONFIG_BUILD_BLE_ROM_CODE := 1
#CONFIG_BLE_PDS := 1
#CONFIG_BT_CONN := 1
#CONFIG_BLE_TX_BUFF_DATA := 2
#CONFIG_BLE_ACT_MAX := 5
#CONFIG_BT_ALLROLES := 1
#CONFIG_DISABLE_BT_HOST_PRIVACY := 1
#CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
#CONFIG_BLE_HOST_DISABLE:=1
#CONFIG_EM_SIZE = 8
#CONFIG_LONG_RANG := 0
#CONFIG_CLK_ACC := 0
#CONFIG_LE_PING := 0
#endif
#
 	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "uarthci")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_BLE_HOST_DISABLE y)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
#
# ifeq ($(PRIV_CONFIG_GEN_BLE),uarthcilp)
# CONFIG_BUILD_BLE_ROM_CODE := 1
# CONFIG_LP_HCIUART := 1
# CONFIG_BLE_PDS := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# CONFIG_BLE_ACT_MAX := 5
# CONFIG_BT_ALLROLES := 1
# CONFIG_DISABLE_BT_HOST_PRIVACY := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_BLE_HOST_DISABLE:=1
# CONFIG_EM_SIZE = 8
# CONFIG_LONG_RANG := 0
# CONFIG_CLK_ACC := 0
# CONFIG_LE_PING := 0
#endif
#
 	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "uarthcilp")
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
		_check_conflict(CONFIG_LP_HCIUART y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 5)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BT_HOST_PRIVACY y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_BLE_HOST_DISABLE y)
		_check_conflict(CONFIG_EM_SIZE 8)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
	endif()
#	
# endif
endif()
# 
# ifeq ($(CONFIG_CHIP_NAME),BL616) //bl808 add for bouffalosdk
# ifeq ($(PRIV_CONFIG_GEN_BLE),ble1m0s1bredr0)
# CONFIG_BT_BREDR := 0
# CONFIG_BLE_PDS := 1
# CONFIG_EM_SIZE := 32
# CONFIG_EM_HEAP_DISABLE := 1
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 0
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# #Update le adv data and scan rsp data when LE adertising is active, sw need 4 adv buffer at least. 
# CONFIG_BLE_ACT_MAX := 7
# CONFIG_DISABLE_BT_ECC := 1
# CONFIG_ADV_EXTENSION := 0
# CONFIG_CIS := 0
# CONFIG_BIS := 0
# CONFIG_CTE := 0
# CONFIG_LE_PWR_CTRL := 0
# endif
if(("${CHIP}" STREQUAL "bl616") OR ("${CHIP}" STREQUAL "bl808") OR ("${CHIP}" STREQUAL "bl616d"))
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "ble1m0s1bredr0")
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_EM_HEAP_DISABLE y)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_DISABLE_BT_ECC y)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),ble1m2s1bredr0)
# CONFIG_BT_BREDR := 0
# CONFIG_BLE_PDS := 1
# CONFIG_EM_SIZE := 32
# CONFIG_BT_ALLROLES := 1
# CONFIG_BT_CENTRAL := 1
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_BT_CONN := 2
# CONFIG_BLE_TX_BUFF_DATA := 4
# #Update le adv data and scan rsp data when LE adertising is active, sw need 4 adv buffer at least. 
# CONFIG_BLE_ACT_MAX := 7
# CONFIG_ADV_EXTENSION := 0
# CONFIG_CIS := 0
# CONFIG_BIS := 0
# CONFIG_CTE := 0
# CONFIG_LE_PWR_CTRL := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "ble1m2s1bredr0")
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_BT_CENTRAL y)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_BT_CONN 2)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 4)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),ble1m10s1bredr0)
# CONFIG_BT_BREDR := 0
# CONFIG_BLE_PDS := 1
# CONFIG_EM_SIZE=32
# CONFIG_BT_ALLROLES := 1
# CONFIG_BT_CENTRAL := 1
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_BT_CONN := 10
# CONFIG_BLE_TX_BUFF_DATA := 10
# CONFIG_BLE_ACT_MAX := 11
# CONFIG_ADV_EXTENSION := 0
# CONFIG_CIS := 0
# CONFIG_BIS := 0
# CONFIG_CTE := 0
# CONFIG_LE_PWR_CTRL := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "ble1m10s1bredr0")
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_BT_CENTRAL y)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_BT_CONN 10)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 10)
		_check_conflict(CONFIG_BLE_ACT_MAX 12)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),ble1m0s1sbredr0)
# CONFIG_BT_BREDR := 0
# CONFIG_BLE_PDS := 1
# CONFIG_EM_SIZE=32
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# #Update le adv data and scan rsp data when LE adertising is active, sw need 4 adv buffer at least. 
# CONFIG_BLE_ACT_MAX := 7
# CONFIG_ADV_EXTENSION := 0
# CONFIG_CIS := 0
# CONFIG_BIS := 0
# CONFIG_CTE := 0
# CONFIG_LE_PWR_CTRL := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "ble1m0s1sbredr0")
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
	endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),ble1m0s1sbredr1)
# CONFIG_BT_BREDR := 1
# CONFIG_EM_SIZE=32
# CONFIG_BT_ALLROLES := 0
# CONFIG_BT_CENTRAL := 0
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_BT_CONN := 1
# CONFIG_BLE_TX_BUFF_DATA := 2
# #Update le adv data and scan rsp data when LE adertising is active, sw need 4 adv buffer at least. 
# CONFIG_BLE_ACT_MAX := 7
# CONFIG_ADV_EXTENSION := 0
# CONFIG_CIS := 0
# CONFIG_BIS := 0
# CONFIG_CTE := 0
# CONFIG_LE_PWR_CTRL := 0
# endif
# endif
        if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "ble1m0s1sbredr1")
		_check_conflict(CONFIG_BT_BREDR y)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_BT_CONN 1)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 2)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
        endif()
# 
# ifeq ($(PRIV_CONFIG_GEN_BLE),ble1m2s1bredr1)
# CONFIG_BT_BREDR := 1
# CONFIG_BLE_PDS := 1
# CONFIG_EM_SIZE := 32
# CONFIG_BT_ALLROLES := 1
# CONFIG_BT_CENTRAL := 1
# CONFIG_BT_PERIPHERAL := 1
# CONFIG_BT_BROADCASTER := 1
# CONFIG_BT_OBSERVER := 1
# CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 1
# CONFIG_BT_CONN := 2
# CONFIG_BLE_TX_BUFF_DATA := 4
# #Update le adv data and scan rsp data when LE adertising is active, sw need 4 adv buffer at least. 
# CONFIG_BLE_ACT_MAX := 7
# CONFIG_ADV_EXTENSION := 0
# CONFIG_CIS := 0
# CONFIG_BIS := 0
# CONFIG_CTE := 0
# CONFIG_LE_PWR_CTRL := 0
# endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "ble1m2s1bredr1")
		_check_conflict(CONFIG_BT_BREDR y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_BT_CENTRAL y)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_BT_OBSERVER y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE y)
		_check_conflict(CONFIG_BT_CONN 2)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 4)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
	endif()
#
#ifeq ($(PRIV_CONFIG_GEN_BLE),uarthci)
#CONFIG_BT_BREDR := 1
#CONFIG_BLE_HOST_DISABLE:= 1
#CONFIG_EM_SIZE := 64
#CONFIG_BT_ALLROLES := 1
#CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
#CONFIG_BT_CONN := 2
#CONFIG_BLE_TX_BUFF_DATA := 4
#CONFIG_BLE_ACT_MAX := 7
#CONFIG_CIS := 0
#CONFIG_BIS := 0
#CONFIG_CTE := 0
#endif
#
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "uarthci")
		_check_conflict(CONFIG_BT_BREDR y)
		_check_conflict(CONFIG_BLE_HOST_DISABLE y)
		_check_conflict(CONFIG_EM_SIZE 64)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_BT_CONN 2)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 4)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_BIS y)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_CIS y)
	endif()
#
#ifeq ($(PRIV_CONFIG_GEN_BLE),bleuarthci)
#CONFIG_BT_BREDR := 0
#CONFIG_BLE_HOST_DISABLE:= 1
#CONFIG_EM_SIZE := 64
#CONFIG_BT_ALLROLES := 1
#CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
#CONFIG_BT_CONN := 2
#CONFIG_BLE_TX_BUFF_DATA := 4
#CONFIG_BLE_ACT_MAX := 7
#CONFIG_CIS := 1
#CONFIG_BIS := 1
#CONFIG_CTE := 0
#endif
#
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "bleuarthci")
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BLE_HOST_DISABLE y)
		_check_conflict(CONFIG_EM_SIZE 64)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_BT_CONN 2)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 4)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_BIS y)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_CIS y)
	endif()
#
#ifeq ($(PRIV_CONFIG_GEN_BLE),blespihci)
#CONFIG_NXSPI_HCI :=1
#CONFIG_BT_BREDR := 0
#CONFIG_BLE_HOST_DISABLE:= 1
#CONFIG_EM_SIZE := 32
#CONFIG_BT_ALLROLES := 1
#CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE := 0
#CONFIG_BT_CONN := 2
#CONFIG_BLE_TX_BUFF_DATA := 4
#CONFIG_BLE_ACT_MAX := 7
#CONFIG_CIS := 1
#CONFIG_BIS := 1
#CONFIG_CTE := 0
#endif
#
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "blespihci")
		_check_conflict(CONFIG_NXSPI_HCI y)
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BLE_HOST_DISABLE y)
		_check_conflict(CONFIG_BLE_PDS y)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_BT_ALLROLES y)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_BT_CONN 2)
		_check_conflict(CONFIG_BLE_TX_BUFF_DATA 4)
		_check_conflict(CONFIG_BLE_ACT_MAX 7)
		_check_conflict(CONFIG_BIS y)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_CIS y)
	endif()
#
#ifeq ($(PRIV_CONFIG_GEN_BLE),mfg)
#CONFIG_EM_SIZE := 32
#CONFIG_BLE_MFG := 1
#CONFIG_BT_MFG := 1
#CONFIG_BLE_MFG_HCI_CMD := 1
#CONFIG_BT_MFG_HCI_CMD := 1
#endif
	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "mfg")
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_BLE_MFG y)
		_check_conflict(CONFIG_BT_MFG y)
		_check_conflict(CONFIG_BLE_MFG_HCI_CMD y)
		_check_conflict(CONFIG_BT_MFG_HCI_CMD y)
	endif()

	if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "wlanif")
		_check_conflict(CONFIG_FREERTOS_DISABLE y)
		_check_conflict(CONFIG_BLE_HOST_DISABLE y)
		_check_conflict(CONFIG_BLE_PDS n)
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_SEC_CON 1)
		_check_conflict(CONFIG_DISABLE_CONTROLLER_BLE_PRIVACY n)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_PHY_UPDATE y)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 32)
		_check_conflict(CONFIG_DBG_RUN_ON_FPGA n)
		_check_conflict(CONFIG_BLE_RES_DYNAMIC_CONF n)
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE y)
	endif()
endif()

if(("${CHIP}" STREQUAL "bl616l"))
    if("${PRIV_CONFIG_GEN_BLE}" STREQUAL "wlanif")
		_check_conflict(CONFIG_FREERTOS_DISABLE y)
		_check_conflict(CONFIG_BLE_HOST_DISABLE y)
		_check_conflict(CONFIG_BLE_PDS n)
		_check_conflict(CONFIG_BT_BREDR n)
		_check_conflict(CONFIG_BT_ALLROLES n)
		_check_conflict(CONFIG_BT_CENTRAL n)
		_check_conflict(CONFIG_BT_PERIPHERAL y)
		_check_conflict(CONFIG_BT_BROADCASTER y)
		_check_conflict(CONFIG_LONG_RANG n)
		_check_conflict(CONFIG_CLK_ACC n)
		_check_conflict(CONFIG_LE_PING n)
		_check_conflict(CONFIG_LE_PWR_CTRL n)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_SEC_CON 1)
		_check_conflict(CONFIG_DISABLE_CONTROLLER_BLE_PRIVACY n)
		_check_conflict(CONFIG_BT_OBSERVER n)
		_check_conflict(CONFIG_ADV_EXTENSION n)
		_check_conflict(CONFIG_PHY_UPDATE n)
		_check_conflict(CONFIG_CIS n)
		_check_conflict(CONFIG_BIS n)
		_check_conflict(CONFIG_CTE n)
		_check_conflict(CONFIG_DISABLE_BLE_CONTROLLER_TEST_MODE n)
		_check_conflict(CONFIG_EM_SIZE 16)
		_check_conflict(CONFIG_DBG_RUN_ON_FPGA n)
		_check_conflict(CONFIG_BLE_RES_DYNAMIC_CONF y)
		_check_conflict(CONFIG_BUILD_BLE_ROM_CODE n)
	endif()
endif()