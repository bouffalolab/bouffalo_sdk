# Makefile for BFLB WiFi Module

# Compiler and parameters
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
CFLAGS += -I../virtualchan
LDFLAGS = -lpthread
VCHAN_LIB = ../virtualchan/libnethub_vchan.a

# ==================== Optional Features ====================
# 启用 GOTIP 自动网卡配置功能
# 使用方法: make ENABLE_NETIF_AUTO_CONFIG=1
ifdef ENABLE_NETIF_AUTO_CONFIG
    CFLAGS += -DBFLBWIFI_NETIF_AUTO_CONFIG
    $(info ==> 启用 GOTIP 自动网卡配置功能)
endif

# ==================== Source Files ====================
# Core source files (always included)
SRCS = src/bflbwifi_log.c \
       src/bflbwifi_parser.c \
       src/bflbwifi_state.c \
       src/bflbwifi_wifi.c

# Channel abstraction layer (always included)
SRCS += src/channel/channel.c \
        src/channel/channel_tty.c \
        src/channel/channel_vchan.c \
        src/bflbwifi_tty.c

# 网卡配置模块（仅在启用自动配置时编译）
ifdef ENABLE_NETIF_AUTO_CONFIG
    SRCS += src/bflbwifi_netif.c
endif

# Object files
OBJS = $(SRCS:.c=.o)

# Library name
LIB = libbflbwifi.a

# App directory
APP_DIR = app
APP_COMMON_SRCS = $(APP_DIR)/bflbwifi_ipc.c $(APP_DIR)/bflbwifi_command.c
DAEMON_SRCS = $(APP_DIR)/bflbwifid.c \
              $(APP_DIR)/bflbwifid_config.c \
              $(APP_DIR)/bflbwifid_instance.c \
              $(APP_DIR)/bflbwifid_log.c \
              $(APP_DIR)/bflbwifid_ota.c \
              $(APP_DIR)/bflbwifid_request.c \
              $(APP_DIR)/bflbwifid_service.c \
              $(APP_DIR)/bflbwifid_runtime.c
APP_TARGETS = $(APP_DIR)/bflbwifid $(APP_DIR)/bflbwifictrl

# Default target
all: $(LIB) $(APP_TARGETS)

$(VCHAN_LIB):
	@echo "Building nethub_vchan static library..."
	$(MAKE) -C ../virtualchan libnethub_vchan.a

# Build library
$(LIB): $(OBJS)
	@echo "Archiving $@..."
	ar rcs $@ $^
	@echo "Build complete: $@"

# Build object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Build app programs
# bflbwifid: Daemon (depends on libbflbwifi)
$(APP_DIR)/bflbwifid: $(LIB) $(VCHAN_LIB) $(DAEMON_SRCS) $(APP_COMMON_SRCS)
	@echo "Compiling bflbwifid..."
	$(CC) $(CFLAGS) -o $@ $(DAEMON_SRCS) $(APP_COMMON_SRCS) -L. -lbflbwifi $(VCHAN_LIB) $(LDFLAGS)
	@echo "Built with unified TTY/VCHAN backend support"

# bflbwifictrl: CLI tool (only depends on pthread)
$(APP_DIR)/bflbwifictrl: $(APP_DIR)/bflbwifictrl.c $(APP_COMMON_SRCS)
	@echo "Compiling bflbwifictrl..."
	$(CC) $(CFLAGS) -o $@ $(APP_DIR)/bflbwifictrl.c $(APP_COMMON_SRCS) -lpthread

# Clean
clean:
	@echo "Cleaning..."
	rm -f $(OBJS) $(LIB) $(APP_TARGETS)
	@echo "Clean complete"

# Install (optional)
install: $(LIB) $(VCHAN_LIB) $(APP_TARGETS)
	@echo "Installing library and apps..."
	mkdir -p /usr/local/lib
	mkdir -p /usr/local/include/bflbwifi
	mkdir -p /usr/local/bin
	cp $(LIB) /usr/local/lib/
	cp $(VCHAN_LIB) /usr/local/lib/
	cp include/*.h /usr/local/include/bflbwifi/
	install -m 755 $(APP_DIR)/bflbwifid /usr/local/bin/
	install -m 755 $(APP_DIR)/bflbwifictrl /usr/local/bin/
	@echo "Install complete"

# Uninstall
uninstall:
	@echo "Uninstalling..."
	rm -rf /usr/local/lib/$(LIB)
	rm -rf /usr/local/lib/$(notdir $(VCHAN_LIB))
	rm -rf /usr/local/include/bflbwifi
	rm -f /usr/local/bin/bflbwifid
	rm -f /usr/local/bin/bflbwifictrl
	@echo "Uninstall complete"

# Help
help:
	@echo "BFLB WiFi Control Module"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build library and apps (default)"
	@echo "  clean     - Remove built files"
	@echo "  install   - Install library and apps to system"
	@echo "  uninstall - Remove library and apps from system"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Build Model:"
	@echo "  Unified backend build with both TTY and VCHAN support"
	@echo ""
	@echo "Examples:"
	@echo "  make                              # Build daemon and library"
	@echo "  make clean                        # Clean build artifacts"
	@echo ""
	@echo "Apps:"
	@echo "  $(APP_DIR)/bflbwifid     - WiFi daemon"
	@echo "  $(APP_DIR)/bflbwifictrl - WiFi CLI tool"

.PHONY: all clean install uninstall help
