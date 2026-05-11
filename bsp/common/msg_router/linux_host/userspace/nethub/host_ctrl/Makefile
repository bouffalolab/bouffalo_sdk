CC ?= gcc
AR ?= ar

CFLAGS ?= -O2
CFLAGS += -Wall -Wextra -D_GNU_SOURCE -fPIC -MMD -MP
CFLAGS += -Iinclude -Icommon -Iclient -Ilink -Iprotocol -Iwifi -Idaemon -I../virtualchan
LDFLAGS += -lpthread

PUBLIC_LIB_A = libnethub_wifi.a
PUBLIC_LIB_SO = libnethub_wifi.so
DAEMON = daemon/bflbwifid
CLI = cli/bflbwifictrl

COMMON_OBJS = common/nethub_ipc.o common/nethub_cmd.o common/nethub_log.o
CLIENT_OBJS = client/nethub_wifi_client.o
LINK_OBJS = link/nethub_ctrl_link.o
PROTO_OBJS = protocol/nethub_ctrl_proto_at.o protocol/nethub_ctrl_proto_private.o
WIFI_OBJS = wifi/nethub_wifi_service.o wifi/nethub_wifi_parser_at.o wifi/nethub_wifi_state.o wifi/nethub_netif.o
DAEMON_OBJS = daemon/bflbwifid.o daemon/bflbwifid_config.o daemon/bflbwifid_instance.o daemon/bflbwifid_service.o daemon/bflbwifid_runtime.o
CLI_OBJS = cli/bflbwifictrl.o
DEPS = $(COMMON_OBJS:.o=.d) $(CLIENT_OBJS:.o=.d) $(LINK_OBJS:.o=.d) $(PROTO_OBJS:.o=.d) $(WIFI_OBJS:.o=.d) $(DAEMON_OBJS:.o=.d) $(CLI_OBJS:.o=.d)

VCHAN_LIB = ../virtualchan/libnethub_vchan.a

LIB_OBJS = $(CLIENT_OBJS) common/nethub_ipc.o common/nethub_cmd.o
DAEMON_LINK_OBJS = $(DAEMON_OBJS) $(CLIENT_OBJS) $(COMMON_OBJS) $(LINK_OBJS) $(PROTO_OBJS) $(WIFI_OBJS)

all: $(PUBLIC_LIB_A) $(PUBLIC_LIB_SO) $(DAEMON) $(CLI)

$(VCHAN_LIB):
	$(MAKE) -C ../virtualchan libnethub_vchan.a

$(PUBLIC_LIB_A): $(LIB_OBJS)
	$(AR) rcs $@ $^

$(PUBLIC_LIB_SO): $(LIB_OBJS)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

$(DAEMON): $(DAEMON_LINK_OBJS) $(VCHAN_LIB)
	$(CC) -o $@ $(DAEMON_LINK_OBJS) $(VCHAN_LIB) $(LDFLAGS)

$(CLI): $(CLI_OBJS) $(PUBLIC_LIB_A)
	$(CC) -o $@ $(CLI_OBJS) $(PUBLIC_LIB_A) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(COMMON_OBJS) $(CLIENT_OBJS) $(LINK_OBJS) $(PROTO_OBJS) $(WIFI_OBJS) $(DAEMON_OBJS) $(CLI_OBJS)
	rm -f $(DEPS)
	rm -f $(PUBLIC_LIB_A) $(PUBLIC_LIB_SO) $(DAEMON) $(CLI)

install: all
	install -d /usr/local/include/nethub /usr/local/lib /usr/local/bin
	install -m 644 include/nethub_wifi.h include/nethub_wifi_types.h /usr/local/include/nethub/
	install -m 644 $(PUBLIC_LIB_A) /usr/local/lib/
	install -m 755 $(PUBLIC_LIB_SO) /usr/local/lib/
	install -m 755 $(DAEMON) $(CLI) /usr/local/bin/

.PHONY: all clean install

-include $(DEPS)
