/**
 * @file
 * LWIP HTTP server implementation
 */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *         Simon Goldschmidt
 *
 */
#include <string.h> /* memset */
#include <stdlib.h> /* atoi */
#include <stdio.h>

#include "lwip/init.h"
#include "lwip/apps/httpd.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/apps/fs.h"
#include "lwip/def.h"

#include "lwip/altcp.h"
#include "lwip/altcp_tcp.h"
#if HTTPD_ENABLE_HTTPS
#include "lwip/altcp_tls.h"
#endif
#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif
#if LWIP_HTTPD_TIMING
#include "lwip/sys.h"
#endif /* LWIP_HTTPD_TIMING */

#include "openthread_rest.h"
#include "openthread_httpd.h"

#if LWIP_TCP && LWIP_CALLBACK_API

/** Minimum length for a valid HTTP/0.9 request: "GET /\r\n" -> 7 bytes */
#define MIN_REQ_LEN   7

#define CRLF "\r\n"
#define HTTP11_CONNECTIONKEEPALIVE  "Connection: keep-alive"
#define HTTP11_CONNECTIONKEEPALIVE2 "Connection: Keep-Alive"

#define HTTP_IS_DYNAMIC_FILE(hs) 0

/* This defines checks whether tcp_write has to copy data or not */

#ifndef HTTP_IS_DATA_VOLATILE
/** tcp_write does not have to copy data when sent from rom-file-system directly */
#define HTTP_IS_DATA_VOLATILE(hs)       (HTTP_IS_DYNAMIC_FILE(hs) ? TCP_WRITE_FLAG_COPY : 0)
#endif
/** Default: dynamic headers are sent from ROM (non-dynamic headers are handled like file data) */
#ifndef HTTP_IS_HDR_VOLATILE
#define HTTP_IS_HDR_VOLATILE(hs, ptr)   0
#endif

/* Return values for http_send_*() */
#define HTTP_DATA_TO_SEND_FREED    3
#define HTTP_DATA_TO_SEND_BREAK    2
#define HTTP_DATA_TO_SEND_CONTINUE 1
#define HTTP_NO_DATA_TO_SEND       0

struct http_state {
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
  struct http_state *next;
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  struct fs_file file_handle;
  struct fs_file *handle;
  const char *file;       /* Pointer to first unsent byte in buf. */

  struct altcp_pcb *pcb;
  struct pbuf *req;

  u32_t left;       /* Number of unsent bytes in buf. */
  u8_t retries;
  u8_t keepalive;
  uint16_t req_len;

#if LWIP_HTTPD_TIMING
  u32_t time_started;
#endif /* LWIP_HTTPD_TIMING */
};

#if HTTPD_USE_MEM_POOL
LWIP_MEMPOOL_DECLARE(HTTPD_STATE,     MEMP_NUM_PARALLEL_HTTPD_CONNS,     sizeof(struct http_state),     "HTTPD_STATE")
#define HTTP_ALLOC_HTTP_STATE() (struct http_state *)LWIP_MEMPOOL_ALLOC(HTTPD_STATE)
#define HTTP_FREE_HTTP_STATE(x) LWIP_MEMPOOL_FREE(HTTPD_STATE, (x))
#else /* HTTPD_USE_MEM_POOL */
#define HTTP_ALLOC_HTTP_STATE() (struct http_state *)mem_malloc(sizeof(struct http_state))
#define HTTP_FREE_HTTP_STATE(x) mem_free(x)
#endif /* HTTPD_USE_MEM_POOL */

static err_t http_close_conn(struct altcp_pcb *pcb, struct http_state *hs);
static err_t http_close_or_abort_conn(struct altcp_pcb *pcb, struct http_state *hs, u8_t abort_conn);
static err_t http_init_file(struct http_state *hs, struct fs_file *file, u8_t tag_check, char *params);
static err_t http_poll(void *arg, struct altcp_pcb *pcb);
static u8_t http_check_eof(struct altcp_pcb *pcb, struct http_state *hs);
#if LWIP_HTTPD_FS_ASYNC_READ
static void http_continue(void *connection);
#else
#define http_continue NULL
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
static err_t http_parse_request(struct pbuf *inp, struct http_state *hs, struct altcp_pcb *pcb);

static const http_method_t http_method_list[] = {
  {
    .id = http_method_type_get,
    .method = "GET",
    .method_len = sizeof("GET")
  },
  {
    .id = http_method_type_put,
    .method = "PUT",
    .method_len = sizeof("PUT")
  },
  {
    .id = http_method_type_delete,
    .method = "DELETE",
    .method_len = sizeof("DELETE")
  },
};

#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
/** global list of active HTTP connections, use to kill the oldest when
    running out of memory */
static struct http_state *http_connections;

fs_bytes_left(struct fs_file *file)
{
  return file->len - file->index;
}

static void
http_add_connection(struct http_state *hs)
{
  /* add the connection to the list */
  hs->next = http_connections;
  http_connections = hs;
}

static void
http_remove_connection(struct http_state *hs)
{
  /* take the connection off the list */
  if (http_connections) {
    if (http_connections == hs) {
      http_connections = hs->next;
    } else {
      struct http_state *last;
      for (last = http_connections; last->next != NULL; last = last->next) {
        if (last->next == hs) {
          last->next = hs->next;
          break;
        }
      }
    }
  }
}

static void
http_kill_oldest_connection(u8_t ssi_required)
{
  struct http_state *hs = http_connections;
  struct http_state *hs_free_next = NULL;
  while (hs && hs->next) {
    LWIP_UNUSED_ARG(ssi_required);
    {
      hs_free_next = hs;
    }
    LWIP_ASSERT("broken list", hs != hs->next);
    hs = hs->next;
  }
  if (hs_free_next != NULL) {
    LWIP_ASSERT("hs_free_next->next != NULL", hs_free_next->next != NULL);
    LWIP_ASSERT("hs_free_next->next->pcb != NULL", hs_free_next->next->pcb != NULL);
    /* send RST when killing a connection because of memory shortage */
    http_close_or_abort_conn(hs_free_next->next->pcb, hs_free_next->next, 1); /* this also unlinks the http_state from the list */
  }
}
#else /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

#define http_add_connection(hs)
#define http_remove_connection(hs)

#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */

/** Initialize a struct http_state.
 */
static void
http_state_init(struct http_state *hs)
{
  /* Initialize the structure. */
  memset(hs, 0, sizeof(struct http_state));
}

/** Allocate a struct http_state. */
static struct http_state *
http_state_alloc(void)
{
  struct http_state *ret = HTTP_ALLOC_HTTP_STATE();
#if LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED
  if (ret == NULL) {
    http_kill_oldest_connection(0);
    ret = HTTP_ALLOC_HTTP_STATE();
  }
#endif /* LWIP_HTTPD_KILL_OLD_ON_CONNECTIONS_EXCEEDED */
  if (ret != NULL) {
    http_state_init(ret);
    http_add_connection(ret);
  }
  return ret;
}

/** Free a struct http_state.
 * Also frees the file data if dynamic.
 */
static void
http_state_eof(struct http_state *hs)
{
  if (hs->handle) {
#if LWIP_HTTPD_TIMING
    u32_t ms_needed = sys_now() - hs->time_started;
    u32_t needed = LWIP_MAX(1, (ms_needed / 100));
    LWIP_DEBUGF(HTTPD_DEBUG_TIMING, ("httpd: needed %"U32_F" ms to send file of %d bytes -> %"U32_F" bytes/sec\n",
                                     ms_needed, hs->handle->len, ((((u32_t)hs->handle->len) * 10) / needed)));
#endif /* LWIP_HTTPD_TIMING */
    // fs_close(hs->handle);
    hs->handle = NULL;
  }
}

/** Free a struct http_state.
 * Also frees the file data if dynamic.
 */
static void
http_state_free(struct http_state *hs)
{
  if (hs != NULL) {
    http_state_eof(hs);
    http_remove_connection(hs);
    HTTP_FREE_HTTP_STATE(hs);
  }
}

/** Call tcp_write() in a loop trying smaller and smaller length
 *
 * @param pcb altcp_pcb to send
 * @param ptr Data to send
 * @param length Length of data to send (in/out: on return, contains the
 *        amount of data sent)
 * @param apiflags directly passed to tcp_write
 * @return the return value of tcp_write
 */
static err_t
http_write(struct altcp_pcb *pcb, const void *ptr, u16_t *length, u8_t apiflags)
{
  u16_t len, max_len;
  err_t err;
  LWIP_ASSERT("length != NULL", length != NULL);
  len = *length;
  if (len == 0) {
    return ERR_OK;
  }
  /* We cannot send more data than space available in the send buffer. */
  max_len = altcp_sndbuf(pcb);
  if (max_len < len) {
    len = max_len;
  }
#ifdef HTTPD_MAX_WRITE_LEN
  /* Additional limitation: e.g. don't enqueue more than 2*mss at once */
  max_len = HTTPD_MAX_WRITE_LEN(pcb);
  if (len > max_len) {
    len = max_len;
  }
#endif /* HTTPD_MAX_WRITE_LEN */
  do {
    LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Trying to send %d bytes\n", len));
    err = altcp_write(pcb, ptr, len, apiflags);
    if (err == ERR_MEM) {
      if ((altcp_sndbuf(pcb) == 0) ||
          (altcp_sndqueuelen(pcb) >= TCP_SND_QUEUELEN)) {
        /* no need to try smaller sizes */
        len = 1;
      } else {
        len /= 2;
      }
      LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE,
                  ("Send failed, trying less (%d bytes)\n", len));
    }
  } while ((err == ERR_MEM) && (len > 1));

  if (err == ERR_OK) {
    LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Sent %d bytes\n", len));
    *length = len;
  } else {
    LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("Send failed with err %d (\"%s\")\n", err, lwip_strerr(err)));
    *length = 0;
  }

#if LWIP_HTTPD_SUPPORT_11_KEEPALIVE
  /* ensure nagle is normally enabled (only disabled for persistent connections
     when all data has been enqueued but the connection stays open for the next
     request */
  altcp_nagle_enable(pcb);
#endif

  return err;
}

/**
 * The connection shall be actively closed (using RST to close from fault states).
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t
http_close_or_abort_conn(struct altcp_pcb *pcb, struct http_state *hs, u8_t abort_conn)
{
  err_t err;
  LWIP_DEBUGF(HTTPD_DEBUG, ("Closing connection %p\n", (void *)pcb));

  altcp_arg(pcb, NULL);
  altcp_recv(pcb, NULL);
  altcp_err(pcb, NULL);
  altcp_poll(pcb, NULL, 0);
  altcp_sent(pcb, NULL);
  if (hs != NULL) {
    http_state_free(hs);
  }

  if (abort_conn) {
    altcp_abort(pcb);
    return ERR_OK;
  }
  err = altcp_close(pcb);
  if (err != ERR_OK) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("Error %d closing %p\n", err, (void *)pcb));
    /* error closing, try again later in poll */
    altcp_poll(pcb, http_poll, HTTPD_POLL_INTERVAL);
  }
  return err;
}

/**
 * The connection shall be actively closed.
 * Reset the sent- and recv-callbacks.
 *
 * @param pcb the tcp pcb to reset callbacks
 * @param hs connection state to free
 */
static err_t
http_close_conn(struct altcp_pcb *pcb, struct http_state *hs)
{
  return http_close_or_abort_conn(pcb, hs, 0);
}

/** End of file: either close the connection (Connection: close) or
 * close the file (Connection: keep-alive)
 */
static void
http_eof(struct altcp_pcb *pcb, struct http_state *hs)
{
  if (hs->file_handle.data) {
    free((void *)hs->file_handle.data);
    hs->file_handle.data = NULL;

    if (hs->req) {
      http_parse_request(NULL, hs, pcb);
      hs->keepalive = 1;
    }
  }

  /* HTTP/1.1 persistent connection? (Not supported for SSI) */
  if (hs->keepalive) {
    http_remove_connection(hs);

    http_state_eof(hs);
    http_state_init(hs);
    /* restore state: */
    hs->pcb = pcb;
    hs->keepalive = 1;
    http_add_connection(hs);
    /* ensure nagle doesn't interfere with sending all data as fast as possible: */
    altcp_nagle_disable(pcb);
  } 
  else {
    http_close_conn(pcb, hs);
  }
}

/** Sub-function of http_send(): end-of-file (or block) is reached,
 * either close the file or read the next block (if supported).
 *
 * @returns: 0 if the file is finished or no data has been read
 *           1 if the file is not finished and data has been read
 */
static u8_t
http_check_eof(struct altcp_pcb *pcb, struct http_state *hs)
{
  int bytes_left;

  /* Do we have a valid file handle? */
  if (hs->handle == NULL) {
    /* No - close the connection. */
    http_eof(pcb, hs);
    return 0;
  }
  bytes_left = fs_bytes_left(hs->handle);
  if (bytes_left <= 0) {
    /* We reached the end of the file so this request is done. */
    LWIP_DEBUGF(HTTPD_DEBUG, ("End of file.\n"));
    http_eof(pcb, hs);
    return 0;
  }

  LWIP_ASSERT("SSI and DYNAMIC_HEADERS turned off but eof not reached", 0);
  return 1;
}

/** Sub-function of http_send(): This is the normal send-routine for non-ssi files
 *
 * @returns: - 1: data has been written (so call tcp_ouput)
 *           - 0: no data has been written (no need to call tcp_output)
 */
static u8_t
http_send_data_nonssi(struct altcp_pcb *pcb, struct http_state *hs)
{
  err_t err;
  u16_t len;
  u8_t data_to_send = 0;

  /* We are not processing an SHTML file so no tag checking is necessary.
   * Just send the data as we received it from the file. */
  len = (u16_t)LWIP_MIN(hs->left, 0xffff);

  err = http_write(pcb, hs->file, &len, HTTP_IS_DATA_VOLATILE(hs));
  if (err == ERR_OK) {
    data_to_send = 1;
    hs->file += len;
    hs->left -= len;
  }

  return data_to_send;
}

/**
 * Try to send more data on this pcb.
 *
 * @param pcb the pcb to send data
 * @param hs connection state
 */
static u8_t
http_send(struct altcp_pcb *pcb, struct http_state *hs)
{
  u8_t data_to_send = HTTP_NO_DATA_TO_SEND;

  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_send: pcb=%p hs=%p left=%d\n", (void *)pcb,
              (void *)hs, hs != NULL ? (int)hs->left : 0));

  /* If we were passed a NULL state structure pointer, ignore the call. */
  if (hs == NULL) {
    return 0;
  }

#if LWIP_HTTPD_FS_ASYNC_READ
  /* Check if we are allowed to read from this file.
     (e.g. SSI might want to delay sending until data is available) */
  if (!fs_is_file_ready(hs->handle, http_continue, hs)) {
    return 0;
  }
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

  /* Have we run out of file data to send? If so, we need to read the next
   * block from the file. */
  if (hs->left == 0) {
    if (!http_check_eof(pcb, hs)) {
      return 0;
    }
  }

  data_to_send = http_send_data_nonssi(pcb, hs);

  if ((hs->left == 0) && (fs_bytes_left(hs->handle) <= 0)) {
    /* We reached the end of the file so this request is done.
     * This adds the FIN flag right into the last data segment. */
    LWIP_DEBUGF(HTTPD_DEBUG, ("End of file.\n"));
    http_eof(pcb, hs);
    return 0;
  }
  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("send_data end.\n"));
  return data_to_send;
}

#define http_find_error_file(hs, error_nr) ERR_ARG

#if LWIP_HTTPD_FS_ASYNC_READ
/** Try to send more data if file has been blocked before
 * This is a callback function passed to fs_read_async().
 */
static void
http_continue(void *connection)
{
  struct http_state *hs = (struct http_state *)connection;
  LWIP_ASSERT_CORE_LOCKED();
  if (hs && (hs->pcb) && (hs->handle)) {
    LWIP_ASSERT("hs->pcb != NULL", hs->pcb != NULL);
    LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("httpd_continue: try to send more data\n"));
    if (http_send(hs->pcb, hs)) {
      /* If we wrote anything to be sent, go ahead and send it now. */
      LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("tcp_output\n"));
      altcp_output(hs->pcb);
    }
  }
}
#endif /* LWIP_HTTPD_FS_ASYNC_READ */

/**
 * When data has been received in the correct state, try to parse it
 * as a HTTP request.
 *
 * @param inp the received pbuf
 * @param hs the connection state
 * @param pcb the altcp_pcb which received this packet
 * @return ERR_OK if request was OK and hs has been initialized correctly
 *         ERR_INPROGRESS if request was OK so far but not fully received
 *         another err_t otherwise
 */
static err_t
http_parse_request(struct pbuf *inp, struct http_state *hs, struct altcp_pcb *pcb)
{
  char *data = NULL, * body = NULL;
  u32_t data_len, line_len, body_len = 0;
  u16_t req_len;
  struct pbuf *p = inp;
  char *uri = NULL;
  char *ver = NULL;
  char *sp1, *sp2, *http_header_start = NULL, *http_header_end = NULL, *ptr = NULL;
  http_method_type_t method_type = http_method_type_unkown;
  http_accept_type_t accept_type = http_accept_type_json;
  http_resp_state_t  error_state = http_resp_state_ok;
  char body_len_str[8];
#ifdef LWIP_DEBUG
  char * lwip_debug_str = NULL;
#endif

  LWIP_UNUSED_ARG(pcb); /* only used for post */
  LWIP_ASSERT("hs != NULL", hs != NULL);
  LWIP_ASSERT("(p != NULL || hs->req =! NULL)", (p != NULL || hs->req != NULL));

  if ((hs->handle != NULL) || (hs->file != NULL)) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("Received data while sending a file\n"));
    if (inp) {
      pbuf_chain(hs->req, inp);
    }
    return ERR_USE;
  }

  if (inp) {
    if (hs->req) {
      pbuf_chain(hs->req, inp);
    }
    else {
      pbuf_ref(inp);
      hs->req = inp;
    }
  }

  /* received enough data for minimal request? */
  data_len = hs->req->tot_len;
  VerifyOrExit(data_len >= MIN_REQ_LEN, (lwip_debug_str = "Invalid length\n", 
                                         error_state = http_resp_state_bad_request));

  data = (char *)malloc(data_len + 1);
  VerifyOrExit(data != NULL, (lwip_debug_str = "No enough memory\n",
                              error_state = http_resp_state_internal_server_error));
  pbuf_copy_partial(hs->req, data, data_len, 0);
  data[data_len] = '\0';

  http_header_end = lwip_strnstr(data, CRLF CRLF, data_len);
  error_state = http_resp_state_bad_request;

  if (http_header_end != NULL) {
    LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("HTTP request receved, try to parse.\n"));
    
    /** try to find content-length */
    sp1 = data + 2;
    do {
      sp1 = lwip_strnstr(sp1, HTTP_CONTENT_LEN, http_header_end - sp1 + sizeof(CRLF) - 1);
      if (sp1) {
        sp1 = sp1 + sizeof(HTTP_CONTENT_LEN) - 1;
        sp2 = lwip_strnstr(sp1, CRLF, (size_t)(http_header_end - sp1 + sizeof(CRLF) - 1));

        VerifyOrExit(sp2 && (size_t)(sp2 - sp1) < sizeof(body_len_str), 
                     (lwip_debug_str = "No found http request\n",
                      error_state = http_resp_state_bad_request));

        memcpy(body_len_str, sp1, (size_t)(sp2 - sp1));
        body_len_str[(size_t)(sp2 - sp1)] = '\0';
        body_len = atoi(body_len_str);

        sp1 = sp2 + 2;
      }
    } while (sp1);

    hs->req_len = body_len + (size_t)(http_header_end - data) + 2 * (sizeof(CRLF) - 1);
    LWIP_DEBUGF(HTTPD_DEBUG, ("Expect %d bytes, and %d bytes received\n", hs->req_len, data_len));
    VerifyOrExit(data_len >= hs->req_len, error_state = http_resp_state_none);

    /** try to request line */
    ptr = data;
    do {
      sp2 = lwip_strnstr(ptr, CRLF, (size_t)(http_header_end + sizeof(CRLF) - ptr - 1));

      VerifyOrExit(sp2, (lwip_debug_str = "Failed to get http request line\n",
                         error_state = http_resp_state_bad_request));
      sp2[0] = '\0';
      sp1 = lwip_strnstr(ptr, " ", (size_t)(sp2 - ptr));
      if (sp1) {
        for (int i = 0; i < sizeof(http_method_list)/ sizeof(http_method_list[0]); i ++) {
          if (0 == memcmp(ptr, http_method_list[i].method, http_method_list[i].method_len - 1)) {
            method_type = http_method_list[i].id;
            break;
          }
        }

        if (sp1[1] == '/') {
          uri = sp1 + 1;
          sp1 = lwip_strnstr(uri, " ", (size_t)(sp2 - uri));
          if (sp1 == NULL) {
            sp1 = sp2;
          } 
          else {
            ver = sp1 + 1;
          }
          if (sp1 > uri && (size_t)(sp1 - uri) > 1) {
            sp1[0] = '\0';
          }
          else {
            uri = NULL;
          }

          if (uri && method_type != http_method_type_unkown) {
            http_header_start = sp2 + 2;
          }
        }
      }

      ptr = sp2 + 2;
    } while (ptr < http_header_end);

    VerifyOrExit(uri && http_header_start, (lwip_debug_str = "Failed to parse requet line\n",
                                            error_state = http_resp_state_bad_request));

    VerifyOrExit(method_type != http_method_type_unkown, (lwip_debug_str = "Unsupported request method\n",
                                                          error_state = http_resp_state_not_implemented));

    LWIP_DEBUGF(HTTPD_DEBUG, ("Received %s %s request for URI: %s\n",
                              ver ? ver:"HTTP/0.9", http_method_list[method_type].method, uri));

    /* Try to parse http header */
    ptr = http_header_start;

    do {
      line_len = strlen(ptr);
      LWIP_DEBUGF(HTTPD_DEBUG, ("HTTP header: [%s], %d\n", ptr, line_len));

      if (lwip_strnstr(ptr, HTTP11_CONNECTIONKEEPALIVE, line_len) 
        || lwip_strnstr(ptr, HTTP11_CONNECTIONKEEPALIVE2, line_len)) {
        hs->keepalive = 1;
      } else {
        hs->keepalive = 0;
      }

      if (lwip_strnstr(ptr, HTTP_HDR_ACCEPT_JSON, line_len)) {
        accept_type = http_accept_type_json;
      } else if (lwip_strnstr(sp1, HTTP_HDR_ACCEPT_TXT, line_len)) {
        accept_type = http_accept_type_txt;
      }

      ptr = ptr + line_len + 2;
    } while (ptr < http_header_end);

    LWIP_DEBUGF(HTTPD_DEBUG, ("More %d bytes over body %d bytes.\n", 
                              data_len - (body_len + (size_t)(http_header_end - data) + 2 * (sizeof(CRLF) - 1)),
                              body_len));

    if (body_len) {
      body = http_header_end + 4;
    }

    VerifyOrExit(openthread_rest_request(hs, method_type, accept_type, uri, body, http_continue), 
                 (lwip_debug_str = "Failed to construct response", 
                  error_state = http_resp_state_internal_server_error));

    error_state = http_resp_state_ok;
  }
  else {
    LWIP_DEBUGF(HTTPD_DEBUG, ("Received %d bytes, but not found CRLF CRLF\n", data_len));
    VerifyOrExit(data_len < LWIP_HTTPD_MAX_REQ_LENGTH, error_state = http_resp_state_bad_request);
  }

exit:

#ifdef LWIP_DEBUG
  if (lwip_debug_str) {
    LWIP_DEBUGF(HTTPD_DEBUG, (lwip_debug_str));
  }
#endif
  if (data) {
    free(data);
  }

  if (error_state == http_resp_state_ok) {
    if (hs->req) {
      p = hs->req;
      req_len = hs->req_len;
      while (p && req_len) {
        if (p->len > req_len) {
          memmove(p->payload, (char *)p->payload + req_len, req_len);
          p->len -= req_len;
          p->tot_len -= req_len;
          break;
        }
        else {
          hs->req = p->next;
          p->next = NULL;
          p->tot_len = p->len;
          pbuf_free(p);
          p = hs->req;
        }
      }
    }

    return http_init_file(hs, &hs->file_handle, 0, NULL);
  }
  else if (error_state == http_resp_state_none) {
    return ERR_INPROGRESS;
  }
  else {
    pbuf_free(hs->req);
    hs->req = NULL;
    return http_init_error_file(hs, http_resp_state_bad_request);
  }
}

/** Initialize a http connection with a file to send (if found).
 * Called by http_find_file and http_find_error_file.
 *
 * @param hs http connection state
 * @param file file structure to send (or NULL if not found)
 * @param uri the HTTP header URI
 * @param tag_check enable SSI tag checking
 * @param params != NULL if URI has parameters (separated by '?')
 * @return ERR_OK if file was found and hs has been initialized correctly
 *         another err_t otherwise
 */
static err_t
http_init_file(struct http_state *hs, struct fs_file *file, u8_t tag_check, char *params)
{
  if (file != NULL) {
    /* file opened, initialise struct http_state */
    /* If dynamic read is disabled, file data must be in one piece and available now */
    LWIP_ASSERT("file->data != NULL", file->data != NULL);

    LWIP_UNUSED_ARG(tag_check);
    hs->handle = file;
    LWIP_UNUSED_ARG(params);
    hs->file = file->data;
    LWIP_ASSERT("File length must be positive!", (file->len >= 0));

    hs->left = (u32_t)file->len;
    hs->retries = 0;
#if LWIP_HTTPD_TIMING
    hs->time_started = sys_now();
#endif /* LWIP_HTTPD_TIMING */
    LWIP_ASSERT("HTTP headers not included in file system",
                (hs->handle->flags & FS_FILE_FLAGS_HEADER_INCLUDED) != 0);
  } else {
    hs->handle = NULL;
    hs->file = NULL;
    hs->left = 0;
    hs->retries = 0;
  }

  if (hs->keepalive) {
    if ((hs->handle != NULL) &&
        ((hs->handle->flags & (FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT)) == FS_FILE_FLAGS_HEADER_INCLUDED)) {
      hs->keepalive = 0;
    }
  }
  return ERR_OK;
}

/**
 * The pcb had an error and is already deallocated.
 * The argument might still be valid (if != NULL).
 */
static void
http_err(void *arg, err_t err)
{
  struct http_state *hs = (struct http_state *)arg;
  LWIP_UNUSED_ARG(err);

  LWIP_DEBUGF(HTTPD_DEBUG, ("http_err: %s", lwip_strerr(err)));

  if (hs != NULL) {
    http_state_free(hs);
  }
}

/**
 * Data has been sent and acknowledged by the remote host.
 * This means that more data can be sent.
 */
static err_t
http_sent(void *arg, struct altcp_pcb *pcb, u16_t len)
{
  struct http_state *hs = (struct http_state *)arg;

  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_sent %p\n", (void *)pcb));

  LWIP_UNUSED_ARG(len);

  if (hs == NULL) {
    return ERR_OK;
  }

  hs->retries = 0;

  http_send(pcb, hs);

  return ERR_OK;
}

/**
 * The poll function is called every 2nd second.
 * If there has been no data sent (which resets the retries) in 8 seconds, close.
 * If the last portion of a file has not been sent in 2 seconds, close.
 *
 * This could be increased, but we don't want to waste resources for bad connections.
 */
static err_t
http_poll(void *arg, struct altcp_pcb *pcb)
{
  struct http_state *hs = (struct http_state *)arg;
  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_poll: pcb=%p hs=%p pcb_state=%s\n",
              (void *)pcb, (void *)hs, tcp_debug_state_str(altcp_dbg_get_tcp_state(pcb))));

  if (hs == NULL) {
    err_t closed;
    /* arg is null, close. */
    LWIP_DEBUGF(HTTPD_DEBUG, ("http_poll: arg is NULL, close\n"));
    closed = http_close_conn(pcb, NULL);
    LWIP_UNUSED_ARG(closed);
#if LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR
    if (closed == ERR_MEM) {
      altcp_abort(pcb);
      return ERR_ABRT;
    }
#endif /* LWIP_HTTPD_ABORT_ON_CLOSE_MEM_ERROR */
    return ERR_OK;
  } else {
    hs->retries++;
    if (hs->retries == HTTPD_MAX_RETRIES) {
      LWIP_DEBUGF(HTTPD_DEBUG, ("http_poll: too many retries, close\n"));
      http_close_conn(pcb, hs);
      return ERR_OK;
    }

    /* If this connection has a file open, try to send some more data. If
     * it has not yet received a GET request, don't do this since it will
     * cause the connection to close immediately. */
    if (hs->handle) {
      LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_poll: try to send more data\n"));
      if (http_send(pcb, hs)) {
        /* If we wrote anything to be sent, go ahead and send it now. */
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("tcp_output\n"));
        altcp_output(pcb);
      }
    }
  }

  return ERR_OK;
}

/**
 * Data has been received on this pcb.
 * For HTTP 1.0, this should normally only happen once (if the request fits in one packet).
 */
static err_t
http_recv(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t err)
{
  struct http_state *hs = (struct http_state *)arg;
  LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_recv: pcb=%p pbuf=%p err=%s\n", (void *)pcb,
              (void *)p, lwip_strerr(err)));

  if ((err != ERR_OK) || (p == NULL) || (hs == NULL)) {
    /* error or closed by other side? */
    if (p != NULL) {
      /* Inform TCP that we have taken the data. */
      altcp_recved(pcb, p->tot_len);
      pbuf_free(p);
    }
    if (hs == NULL) {
      /* this should not happen, only to be robust */
      LWIP_DEBUGF(HTTPD_DEBUG, ("Error, http_recv: hs is NULL, close\n"));
    }
    http_close_conn(pcb, hs);
    return ERR_OK;
  }

  /* Inform TCP that we have taken the data. */
  altcp_recved(pcb, p->tot_len);

  if (hs->handle == NULL) {
    err_t parsed = http_parse_request(p, hs, pcb);
    LWIP_ASSERT("http_parse_request: unexpected return value", parsed == ERR_OK
                || parsed == ERR_INPROGRESS || parsed == ERR_ARG || parsed == ERR_USE);
    
    pbuf_free(p);
    if (parsed == ERR_OK) {
        LWIP_DEBUGF(HTTPD_DEBUG | LWIP_DBG_TRACE, ("http_recv: data %p len %"S32_F"\n", (const void *)hs->file, hs->left));
        http_send(pcb, hs);
    } else if (parsed == ERR_ARG) {
      /* @todo: close on ERR_USE? */
      http_close_conn(pcb, hs);
    }
  } else {
    LWIP_DEBUGF(HTTPD_DEBUG, ("http_recv: already sending data\n"));
    /* already sending but still receiving data, we might want to RST here? */
    pbuf_free(p);
  }

  return ERR_OK;
}

/**
 * A new incoming connection has been accepted.
 */
static err_t
http_accept(void *arg, struct altcp_pcb *pcb, err_t err)
{
  struct http_state *hs;
  LWIP_UNUSED_ARG(err);
  LWIP_UNUSED_ARG(arg);
  LWIP_DEBUGF(HTTPD_DEBUG, ("http_accept %p / %p\n", (void *)pcb, arg));

  if ((err != ERR_OK) || (pcb == NULL)) {
    return ERR_VAL;
  }

  /* Set priority */
  altcp_setprio(pcb, HTTPD_TCP_PRIO);

  /* Allocate memory for the structure that holds the state of the
     connection - initialized by that function. */
  hs = http_state_alloc();
  if (hs == NULL) {
    LWIP_DEBUGF(HTTPD_DEBUG, ("http_accept: Out of memory, RST\n"));
    return ERR_MEM;
  }
  hs->pcb = pcb;

  /* Tell TCP that this is the structure we wish to be passed for our
     callbacks. */
  altcp_arg(pcb, hs);

  /* Set up the various callback functions */
  altcp_recv(pcb, http_recv);
  altcp_err(pcb, http_err);
  altcp_poll(pcb, http_poll, HTTPD_POLL_INTERVAL);
  altcp_sent(pcb, http_sent);

  return ERR_OK;
}

static void
httpd_init_pcb(struct altcp_pcb *pcb, u16_t port)
{
  err_t err;

  if (pcb) {
    altcp_setprio(pcb, HTTPD_TCP_PRIO);
    /* set SOF_REUSEADDR here to explicitly bind httpd to multiple interfaces */
    err = altcp_bind(pcb, IP_ANY_TYPE, port);
    LWIP_UNUSED_ARG(err); /* in case of LWIP_NOASSERT */
    LWIP_ASSERT("openthread_httpd_init: tcp_bind failed", err == ERR_OK);
    pcb = altcp_listen(pcb);
    LWIP_ASSERT("openthread_httpd_init: tcp_listen failed", pcb != NULL);
    altcp_accept(pcb, http_accept);
  }
}

/**
 * Get the file struct for a 404 error page.
 * Tries some file names and returns NULL if none found.
 *
 * @param uri pointer that receives the actual file name URI
 * @return file struct for the error page or NULL no matching file was found
 */
err_t http_setup_file(void * connection, http_resp_state_t resp_state, char * body)
{
  struct http_state *hs = (struct http_state *) connection;
  char * resp = openthread_rest_construct_resp(resp_state, body);

  if (NULL == resp) {
    return ERR_VAL;
  }

  hs->file_handle.data = resp;
  hs->file_handle.len = strlen(resp) ;
  hs->file_handle.index = strlen(resp);
  hs->file_handle.pextension = NULL;
  hs->file_handle.flags = FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT;

  return ERR_OK;
}

err_t http_init_error_file(void *hs, http_resp_state_t resp_state)
{
  return ERR_OK == http_setup_file(hs, resp_state, NULL);
}
/**
 * @ingroup openthread_httpd_init
 * Initialize the httpd: set up a listening PCB and bind it to the defined port
 */
void openthread_httpd_init(uint16_t port) 
{
  struct altcp_pcb *pcb;

#if HTTPD_USE_MEM_POOL
  LWIP_MEMPOOL_INIT(HTTPD_STATE);
#endif
  LWIP_DEBUGF(HTTPD_DEBUG, ("openthread_httpd_init\n"));

  /* LWIP_ASSERT_CORE_LOCKED(); is checked by tcp_new() */

  pcb = altcp_tcp_new_ip_type(IPADDR_TYPE_ANY);
  LWIP_ASSERT("openthread_httpd_init: tcp_new failed", pcb != NULL);
  httpd_init_pcb(pcb, port);
}

// #if HTTPD_ENABLE_HTTPS
// /**
//  * @ingroup httpd
//  * Initialize the httpd: set up a listening PCB and bind it to the defined port.
//  * Also set up TLS connection handling (HTTPS).
//  */
// void
// httpd_inits(struct altcp_tls_config *conf)
// {
// #if LWIP_ALTCP_TLS
//   struct altcp_pcb *pcb_tls = altcp_tls_new(conf, IPADDR_TYPE_ANY);
//   LWIP_ASSERT("openthread_httpd_init: altcp_tls_new failed", pcb_tls != NULL);
//   httpd_init_pcb(pcb_tls, HTTPD_SERVER_PORT_HTTPS);
// #else /* LWIP_ALTCP_TLS */
//   LWIP_UNUSED_ARG(conf);
// #endif /* LWIP_ALTCP_TLS */
// }
// #endif /* HTTPD_ENABLE_HTTPS */

#endif /* LWIP_TCP && LWIP_CALLBACK_API */
