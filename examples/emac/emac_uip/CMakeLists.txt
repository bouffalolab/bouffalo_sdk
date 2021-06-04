set(mains emac_basic.c )
set(TARGET_REQUIRED_SRCS emac_bd.c clock-arch.c tapdev.c timer.c uip.c uip_arp.c psock.c httpd.c httpd-cgi.c httpd-fs.c http-strings.c uip_main.c)
generate_bin()



