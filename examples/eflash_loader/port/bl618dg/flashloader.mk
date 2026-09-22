# Leave the last 32 KiB of B0 AP RAM outside the link region for mailbox I/O.
BOARD ?= bl618dgdk
CPU_ID ?= ap
CONFIG_RAM_FOR_AP_SIZE ?= 0x68000
CONFIG_XZ ?= y
