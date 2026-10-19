#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "address.h"
#include "config.h"
#include "dtls_srtp.h"
#if CONFIG_MBEDTLS_DEBUG
#include "mbedtls/debug.h"
#endif
#include "mbedtls/sha256.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/ssl.h"
#include "ports.h"
#include "socket.h"
#include "utils.h"

int dtls_srtp_udp_send(void* ctx, const uint8_t* buf, size_t len) {
  DtlsSrtp* dtls_srtp = (DtlsSrtp*)ctx;
  UdpSocket* udp_socket = (UdpSocket*)dtls_srtp->user_data;

  int ret = udp_socket_sendto(udp_socket, dtls_srtp->remote_addr, buf, len);

  printf("dtls_srtp_udp_send (%d)", ret);

  return ret;
}

int dtls_srtp_udp_recv(void* ctx, uint8_t* buf, size_t len) {
  DtlsSrtp* dtls_srtp = (DtlsSrtp*)ctx;
  UdpSocket* udp_socket = (UdpSocket*)dtls_srtp->user_data;

  int ret;

  while ((ret = udp_socket_recvfrom(udp_socket, &udp_socket->bind_addr, buf, len)) <= 0) {
    ports_sleep_ms(1);
  }

  printf("dtls_srtp_udp_recv (%d)", ret);

  return ret;
}

static void dtls_srtp_log_mbedtls_error(const char* operation, int ret) {
#if CONFIG_MBEDTLS_DEBUG
  char error[128];

  mbedtls_strerror(ret, error, sizeof(error));
  LOGE("%s returned -0x%.4x (%s)", operation, (unsigned int)-ret, error);
#else
  LOGE("%s returned -0x%.4x", operation, (unsigned int)-ret);
#endif
}

#if CONFIG_DTLS_CLIENT_HELLO_REASSEMBLY_SIZE > 0
#define DTLS_RECORD_HEADER_SIZE 13
#define DTLS_HANDSHAKE_HEADER_SIZE 12

static size_t dtls_srtp_read_u16(const unsigned char* p) {
  return ((size_t)p[0] << 8) | p[1];
}

static size_t dtls_srtp_read_u24(const unsigned char* p) {
  return ((size_t)p[0] << 16) | ((size_t)p[1] << 8) | p[2];
}

static void dtls_srtp_write_u16(unsigned char* p, size_t value) {
  p[0] = (unsigned char)(value >> 8);
  p[1] = (unsigned char)value;
}

static void dtls_srtp_write_u24(unsigned char* p, size_t value) {
  p[0] = (unsigned char)(value >> 16);
  p[1] = (unsigned char)(value >> 8);
  p[2] = (unsigned char)value;
}

static void dtls_srtp_reset_client_hello_reassembly(DtlsSrtp* dtls_srtp) {
  memset(dtls_srtp->client_hello_seen, 0, sizeof(dtls_srtp->client_hello_seen));
  dtls_srtp->client_hello_total_len = 0;
  dtls_srtp->client_hello_received_len = 0;
  dtls_srtp->client_hello_message_seq = 0;
  dtls_srtp->client_hello_header_ready = 0;
}

static int dtls_srtp_client_hello_matches(const DtlsSrtp* dtls_srtp,
                                          size_t total_len, uint16_t message_seq) {
  return dtls_srtp->client_hello_total_len == total_len &&
         dtls_srtp->client_hello_message_seq == message_seq;
}
#endif

int dtls_srtp_reassemble_client_hello(DtlsSrtp* dtls_srtp, unsigned char* buf,
                                      size_t* len, size_t capacity) {
#if CONFIG_DTLS_CLIENT_HELLO_REASSEMBLY_SIZE > 0
  size_t pos = 0;
  int saw_fragment = 0;

  if (dtls_srtp->role != DTLS_SRTP_ROLE_SERVER || buf == NULL || len == NULL) {
    return 0;
  }

  while (pos + DTLS_RECORD_HEADER_SIZE <= *len) {
    unsigned char* record = buf + pos;
    size_t record_len = dtls_srtp_read_u16(record + 11);
    unsigned char* handshake;
    size_t total_len;
    size_t fragment_offset;
    size_t fragment_len;
    uint16_t message_seq;
    size_t i;

    if (record_len > *len - pos - DTLS_RECORD_HEADER_SIZE) {
      dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
      return 0;
    }

    if (record[0] != MBEDTLS_SSL_MSG_HANDSHAKE || record[3] != 0 || record[4] != 0 ||
        record_len < DTLS_HANDSHAKE_HEADER_SIZE) {
      if (dtls_srtp->client_hello_total_len != 0) {
        dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
      }
      return 0;
    }

    handshake = record + DTLS_RECORD_HEADER_SIZE;
    total_len = dtls_srtp_read_u24(handshake + 1);
    message_seq = (uint16_t)dtls_srtp_read_u16(handshake + 4);
    fragment_offset = dtls_srtp_read_u24(handshake + 6);
    fragment_len = dtls_srtp_read_u24(handshake + 9);

    if (handshake[0] != MBEDTLS_SSL_HS_CLIENT_HELLO ||
        fragment_len + DTLS_HANDSHAKE_HEADER_SIZE != record_len ||
        fragment_offset + fragment_len > total_len) {
      if (dtls_srtp->client_hello_total_len != 0) {
        dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
      }
      return 0;
    }

    if (fragment_offset == 0 && fragment_len == total_len &&
        dtls_srtp->client_hello_total_len == 0) {
      return 0;
    }

    if (total_len > CONFIG_DTLS_CLIENT_HELLO_REASSEMBLY_SIZE ||
        total_len + DTLS_RECORD_HEADER_SIZE + DTLS_HANDSHAKE_HEADER_SIZE > capacity) {
      LOGW("DTLS ClientHello too large for reassembly (%lu bytes)", (unsigned long)total_len);
      dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
      return 0;
    }

    if (dtls_srtp->client_hello_total_len != 0 &&
        !dtls_srtp_client_hello_matches(dtls_srtp, total_len, message_seq)) {
      dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
    }

    if (dtls_srtp->client_hello_total_len == 0) {
      dtls_srtp->client_hello_total_len = total_len;
      dtls_srtp->client_hello_message_seq = message_seq;
    }

    if (fragment_offset == 0) {
      memcpy(dtls_srtp->client_hello_reassembly, record, DTLS_RECORD_HEADER_SIZE);
      memcpy(dtls_srtp->client_hello_reassembly + DTLS_RECORD_HEADER_SIZE, handshake,
             DTLS_HANDSHAKE_HEADER_SIZE);
      dtls_srtp->client_hello_header_ready = 1;
    }

    for (i = 0; i < fragment_len; i++) {
      size_t offset = fragment_offset + i;
      unsigned char bit = (unsigned char)(1U << (offset & 7));

      if ((dtls_srtp->client_hello_seen[offset >> 3] & bit) == 0) {
        dtls_srtp->client_hello_seen[offset >> 3] |= bit;
        dtls_srtp->client_hello_received_len++;
      }
      dtls_srtp->client_hello_reassembly[DTLS_RECORD_HEADER_SIZE +
                                         DTLS_HANDSHAKE_HEADER_SIZE + offset] =
          handshake[DTLS_HANDSHAKE_HEADER_SIZE + i];
    }

    saw_fragment = 1;
    pos += DTLS_RECORD_HEADER_SIZE + record_len;
  }

  if (!saw_fragment) {
    return 0;
  }

  if (dtls_srtp->client_hello_header_ready &&
      dtls_srtp->client_hello_received_len == dtls_srtp->client_hello_total_len) {
    size_t full_len = DTLS_RECORD_HEADER_SIZE + DTLS_HANDSHAKE_HEADER_SIZE +
                      dtls_srtp->client_hello_total_len;
    unsigned char* record = dtls_srtp->client_hello_reassembly;
    unsigned char* handshake = record + DTLS_RECORD_HEADER_SIZE;

    dtls_srtp_write_u16(record + 11, DTLS_HANDSHAKE_HEADER_SIZE +
                                           dtls_srtp->client_hello_total_len);
    memset(handshake + 6, 0, 3);
    dtls_srtp_write_u24(handshake + 9, dtls_srtp->client_hello_total_len);
    memcpy(buf, record, full_len);
    *len = full_len;
    LOGI("Reassembled fragmented DTLS ClientHello (%lu bytes)",
         (unsigned long)dtls_srtp->client_hello_total_len);
    dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
    return 1;
  }

  return -1;
#else
  (void)dtls_srtp;
  (void)buf;
  (void)len;
  (void)capacity;
  return 0;
#endif
}

static int dtls_srtp_x509_digest(const mbedtls_x509_crt* crt, char* buf) {
  int i;
  int ret;
  unsigned char digest[32];

#if CONFIG_MBEDTLS_2_X
  ret = mbedtls_sha256_ret(crt->raw.p, crt->raw.len, digest, 0);
#else
  ret = mbedtls_sha256(crt->raw.p, crt->raw.len, digest, 0);
#endif
  if (ret != 0) {
    return ret;
  }

  for (i = 0; i < 32; i++) {
    static const char hex[] = "0123456789ABCDEF";
    buf[3 * i] = hex[digest[i] >> 4];
    buf[3 * i + 1] = hex[digest[i] & 15];
    buf[3 * i + 2] = i == 31 ? '\0' : ':';
  }
  return 0;
}

/* WebRTC identity is the SHA-256 leaf pin from authenticated SDP plus the TLS
 * proof of private-key possession, for both client and server roles. CA/name,
 * dates and legacy self-signature policy are not that identity. Only those
 * flags are exempted; revocation, usage, missing/unknown errors remain fatal.
 * HTTPS signaling still uses normal CA/hostname verification. */
static int dtls_srtp_cert_verify(void* data, mbedtls_x509_crt* crt, int depth, uint32_t* flags) {
  DtlsSrtp* dtls_srtp = (DtlsSrtp*)data;
  int ret;

  if (!dtls_srtp || !crt || !flags) return MBEDTLS_ERR_X509_CERT_VERIFY_FAILED;
  if (depth == 0) {
    ret = dtls_srtp_x509_digest(crt, dtls_srtp->actual_remote_fingerprint);
    if (ret != 0) {
      return ret;
    }
    dtls_srtp->remote_fingerprint_ready = 1;
    if (strcasecmp(dtls_srtp->remote_fingerprint,
                   dtls_srtp->actual_remote_fingerprint) != 0) {
      return MBEDTLS_ERR_X509_CERT_VERIFY_FAILED;
    }
  }

  *flags &= ~(MBEDTLS_X509_BADCERT_NOT_TRUSTED |
              MBEDTLS_X509_BADCERT_CN_MISMATCH |
              MBEDTLS_X509_BADCERT_EXPIRED |
              MBEDTLS_X509_BADCERT_FUTURE |
              MBEDTLS_X509_BADCERT_BAD_MD |
              MBEDTLS_X509_BADCERT_BAD_KEY);
  return 0;
}

static int dtls_srtp_selfsign_cert(DtlsSrtp* dtls_srtp) {
  int ret;

  mbedtls_x509write_cert crt;

  unsigned char* cert_buf = NULL;
#if CONFIG_MBEDTLS_2_X
  mbedtls_mpi serial;
#else
  unsigned char serial[16];
#endif
  const char* pers = "dtls_srtp";

  mbedtls_x509write_crt_init(&crt);
#if CONFIG_MBEDTLS_2_X
  mbedtls_mpi_init(&serial);
#endif

  cert_buf = (unsigned char*)malloc(RSA_KEY_LENGTH * 2);
  if (cert_buf == NULL) {
    LOGE("malloc failed");
    ret = -1;
    goto exit;
  }

  ret = mbedtls_ctr_drbg_seed(&dtls_srtp->ctr_drbg, mbedtls_entropy_func,
                              &dtls_srtp->entropy, (const unsigned char*)pers,
                              strlen(pers));
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("mbedtls_ctr_drbg_seed", ret);
    goto exit;
  }

#if CONFIG_DTLS_USE_ECDSA
  ret = mbedtls_pk_setup(&dtls_srtp->pkey, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
  if (ret == 0) {
    ret = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1,
                              mbedtls_pk_ec(dtls_srtp->pkey),
                              mbedtls_ctr_drbg_random, &dtls_srtp->ctr_drbg);
  }
#else
  ret = mbedtls_pk_setup(&dtls_srtp->pkey, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
  if (ret == 0) {
    ret = mbedtls_rsa_gen_key(mbedtls_pk_rsa(dtls_srtp->pkey),
                              mbedtls_ctr_drbg_random, &dtls_srtp->ctr_drbg,
                              RSA_KEY_LENGTH, 65537);
  }
#endif
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("certificate key generation", ret);
    goto exit;
  }

  mbedtls_x509write_crt_set_version(&crt, MBEDTLS_X509_CRT_VERSION_3);

  mbedtls_x509write_crt_set_md_alg(&crt, MBEDTLS_MD_SHA256);

  mbedtls_x509write_crt_set_subject_key(&crt, &dtls_srtp->pkey);

  mbedtls_x509write_crt_set_issuer_key(&crt, &dtls_srtp->pkey);

  ret = mbedtls_x509write_crt_set_subject_name(&crt, "CN=dtls_srtp");
  if (ret == 0) {
    ret = mbedtls_x509write_crt_set_issuer_name(&crt, "CN=dtls_srtp");
  }
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("certificate name setup", ret);
    goto exit;
  }

#if CONFIG_MBEDTLS_2_X
  ret = mbedtls_mpi_fill_random(&serial, 16, mbedtls_ctr_drbg_random,
                                &dtls_srtp->ctr_drbg);
  if (ret == 0) {
    ret = mbedtls_x509write_crt_set_serial(&crt, &serial);
  }
#else
  ret = mbedtls_ctr_drbg_random(&dtls_srtp->ctr_drbg, serial, sizeof(serial));
  if (ret == 0) {
    ret = mbedtls_x509write_crt_set_serial_raw(&crt, serial, sizeof(serial));
  }
#endif
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("certificate serial setup", ret);
    goto exit;
  }

  ret = mbedtls_x509write_crt_set_validity(&crt, "20200101000000", "20400101000000");
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("certificate validity setup", ret);
    goto exit;
  }

  ret = mbedtls_x509write_crt_pem(&crt, cert_buf, 2 * RSA_KEY_LENGTH, mbedtls_ctr_drbg_random, &dtls_srtp->ctr_drbg);

  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("mbedtls_x509write_crt_pem", ret);
    goto exit;
  }

  ret = mbedtls_x509_crt_parse(&dtls_srtp->cert, cert_buf, 2 * RSA_KEY_LENGTH);
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("mbedtls_x509_crt_parse", ret);
  }

exit:
  mbedtls_x509write_crt_free(&crt);
#if CONFIG_MBEDTLS_2_X
  mbedtls_mpi_free(&serial);
#endif

  free(cert_buf);

  return ret;
}
#if CONFIG_MBEDTLS_DEBUG
static void dtls_srtp_debug(void* ctx, int level, const char* file, int line, const char* str) {
  printf("%s:%04d: %s", file, line, str);
}
#endif

int dtls_srtp_init(DtlsSrtp* dtls_srtp, DtlsSrtpRole role, void* user_data) {
  int ret;
  static const mbedtls_ssl_srtp_profile default_profiles[] = {
      MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80,
      MBEDTLS_TLS_SRTP_UNSET};

  dtls_srtp->role = role;
  dtls_srtp->state = DTLS_SRTP_STATE_INIT;
  dtls_srtp->key_derivation_result = -1;
  dtls_srtp->srtp_in = NULL;
  dtls_srtp->srtp_out = NULL;
  memset(&dtls_srtp->timer, 0, sizeof(dtls_srtp->timer));
  dtls_srtp->actual_remote_fingerprint[0] = '\0';
  dtls_srtp->remote_fingerprint_ready = 0;
  dtls_srtp->user_data = user_data;
  dtls_srtp->udp_send = dtls_srtp_udp_send;
  dtls_srtp->udp_recv = dtls_srtp_udp_recv;
#if CONFIG_DTLS_CLIENT_HELLO_REASSEMBLY_SIZE > 0
  dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
#endif

  mbedtls_ssl_config_init(&dtls_srtp->conf);
  mbedtls_ssl_init(&dtls_srtp->ssl);
  mbedtls_ssl_cookie_init(&dtls_srtp->cookie_ctx);

  mbedtls_x509_crt_init(&dtls_srtp->cert);
  mbedtls_pk_init(&dtls_srtp->pkey);
  mbedtls_entropy_init(&dtls_srtp->entropy);
  mbedtls_ctr_drbg_init(&dtls_srtp->ctr_drbg);
  ret = dtls_srtp_selfsign_cert(dtls_srtp);
  if (ret < 0) {
    dtls_srtp_log_mbedtls_error("dtls_srtp_selfsign_cert", ret);
    return ret;
  }

  ret = mbedtls_ssl_config_defaults(&dtls_srtp->conf,
                                    dtls_srtp->role == DTLS_SRTP_ROLE_SERVER ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("mbedtls_ssl_config_defaults", ret);
    return ret;
  }

  if (CONFIG_DTLS_USE_COOKIES) {
    ret = mbedtls_ssl_cookie_setup(&dtls_srtp->cookie_ctx, mbedtls_ctr_drbg_random, &dtls_srtp->ctr_drbg);
    if (ret != 0) {
      dtls_srtp_log_mbedtls_error("mbedtls_ssl_cookie_setup", ret);
      return ret;
    }
    mbedtls_ssl_conf_dtls_cookies(&dtls_srtp->conf, mbedtls_ssl_cookie_write,
                                  mbedtls_ssl_cookie_check, &dtls_srtp->cookie_ctx);
  }

#if CONFIG_MBEDTLS_DEBUG
  mbedtls_debug_set_threshold(3);
  mbedtls_ssl_conf_dbg(&dtls_srtp->conf, dtls_srtp_debug, NULL);
#endif
  mbedtls_ssl_conf_verify(&dtls_srtp->conf, dtls_srtp_cert_verify, dtls_srtp);
  mbedtls_ssl_conf_authmode(&dtls_srtp->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_ca_chain(&dtls_srtp->conf, &dtls_srtp->cert, NULL);
  ret = mbedtls_ssl_conf_own_cert(&dtls_srtp->conf, &dtls_srtp->cert, &dtls_srtp->pkey);
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("mbedtls_ssl_conf_own_cert", ret);
    return ret;
  }
  mbedtls_ssl_conf_rng(&dtls_srtp->conf, mbedtls_ctr_drbg_random, &dtls_srtp->ctr_drbg);
  mbedtls_ssl_conf_read_timeout(&dtls_srtp->conf, 1000);

  ret = dtls_srtp_x509_digest(&dtls_srtp->cert, dtls_srtp->local_fingerprint);
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("certificate fingerprint", ret);
    return ret;
  }

  printf("local fingerprint: %s\r\n", dtls_srtp->local_fingerprint);

  mbedtls_ssl_conf_dtls_srtp_protection_profiles(&dtls_srtp->conf, default_profiles);

  mbedtls_ssl_conf_srtp_mki_value_supported(&dtls_srtp->conf, MBEDTLS_SSL_DTLS_SRTP_MKI_UNSUPPORTED);

  mbedtls_ssl_conf_cert_req_ca_list(&dtls_srtp->conf, MBEDTLS_SSL_CERT_REQ_CA_LIST_DISABLED);

  ret = mbedtls_ssl_setup(&dtls_srtp->ssl, &dtls_srtp->conf);
  if (ret != 0) {
    dtls_srtp_log_mbedtls_error("mbedtls_ssl_setup", ret);
    return ret;
  }

  /* Identity is the SDP fingerprint, not a DNS name. Explicitly select this
   * mode: mbedTLS 3.6 rejects REQUIRED client verification without this call. */
  ret = mbedtls_ssl_set_hostname(&dtls_srtp->ssl, NULL);
  if (ret != 0) {
    return ret;
  }

  return 0;
}

void dtls_srtp_deinit(DtlsSrtp* dtls_srtp) {
  mbedtls_ssl_free(&dtls_srtp->ssl);
  mbedtls_ssl_config_free(&dtls_srtp->conf);

  mbedtls_x509_crt_free(&dtls_srtp->cert);
  mbedtls_pk_free(&dtls_srtp->pkey);
  mbedtls_entropy_free(&dtls_srtp->entropy);
  mbedtls_ctr_drbg_free(&dtls_srtp->ctr_drbg);

  mbedtls_ssl_cookie_free(&dtls_srtp->cookie_ctx);

  if (dtls_srtp->srtp_in) {
    srtp_dealloc(dtls_srtp->srtp_in);
    dtls_srtp->srtp_in = NULL;
  }
  if (dtls_srtp->srtp_out) {
    srtp_dealloc(dtls_srtp->srtp_out);
    dtls_srtp->srtp_out = NULL;
  }
}

static int dtls_srtp_key_derivation(DtlsSrtp* dtls_srtp, const unsigned char* master_secret, size_t secret_len, const unsigned char* randbytes, size_t randbytes_len, mbedtls_tls_prf_types tls_prf_type) {
  int ret;
  const char* dtls_srtp_label = "EXTRACTOR-dtls_srtp";
  uint8_t key_material[DTLS_SRTP_KEY_MATERIAL_LENGTH];
  const uint8_t* client_key;
  const uint8_t* server_key;
  const uint8_t* client_salt;
  const uint8_t* server_salt;
  const uint8_t* local_key;
  const uint8_t* local_salt;
  const uint8_t* remote_key;
  const uint8_t* remote_salt;

  if (dtls_srtp == NULL || master_secret == NULL || randbytes == NULL) {
    return -1;
  }

  // Export keying material
  if ((ret = mbedtls_ssl_tls_prf(tls_prf_type, master_secret, secret_len, dtls_srtp_label,
                                 randbytes, randbytes_len, key_material, sizeof(key_material))) != 0) {
    LOGE("mbedtls_ssl_tls_prf failed(%d)", ret);
    return ret;
  }

#if 0
  int i, j;
  printf("    DTLS-SRTP key material is:");
  for (j = 0; j < sizeof(key_material); j++) {
    if (j % 8 == 0) {
      printf("\n    ");
    }
    printf("%02x ", key_material[j]);
  }
  printf("\n");

  /* produce a less readable output used to perform automatic checks
   * - compare client and server output
   * - interop test with openssl which client produces this kind of output
   */
  printf("    Keying material: ");
  for (j = 0; j < sizeof(key_material); j++) {
    printf("%02X", key_material[j]);
  }
  printf("\n");
#endif

  client_key = key_material;
  server_key = client_key + SRTP_MASTER_KEY_LENGTH;
  client_salt = server_key + SRTP_MASTER_KEY_LENGTH;
  server_salt = client_salt + SRTP_MASTER_SALT_LENGTH;

  if (dtls_srtp->role == DTLS_SRTP_ROLE_CLIENT) {
    local_key = client_key;
    local_salt = client_salt;
    remote_key = server_key;
    remote_salt = server_salt;
  } else {
    local_key = server_key;
    local_salt = server_salt;
    remote_key = client_key;
    remote_salt = client_salt;
  }

  // derive inbounds keys

  memset(&dtls_srtp->remote_policy, 0, sizeof(dtls_srtp->remote_policy));

  srtp_crypto_policy_set_rtp_default(&dtls_srtp->remote_policy.rtp);
  srtp_crypto_policy_set_rtcp_default(&dtls_srtp->remote_policy.rtcp);

  memcpy(dtls_srtp->remote_policy_key, remote_key, SRTP_MASTER_KEY_LENGTH);
  memcpy(dtls_srtp->remote_policy_key + SRTP_MASTER_KEY_LENGTH, remote_salt,
         SRTP_MASTER_SALT_LENGTH);

  dtls_srtp->remote_policy.ssrc.type = ssrc_any_inbound;
  dtls_srtp->remote_policy.key = dtls_srtp->remote_policy_key;
  dtls_srtp->remote_policy.next = NULL;

  if (srtp_create(&dtls_srtp->srtp_in, &dtls_srtp->remote_policy) != srtp_err_status_ok) {
    mbedtls_platform_zeroize(key_material, sizeof(key_material));
    LOGE("Error creating inbound SRTP session");
    return -1;
  }

  LOGI("Created inbound SRTP session");

  // derive outbounds keys
  memset(&dtls_srtp->local_policy, 0, sizeof(dtls_srtp->local_policy));

  srtp_crypto_policy_set_rtp_default(&dtls_srtp->local_policy.rtp);
  srtp_crypto_policy_set_rtcp_default(&dtls_srtp->local_policy.rtcp);

  memcpy(dtls_srtp->local_policy_key, local_key, SRTP_MASTER_KEY_LENGTH);
  memcpy(dtls_srtp->local_policy_key + SRTP_MASTER_KEY_LENGTH, local_salt,
         SRTP_MASTER_SALT_LENGTH);

  dtls_srtp->local_policy.ssrc.type = ssrc_any_outbound;
  dtls_srtp->local_policy.key = dtls_srtp->local_policy_key;
  dtls_srtp->local_policy.next = NULL;

  if (srtp_create(&dtls_srtp->srtp_out, &dtls_srtp->local_policy) != srtp_err_status_ok) {
    LOGE("Error creating outbound SRTP session");
    srtp_dealloc(dtls_srtp->srtp_in);
    dtls_srtp->srtp_in = NULL;
    mbedtls_platform_zeroize(key_material, sizeof(key_material));
    return -1;
  }

  mbedtls_platform_zeroize(key_material, sizeof(key_material));
  LOGI("Created outbound SRTP session");
  return 0;
}

#if CONFIG_MBEDTLS_2_X
static int dtls_srtp_key_derivation_cb(void* context,
                                       const unsigned char* ms,
                                       const unsigned char* kb,
                                       size_t maclen,
                                       size_t keylen,
                                       size_t ivlen,
                                       const unsigned char client_random[32],
                                       const unsigned char server_random[32],
                                       mbedtls_tls_prf_types tls_prf_type) {
#else
static void dtls_srtp_key_derivation_cb(void* context,
                                        mbedtls_ssl_key_export_type secret_type,
                                        const unsigned char* secret,
                                        size_t secret_len,
                                        const unsigned char client_random[32],
                                        const unsigned char server_random[32],
                                        mbedtls_tls_prf_types tls_prf_type) {
#endif
  DtlsSrtp* dtls_srtp = (DtlsSrtp*)context;

  unsigned char master_secret[48];
  unsigned char randbytes[64];

  memcpy(randbytes, client_random, 32);
  memcpy(randbytes + 32, server_random, 32);

#if CONFIG_MBEDTLS_2_X
  (void)kb;
  (void)maclen;
  (void)keylen;
  (void)ivlen;
  memcpy(master_secret, ms, sizeof(master_secret));
  int ret = dtls_srtp_key_derivation(dtls_srtp, master_secret,
                                     sizeof(master_secret), randbytes,
                                     sizeof(randbytes), tls_prf_type);
  dtls_srtp->key_derivation_result = ret;
  mbedtls_platform_zeroize(master_secret, sizeof(master_secret));
  return ret;
#else
  if (secret_type != MBEDTLS_SSL_KEY_EXPORT_TLS12_MASTER_SECRET || secret == NULL ||
      secret_len != sizeof(master_secret)) {
    return;
  }
  memcpy(master_secret, secret, sizeof(master_secret));
  dtls_srtp->key_derivation_result = dtls_srtp_key_derivation(
      dtls_srtp, master_secret, sizeof(master_secret), randbytes,
      sizeof(randbytes), tls_prf_type);
  if (dtls_srtp->key_derivation_result != 0) {
    LOGE("Failed to derive DTLS-SRTP keys");
  }
  mbedtls_platform_zeroize(master_secret, sizeof(master_secret));
#endif
}

int dtls_srtp_set_role(DtlsSrtp* dtls_srtp, DtlsSrtpRole role) {
  if (!dtls_srtp || dtls_srtp->state != DTLS_SRTP_STATE_INIT ||
      (role != DTLS_SRTP_ROLE_CLIENT && role != DTLS_SRTP_ROLE_SERVER)) {
    return -1;
  }
  if (dtls_srtp->role == role) {
    return 0;
  }
  dtls_srtp->role = role;
  mbedtls_ssl_conf_endpoint(&dtls_srtp->conf,
      role == DTLS_SRTP_ROLE_SERVER ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT);
  return mbedtls_ssl_session_reset(&dtls_srtp->ssl);
}

/* Advance one handshake flight, yielding on socket pressure. The owner can
 * still service stop/reconnect requests while the other endpoint is silent. */
int dtls_srtp_handshake(DtlsSrtp* dtls_srtp, Address* addr) {
  int ret;
  if (!dtls_srtp || (dtls_srtp->role == DTLS_SRTP_ROLE_SERVER &&
                      CONFIG_DTLS_USE_COOKIES && !addr)) {
    return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
  }
  dtls_srtp->remote_addr = addr;
  if (dtls_srtp->state == DTLS_SRTP_STATE_CONNECTED) {
    return 0;
  }
  if (dtls_srtp->state == DTLS_SRTP_STATE_INIT) {
    dtls_srtp->actual_remote_fingerprint[0] = '\0';
    dtls_srtp->remote_fingerprint_ready = 0;
    mbedtls_ssl_set_timer_cb(&dtls_srtp->ssl, &dtls_srtp->timer,
                             mbedtls_timing_set_delay, mbedtls_timing_get_delay);
#if CONFIG_MBEDTLS_2_X
    mbedtls_ssl_conf_export_keys_ext_cb(&dtls_srtp->conf, dtls_srtp_key_derivation_cb, dtls_srtp);
#else
    mbedtls_ssl_set_export_keys_cb(&dtls_srtp->ssl, dtls_srtp_key_derivation_cb, dtls_srtp);
#endif
    mbedtls_ssl_set_bio(&dtls_srtp->ssl, dtls_srtp,
                        dtls_srtp->udp_send, dtls_srtp->udp_recv, NULL);
    if (dtls_srtp->role == DTLS_SRTP_ROLE_SERVER && CONFIG_DTLS_USE_COOKIES) {
      const unsigned char* client_id = (const unsigned char*)addr;
      size_t client_id_len = sizeof(*addr);
      ret = mbedtls_ssl_set_client_transport_id(&dtls_srtp->ssl, client_id, client_id_len);
      if (ret != 0) {
        return ret;
      }
    }
    dtls_srtp->state = DTLS_SRTP_STATE_HANDSHAKE;
  }
  ret = mbedtls_ssl_handshake(&dtls_srtp->ssl);
  if (ret == MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED) {
    dtls_srtp_reset_session(dtls_srtp);
    return MBEDTLS_ERR_SSL_WANT_READ;
  }
  if (ret != 0) {
    return ret;
  }
  if (dtls_srtp->key_derivation_result != 0 ||
      !dtls_srtp->srtp_in || !dtls_srtp->srtp_out) {
    LOGE("DTLS completed without exporting SRTP keys");
    return -1;
  }

  if (!dtls_srtp->remote_fingerprint_ready) {
    const mbedtls_x509_crt* remote_crt =
        mbedtls_ssl_get_peer_cert(&dtls_srtp->ssl);

    if (remote_crt == NULL) {
      LOGE("DTLS completed without a peer certificate fingerprint");
      return -1;
    }

    ret = dtls_srtp_x509_digest(remote_crt, dtls_srtp->actual_remote_fingerprint);
    if (ret != 0) {
      dtls_srtp_log_mbedtls_error("peer certificate fingerprint", ret);
      return ret;
    }
    dtls_srtp->remote_fingerprint_ready = 1;
  }

  if (strncasecmp(dtls_srtp->remote_fingerprint,
              dtls_srtp->actual_remote_fingerprint,
              DTLS_SRTP_FINGERPRINT_LENGTH) != 0) {
    LOGE("Actual and Expected Fingerprint mismatch: %s %s",
         dtls_srtp->remote_fingerprint,
         dtls_srtp->actual_remote_fingerprint);
    return -1;
  }

  dtls_srtp->state = DTLS_SRTP_STATE_CONNECTED;
  LOGI("DTLS %s handshake and fingerprint verified",
       dtls_srtp->role == DTLS_SRTP_ROLE_CLIENT ? "client" : "server");
  return 0;
}

void dtls_srtp_reset_session(DtlsSrtp* dtls_srtp) {
  if (dtls_srtp->srtp_in) {
    srtp_dealloc(dtls_srtp->srtp_in);
    dtls_srtp->srtp_in = NULL;
  }
  if (dtls_srtp->srtp_out) {
    srtp_dealloc(dtls_srtp->srtp_out);
    dtls_srtp->srtp_out = NULL;
  }
  mbedtls_ssl_session_reset(&dtls_srtp->ssl);
  memset(&dtls_srtp->timer, 0, sizeof(dtls_srtp->timer));
  dtls_srtp->key_derivation_result = -1;
  dtls_srtp->state = DTLS_SRTP_STATE_INIT;
  dtls_srtp->actual_remote_fingerprint[0] = '\0';
  dtls_srtp->remote_fingerprint_ready = 0;
#if CONFIG_DTLS_CLIENT_HELLO_REASSEMBLY_SIZE > 0
  dtls_srtp_reset_client_hello_reassembly(dtls_srtp);
#endif
}

int dtls_srtp_write(DtlsSrtp* dtls_srtp, const unsigned char* buf, size_t len) {
  return mbedtls_ssl_write(&dtls_srtp->ssl, buf, len);
}

int dtls_srtp_read(DtlsSrtp* dtls_srtp, unsigned char* buf, size_t len) {
  memset(buf, 0, len);
  return mbedtls_ssl_read(&dtls_srtp->ssl, buf, len);
}

int dtls_srtp_probe(uint8_t* buf) {
  if (buf == NULL)
    return 0;

  //printf("DTLS content type: %d", buf[0]);
  // only handle application data
  return (buf[0] == 0x17);
}

int dtls_srtp_decrypt_rtp_packet(DtlsSrtp* dtls_srtp, uint8_t* packet, int* bytes) {
  return srtp_unprotect(dtls_srtp->srtp_in, packet, bytes);
}

int dtls_srtp_decrypt_rtcp_packet(DtlsSrtp* dtls_srtp, uint8_t* packet, int* bytes) {
  return srtp_unprotect_rtcp(dtls_srtp->srtp_in, packet, bytes);
}

int dtls_srtp_encrypt_rtp_packet(DtlsSrtp* dtls_srtp, uint8_t* packet, int* bytes) {
  return srtp_protect(dtls_srtp->srtp_out, packet, bytes);
}

int dtls_srtp_encrypt_rtcp_packet(DtlsSrtp* dtls_srtp, uint8_t* packet, int* bytes) {
  return srtp_protect_rtcp(dtls_srtp->srtp_out, packet, bytes);
}
