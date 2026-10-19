.PHONY: all clean

all: blctl blctld
all: qcctl qcctld tty_at_helper

CC ?= gcc
CFLAGS := -Wall -Wextra -O2 -g -pthread
BLCTL_INCLUDE_DIRS := -I./ -I../

BLCTL_OBJS := blctl.o blctl_ota.o utils.o blctl_log.o msg_handlers.o

blctl: $(BLCTL_OBJS) blctl_main.o
	@$(info Linking $@)
	@$(CC) $(CFLAGS) $(BLCTL_INCLUDE_DIRS) -o $@ $^

blctld: $(BLCTL_OBJS) blctld_main.o
	@$(info Linking $@)
	@$(CC) $(CFLAGS) $(BLCTL_INCLUDE_DIRS) -o $@ $^

tty_at_helper: tty_at_helper.o
	@$(info Linking $@)
	@$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@$(info CC $@)
	@$(CC) $(CFLAGS) $(BLCTL_INCLUDE_DIRS) -c -o $@ $<

clean:
	rm -f blctl blctld *.o
