# Makefile for BFLB WiFi Module

# Compiler and parameters
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
LDFLAGS = -lpthread

# 可选功能开关
# 启用 GOTIP 自动网卡配置功能
# 使用方法: make ENABLE_NETIF_AUTO_CONFIG=1
ifdef ENABLE_NETIF_AUTO_CONFIG
    CFLAGS += -DBFLBWIFI_NETIF_AUTO_CONFIG
    $(info ==> 启用 GOTIP 自动网卡配置功能)
endif

# Source files
SRCS = src/bflbwifi_log.c \
       src/bflbwifi_tty.c \
       src/bflbwifi_parser.c \
       src/bflbwifi_state.c \
       src/bflbwifi_wifi.c

# 网卡配置模块（仅在启用自动配置时编译）
ifdef ENABLE_NETIF_AUTO_CONFIG
    SRCS += src/bflbwifi_netif.c
endif

# Object files
OBJS = $(SRCS:.c=.o)

# Library name
LIB = libbflbwifi.a

# Example programs
EXAMPLE_SRCS = examples/test_bflbwifi.c
EXAMPLE = examples/test_bflbwifi

# App directory
APP_DIR = app
APP_TARGETS = $(APP_DIR)/bflbwifid $(APP_DIR)/bflbwifictrl

# Default target
all: $(LIB) $(APP_TARGETS)

# Build library
$(LIB): $(OBJS)
	@echo "Archiving $@..."
	ar rcs $@ $^
	@echo "Build complete: $@"

# Build object files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Build examples
examples: $(EXAMPLE)

$(EXAMPLE): $(LIB) $(EXAMPLE_SRCS)
	@echo "Building example..."
	$(CC) $(CFLAGS) $(EXAMPLE_SRCS) -L. -lbflbwifi $(LDFLAGS) -o $@
	@echo "Example built: $@"

# Build app programs
# bflbwifid: Daemon (depends on libbflbwifi)
$(APP_DIR)/bflbwifid: $(LIB) $(APP_DIR)/bflbwifid.c
	@echo "Compiling bflbwifid..."
	$(CC) $(CFLAGS) -o $@ $(APP_DIR)/bflbwifid.c -L. -lbflbwifi $(LDFLAGS)

# bflbwifictrl: CLI tool (only depends on pthread)
$(APP_DIR)/bflbwifictrl: $(APP_DIR)/bflbwifictrl.c
	@echo "Compiling bflbwifictrl..."
	$(CC) $(CFLAGS) -o $@ $(APP_DIR)/bflbwifictrl.c -lpthread

# Clean
clean:
	@echo "Cleaning..."
	rm -f $(OBJS) $(LIB) $(EXAMPLE) $(APP_TARGETS)
	@echo "Clean complete"

# Install (optional)
install: $(LIB) $(APP_TARGETS)
	@echo "Installing library and apps..."
	mkdir -p /usr/local/lib
	mkdir -p /usr/local/include/bflbwifi
	mkdir -p /usr/local/bin
	cp $(LIB) /usr/local/lib/
	cp include/*.h /usr/local/include/bflbwifi/
	install -m 755 $(APP_DIR)/bflbwifid /usr/local/bin/
	install -m 755 $(APP_DIR)/bflbwifictrl /usr/local/bin/
	@echo "Install complete"

# Uninstall
uninstall:
	@echo "Uninstalling..."
	rm -rf /usr/local/lib/$(LIB)
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
	@echo "  examples  - Build example programs"
	@echo "  clean     - Remove built files"
	@echo "  install   - Install library and apps to system"
	@echo "  uninstall - Remove library and apps from system"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Apps:"
	@echo "  $(APP_DIR)/bflbwifid     - WiFi daemon"
	@echo "  $(APP_DIR)/bflbwifictrl - WiFi CLI tool"

.PHONY: all examples clean install uninstall help
