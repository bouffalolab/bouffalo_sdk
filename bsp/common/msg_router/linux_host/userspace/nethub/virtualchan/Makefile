# Makefile for nethub_vchan_app
# Compatible with old Linux distributions (kernel 3.x+)

CC ?= gcc
CFLAGS = -Wall -Wextra -O2 -D_GNU_SOURCE
LDFLAGS = -lpthread

TARGET = nethub_vchan_app
SRCS = nethub_vchan.c nethub_vchan_app.c
OBJS = $(SRCS:.c=.o)

# Detect readline support (compatible with old systems)
# Try pkg-config first, then fallback to manual detection
HAVE_PKGCONFIG := $(shell which pkg-config 2>/dev/null)
ifeq ($(HAVE_PKGCONFIG),)
    # No pkg-config, try manual detection
    HAVE_READLINE := $(shell [ -f /usr/include/readline/readline.h ] && echo 1 || echo 0)
    READLINE_LIBS := -lreadline -lncurses
    READLINE_CFLAGS :=
else
    # Use pkg-config
    HAVE_READLINE := $(shell pkg-config --exists readline && echo 1 || echo 0)
    READLINE_LIBS := $(shell pkg-config --libs readline 2>/dev/null || echo "-lreadline -lncurses")
    READLINE_CFLAGS := $(shell pkg-config --cflags readline 2>/dev/null || echo "")
endif

# Apply readline settings if available
ifeq ($(HAVE_READLINE),1)
    CFLAGS += -DHAVE_READLINE $(READLINE_CFLAGS)
    LDFLAGS += $(READLINE_LIBS)
endif

# Library name
LIB = libnethub_vchan.a

# Default target
all: $(TARGET) $(LIB)

# Build static library
$(LIB): nethub_vchan.o
	ar rcs $@ $<
	@echo "Build complete: $@"

# Build executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET) $(LIB)

# Install to system
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Uninstall from system
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Show configuration
config:
	@echo "Configuration:"
	@echo "  CC       = $(CC)"
	@echo "  CFLAGS   = $(CFLAGS)"
	@echo "  LDFLAGS  = $(LDFLAGS)"
	@echo "  TARGET    = $(TARGET)"
	@echo "  HAVE_READLINE = $(HAVE_READLINE)"
	@echo "  READLINE_LIBS = $(READLINE_LIBS)"

.PHONY: all clean install uninstall config
