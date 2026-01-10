# Makefile for netlink_app

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread

TARGET = netlink_app
SRCS = netlink_ctrl.c netlink_app.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Install to system
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Uninstall from system
uninstall:
	rm -f /usr/local/bin/$(TARGET)

.PHONY: all clean install uninstall
