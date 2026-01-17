# NetHub Userspace Tools Makefile

.PHONY: all clean build-prepare bflbwifictrl bflbvirtualchannel help

# Default target: build bflbwifictrl, bflbvirtualchannel
all: build-prepare bflbwifictrl bflbvirtualchannel

# Build preparation
build-prepare:
	@echo "Preparing build environment..."
	@echo "✓ Build environment preparation complete"

bflbwifictrl:
	@echo "Building bflbwifictrl ..."
	@cd bflbwifictrl && $(MAKE)
	@echo "✓ bflbwifictrl build complete"

bflbvirtualchannel:
	@echo "Building bflbvirtualchannel tools..."
	@cd bflbvirtualchannel && $(MAKE)
	@echo "✓ bflbvirtualchannel build complete"

# Clean build files
clean:
	@echo "Cleaning build files..."
	@cd bflbwifictrl && $(MAKE) clean 2>/dev/null || true
	@cd bflbvirtualchannel && $(MAKE) clean 2>/dev/null || true
	@echo "✓ Build files cleanup complete"

# Show help information
help:
	@echo "NetHub Userspace Tools Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all                - Build all modules (default)"
	@echo "  bflbwifictrl      - Build bflbwifictrl module"
	@echo "  bflbvirtualchannel - Build bflbvirtualchannel module"
	@echo "  clean              - Clean build files"
	@echo "  help               - Show this help information"
	@echo ""
	@echo "Module description:"
	@echo "  bflbwifictrl      - BFLB WiFi AT command control module (daemon + CLI)"
	@echo "  bflbvirtualchannel - BFLB virtual channel (Netlink communication)"

