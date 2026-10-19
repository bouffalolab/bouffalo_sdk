############################################################################
# vendor/bouffalolab/chip/bl616/wl80211/Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

# Include header paths
WL80211_DIR := chip/wl80211

# Add wl80211 include paths from CMakeLists.txt
INCLUDES += ${INCDIR_PREFIX}$(ARCH_SRCDIR)$(DELIM)$(WL80211_DIR)
INCLUDES += ${INCDIR_PREFIX}$(ARCH_SRCDIR)$(DELIM)$(WL80211_DIR)/include
INCLUDES += ${INCDIR_PREFIX}$(ARCH_SRCDIR)$(DELIM)$(WL80211_DIR)/macsw

# Source files

# wl80211 library sources
CHIP_CSRCS += $(WL80211_DIR)$(DELIM)wifi_mgmr.c

# NuttX-specific sources
CHIP_CSRCS += $(WL80211_DIR)$(DELIM)country.c
CHIP_CSRCS += $(WL80211_DIR)$(DELIM)nuttx.c
CHIP_CSRCS += $(WL80211_DIR)$(DELIM)rtos_al_nuttx.c


# C flags
#
