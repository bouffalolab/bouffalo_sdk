EXAMPLE ?= uart_wifi
.DEFAULT_GOAL := all
.PHONY: all
all:
	$(MAKE) -C examples/$(EXAMPLE)

Makefile: ;
%:
	$(MAKE) -C examples/$(EXAMPLE) $@
