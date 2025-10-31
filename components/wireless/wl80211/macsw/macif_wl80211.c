#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "macsw/wl80211_mac.h"
#include "macsw.h"

#include "rtos_al.h"

extern void wl80211_handle_ke_msg(const struct ke_msg *ke_msg);

static rtos_mutex macif_call_mutex;
static struct macif_call_ctx {
  int rsp_msg_id;

  int rsp_msg_buf_len;
  void *rsp_msg_buf;

  rtos_semaphore rsp_wait_sem;
} *call_ctx = NULL;


int _macsw_macif_init(void) {
  if (rtos_mutex_create(&macif_call_mutex)) {
    abort();
  }

  /* init macsw interface */
  extern int macif_init(void);
  macif_init();
  return 0;
}

// send kmsg to macsw and wait response to make func call
void macif_kmsg_call_t(int cfm_msg_id, void *rsp, int rsp_size, int msg_id,
                       int task_id, int nb_msg, ...) {
  va_list va;
  int i, length;
  uint8_t *ke_msg_payload;
  uint8_t *ptr_src, *ptr_dest;
  length = 0;

  va_start(va, nb_msg);
  for (i = 0; i < nb_msg; i++) {
    ptr_src = va_arg(va, void *);
    length += va_arg(va, int);
  }
  va_end(va);

  ke_msg_payload = ke_msg_alloc(msg_id, task_id, TASK_API, length);

  ptr_dest = ke_msg_payload;
  va_start(va, nb_msg);
  for (i = 0; i < nb_msg; i++) {
    ptr_src = va_arg(va, void *);
    length = va_arg(va, int);
    memcpy(ptr_dest, ptr_src, length);
    ptr_dest += length;
  }
  va_end(va);

  rtos_mutex_lock(macif_call_mutex);

  if (cfm_msg_id != -1) {
    call_ctx = malloc(sizeof(struct macif_call_ctx));
    if (call_ctx == NULL) {
      printf("%s:%d\r\n", __func__, __LINE__);
      abort();
    }
    if (rtos_semaphore_create(&call_ctx->rsp_wait_sem, 1, 0)) {
      printf("%s:%d\r\n", __func__, __LINE__);
      abort();
    }
    call_ctx->rsp_msg_id = cfm_msg_id;
    call_ctx->rsp_msg_buf = rsp;
    call_ctx->rsp_msg_buf_len = rsp_size;
  }

  // dispatch the message to the Wifi task
  ke_msg_send(ke_msg_payload);
  wifi_task_resume(false);

  // wait mac fw response
  if (cfm_msg_id != -1) {
    rtos_semaphore_wait(call_ctx->rsp_wait_sem, -1);
    rtos_semaphore_delete(call_ctx->rsp_wait_sem);
    free(call_ctx);
    call_ctx = NULL;
  }

  rtos_mutex_unlock(macif_call_mutex);

  return;
}

void macif_kmsg_call(int cfm_msg_id, void *rsp, int rsp_size, int msg_id,
                     int task_id, void *param, int param_len) {
  macif_kmsg_call_t(cfm_msg_id, rsp, rsp_size, msg_id, task_id, 1, param,
                    param_len);
}

void macif_kmsg_fwd(const struct ke_msg *ke_msg) {
  if (call_ctx != NULL && ke_msg->id == call_ctx->rsp_msg_id) {
    if (call_ctx->rsp_msg_buf && call_ctx->rsp_msg_buf_len) {
      int copy = ke_msg->param_len;

#ifdef INVARIANTS
      if (copy > call_ctx->rsp_msg_buf_len) {
        printf("%s:%d\r\n", __func__, __LINE__);
        abort();
      }
#endif
      memcpy(call_ctx->rsp_msg_buf, ke_msg->param, copy);
    }
    ke_msg_free(ke_msg);
    rtos_semaphore_signal(call_ctx->rsp_wait_sem, 0);
  } else {
#if 1
    wl80211_handle_ke_msg(ke_msg);
#else
    printf("ke_msg id:%d\r\n", ke_msg->id);
    ke_msg_free(ke_msg);
#endif
  }
}
