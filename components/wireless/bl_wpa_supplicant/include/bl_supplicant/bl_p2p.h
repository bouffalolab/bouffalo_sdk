#ifndef BL_P2P_H
#define BL_P2P_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum bl_p2p_wps_mode {
    BL_P2P_WPS_PBC = 0,
    BL_P2P_WPS_PIN_DISPLAY,
    BL_P2P_WPS_PIN_KEYPAD,
};

typedef void (*bl_p2p_listen_pin_cb_t)(const uint8_t peer_addr[6],
                                       const char *pin, void *arg);

#if defined(CONFIG_BL_SUPPLICANT_P2P)

int bl_p2p_init(void);
int bl_p2p_set_device_name(const char *name);
int bl_p2p_set_go_intent(int go_intent, int listen_wps_go_intent);
void bl_p2p_get_go_intent(int *go_intent, int *listen_wps_go_intent);
void bl_p2p_deinit(void);
int bl_p2p_find(unsigned int timeout_sec);
int bl_p2p_listen(unsigned int duration_ms);
int bl_p2p_listen_on_channel(unsigned int duration_ms, uint8_t channel);
int bl_p2p_stop(void);
void bl_p2p_set_listen_pin_cb(bl_p2p_listen_pin_cb_t cb, void *arg);
int bl_p2p_listen_wps_start(enum bl_p2p_wps_mode mode, uint8_t channel,
                            unsigned int duration_ms, const char *pin);
void bl_p2p_listen_wps_disarm(void);
const char *bl_p2p_listen_wps_pin(void);
int bl_p2p_group_add(unsigned int freq, enum bl_p2p_wps_mode mode,
                     const char *pin);
int bl_p2p_connect(const uint8_t peer_addr[6], enum bl_p2p_wps_mode mode,
                   const char *pin, int go_intent, bool auth);
int bl_p2p_join(const uint8_t peer_addr[6], enum bl_p2p_wps_mode mode,
                const char *pin);
int bl_p2p_prov_disc(const uint8_t peer_addr[6], enum bl_p2p_wps_mode mode,
                     bool join);
void bl_p2p_notify_ap_assoc(const uint8_t *addr, const uint8_t *ie, size_t len);
void bl_p2p_notify_ap_disassoc(const uint8_t *addr);
bool bl_p2p_is_ready(void);
#else
static inline int bl_p2p_init(void)
{
    return -1;
}

static inline int bl_p2p_set_device_name(const char *name)
{
    (void) name;
    return -1;
}

static inline int bl_p2p_set_go_intent(int go_intent, int listen_wps_go_intent)
{
    (void) go_intent;
    (void) listen_wps_go_intent;
    return -1;
}

static inline void bl_p2p_get_go_intent(int *go_intent,
                                        int *listen_wps_go_intent)
{
    (void) go_intent;
    (void) listen_wps_go_intent;
}

static inline void bl_p2p_deinit(void)
{
}

static inline int bl_p2p_find(unsigned int timeout_sec)
{
    (void) timeout_sec;
    return -1;
}

static inline int bl_p2p_listen(unsigned int duration_ms)
{
    (void) duration_ms;
    return -1;
}

static inline int bl_p2p_listen_on_channel(unsigned int duration_ms,
                                           uint8_t channel)
{
    (void) duration_ms;
    (void) channel;
    return -1;
}

static inline int bl_p2p_stop(void)
{
    return -1;
}

static inline void bl_p2p_set_listen_pin_cb(bl_p2p_listen_pin_cb_t cb,
                                            void *arg)
{
    (void) cb;
    (void) arg;
}

static inline int bl_p2p_listen_wps_start(enum bl_p2p_wps_mode mode,
                                          uint8_t channel,
                                          unsigned int duration_ms,
                                          const char *pin)
{
    (void) mode;
    (void) channel;
    (void) duration_ms;
    (void) pin;
    return -1;
}

static inline void bl_p2p_listen_wps_disarm(void)
{
}

static inline const char *bl_p2p_listen_wps_pin(void)
{
    return NULL;
}

static inline int bl_p2p_group_add(unsigned int freq, enum bl_p2p_wps_mode mode,
                                   const char *pin)
{
    (void) freq;
    (void) mode;
    (void) pin;
    return -1;
}

static inline int bl_p2p_connect(const uint8_t peer_addr[6],
                                 enum bl_p2p_wps_mode mode,
                                 const char *pin, int go_intent, bool auth)
{
    (void) peer_addr;
    (void) mode;
    (void) pin;
    (void) go_intent;
    (void) auth;
    return -1;
}

static inline int bl_p2p_join(const uint8_t peer_addr[6],
                              enum bl_p2p_wps_mode mode, const char *pin)
{
    (void) peer_addr;
    (void) mode;
    (void) pin;
    return -1;
}

static inline int bl_p2p_prov_disc(const uint8_t peer_addr[6],
                                   enum bl_p2p_wps_mode mode, bool join)
{
    (void) peer_addr;
    (void) mode;
    (void) join;
    return -1;
}

static inline void bl_p2p_notify_ap_assoc(const uint8_t *addr, const uint8_t *ie,
                                          size_t len)
{
    (void) addr;
    (void) ie;
    (void) len;
}

static inline void bl_p2p_notify_ap_disassoc(const uint8_t *addr)
{
    (void) addr;
}

static inline bool bl_p2p_is_ready(void)
{
    return false;
}
#endif

#endif
