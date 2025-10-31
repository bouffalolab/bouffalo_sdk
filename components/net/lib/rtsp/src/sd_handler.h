#ifndef __SD_HANDLER_H__
#define __SD_HANDLER_H__

struct sd_handler;
typedef int (*sd_handler_proc_t)(struct sd_handler *, unsigned int);

/*
 * Handler of socket descriptors.
 * Called when there's event occured on socket.
 */
struct sd_handler {
    struct list_head entry;     /* Entry of handler_list. */
    int sd;                     /* Can be all kinds of socket descriptors. */
    void *arg;                  /* Argument for the callback function. */
    sd_handler_proc_t proc;     /* Callback of the handler. */
};


void init_sd_handler_list(void);
void deinit_sd_handler_list(void);

int handle_lsn_sd(struct sd_handler *hp, unsigned int event);
int handle_rtsp_sd(struct sd_handler *hp, unsigned int event);
int handle_rtp_sd(struct sd_handler *hp, unsigned int event);
int handle_rtcp_sd(struct sd_handler *hp, unsigned int event);

void do_sd_handler(int sd, unsigned int event);

int register_sd_handler(int sd, sd_handler_proc_t proc, void *arg);
void deregister_sd_handler(int sd);


#endif /* __SD_HANDLER_H__ */
