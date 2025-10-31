/*
 * Copyright (c) 2025, the respective contributors, as shown by the AUTHORS file.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <regex.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "queue.h"
#include "tree.h"

#define DUP_MATCH_STRING(idx)                                                  \
  strndup(p + m[idx].rm_so, m[idx].rm_eo - m[idx].rm_so)

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

extern uint32_t utils_crc32(const void *dataIn, uint32_t len);
extern int base64decode(const uint8_t *inputData, uint32_t inputLength,
                        uint32_t outputLenMax, uint8_t *decodedData,
                        uint32_t *outputLength);

struct coredump_header {
  uint8_t magic[4];
  uint32_t version;
  uint32_t count;
} __packed;

struct coredump_section {
  struct {
    uint64_t addr;
    uint64_t length;
  } h;
  // ------------
  RB_ENTRY(coredump_section) entry;
  uint8_t *payload;
} __packed;

struct coredump {
  STAILQ_ENTRY(coredump) entry;
  RB_HEAD(section_tree, coredump_section) root;
  unsigned int count;
  size_t total_size;
};

STAILQ_HEAD(, coredump)
coredump_list_head = STAILQ_HEAD_INITIALIZER(coredump_list_head);

//////////////////////////////////////////////////////////////////////////////
static int intcmp(struct coredump_section *e1, struct coredump_section *e2) {
  return (e1->h.addr < e2->h.addr ? -1 : e1->h.addr > e2->h.addr);
}

RB_GENERATE(section_tree, coredump_section, entry, intcmp)

int main(int argc, char *argv[]) {
  int ret;

  if (argc < 2) {
    printf("usage: bugkiller_dump <coredump log file>\n");
    exit(-1);
  }

  regex_t *regex_dump_hdr = malloc(sizeof(regex_t));
  char *pattern_dump_hdr = "\\-\\+\\-\\+\\-\\+\\- BFLB COREDUMP v([0-9\\.]+) "
                           "\\+\\-\\+\\-\\+\\-\\+";

  ret = regcomp(regex_dump_hdr, pattern_dump_hdr, REG_EXTENDED | REG_NEWLINE);
  if (ret) {
    exit(-1);
  }

  regex_t *regex_sec_hdr = malloc(sizeof(regex_t));
  char *pattern_sec_hdr =
      "\\-\\-\\-\\-\\-\\- DATA BEGIN (.+) \\-\\-\\-\\-\\-\\-";
  ret = regcomp(regex_sec_hdr, pattern_sec_hdr, REG_EXTENDED | REG_NEWLINE);
  if (ret) {
    exit(-1);
  }

  regex_t *regex_sec_end = malloc(sizeof(regex_t));
  char *pattern_sec_end = "\\-\\-\\-\\-\\-\\- END (.+) \\-\\-\\-\\-\\-\\-";
  ret = regcomp(regex_sec_end, pattern_sec_end, REG_EXTENDED | REG_NEWLINE);
  if (ret) {
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    perror("open");
    exit(-1);
  }

  struct stat statbuf;

  ret = fstat(fd, &statbuf);
  if (ret) {
    perror("ftruncate");
    exit(-1);
  }

  void *content = mmap(NULL, statbuf.st_size + 1, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE, fd, 0);
  if (content == (void *)-1) {
    perror("mmap");
    exit(-1);
  }
  char *log_data = content;
  char *next_line;

  // process convert 00 to 20 <space>
  for (size_t i = 0; i < statbuf.st_size; i++) {
    if (log_data[i] == 0x0) {
      log_data[i] = 0x20;
    }
  }

  log_data[statbuf.st_size] = 0; // null end

  ///////////////////////////////////////////////////////////////////////////

  enum {
    PARSER_TO_INIT = 0,
    PARSER_INIT,

    PARSER_TO_SECTION,
    PARSER_SECTION,

    PARSER_TO_DUMP,
    PARSER_DUMP,
  } parser_state = 0;

  int no_match_cnt = 0;
  uint32_t sec_addr, sec_len; // section addr and len
  char splitter = -1;
  int dump_line_cnt = 0;

  int base64_len; // 每次section dump的内容
  char *base64_content = NULL, *base64_content_pos;

  uint8_t *sec_data = NULL; // section binary data
  struct coredump *curr_coredump = NULL;

  char *line = strtok_r(log_data, "\r\n", &next_line);
  /* foreach each line */
  for (int i = 0; i < statbuf.st_size && line != NULL;) {
    if (strlen(line) == 0) {
      goto _cont;
    }

    switch (parser_state) {
    case PARSER_TO_INIT:
      parser_state = PARSER_INIT;
      splitter = -1;

    case PARSER_INIT:
      do {
        char *p = line;
        regmatch_t m[2];

        while (1) {
          ret = regexec(regex_dump_hdr, p, 2, m, 0);
          if (ret) {
            if (ret != REG_NOMATCH) {
              printf("%s:%d\n", __func__, __LINE__);
            }
            break;
          }

          // found coredump header
          char *match_str = DUP_MATCH_STRING(1);
          if (strcmp("0.0.1", match_str) == 0) {
            printf("found valid coredump.\n");
            curr_coredump = malloc(sizeof(struct coredump));
            if (curr_coredump == NULL) {
              perror("malloc");
              exit(-1);
            }
            RB_INIT(&curr_coredump->root);
            curr_coredump->count = 0;
            curr_coredump->total_size = 0;
            STAILQ_INSERT_TAIL(&coredump_list_head, curr_coredump, entry);
            parser_state = PARSER_TO_SECTION;
          }
          free(match_str);

          if (m[0].rm_so > 0) {
            splitter = p[m[0].rm_so - 1];
            printf("splitter:%c %X\n", splitter, splitter);
          }

          p += m[0].rm_eo;
        }

      } while (0);
      break;

    case PARSER_TO_SECTION:
      no_match_cnt = 0;
      parser_state = PARSER_SECTION;
      if (base64_content != NULL) {
        free(base64_content);
        base64_content = NULL;
      }
      if (sec_data != NULL) {
        free(sec_data);
        sec_data = NULL;
      }

    case PARSER_SECTION:
      do {
        char *p = line;
        regmatch_t m[2];

        memset(&m, 0, sizeof(m));

        while (1) {
          ret = regexec(regex_sec_hdr, p, 2, m, 0);
          if (ret == REG_NOMATCH) {
            if (no_match_cnt++ > 3) {
              parser_state = PARSER_TO_INIT;
            }
            break;
          } else if (ret == 0) {
            char *match_str = DUP_MATCH_STRING(1);
            char *next_attr;

            char *attr = strtok_r(match_str, "@", &next_attr);
            sec_addr = strtol(attr, NULL, 16);

            attr = strtok_r(NULL, "@", &next_attr);
            sec_len = strtol(attr, NULL, 16);

            // 数据长度被base64编码后的总长度
            base64_len = ((sec_len + 2) / 3) * 4;

            dump_line_cnt =
                (base64_len + 103) / 104; // 计算本次dump section的行数

            assert(base64_content == NULL);
            base64_content = malloc(base64_len);
            if (base64_content == NULL) {
              perror("malloc");
              free(match_str);
              exit(-1);
            }
            base64_content_pos = base64_content;

            assert(sec_data == NULL);
            sec_data = malloc(sec_len);
            if (sec_data == NULL) {
              perror("malloc");
              free(match_str);
              free(base64_content);
              exit(-1);
            }

#if 1
            printf("addr: %x ", sec_addr);
            printf("length: %x line:%d ", sec_len, dump_line_cnt);

            attr = strtok_r(NULL, "@", &next_attr);
            printf("name: %s\n", attr);
#endif

            parser_state = PARSER_TO_DUMP;
          } else {
            abort();
          }

          p += m[0].rm_eo;
        }
      } while (0);
      break;

    case PARSER_TO_DUMP:
      parser_state = PARSER_DUMP;
    case PARSER_DUMP:
      do {
        char *p = line;
        regmatch_t m[2];

        ret = regexec(regex_sec_end, p, 2, m, 0);
        if (ret == REG_NOMATCH) {
          char *dump_dat = NULL;

          if (dump_line_cnt-- == 0) { // dump超过了实际的长度
            printf("bad coredump format!\n");
            parser_state = PARSER_TO_SECTION;
            break;
          }

          // 过滤前面的时间戳等数据
          if (splitter != (char)-1) {
            dump_dat = strrchr(p, splitter);
          }

          if (dump_dat) {
            dump_dat = dump_dat + 1;
          } else {
            dump_dat = p;
          }
          char *pos = strchr(dump_dat, '\r');
          if (pos != NULL) {
            *pos = '\0';
          }
          pos = strchr(dump_dat, '\n');
          if (pos != NULL) {
            *pos = '\0';
          }

          int len = strlen(dump_dat);
          // 检查base64是否过长
          if (((base64_content_pos - base64_content) + len) > base64_len) {
            printf("section base64 too long\n");
            parser_state = PARSER_TO_SECTION;
            break;
          }

          memcpy(base64_content_pos, dump_dat, len);
          base64_content_pos += len;
          break;
        } else if (ret == 0) {
          if (dump_line_cnt > 0) { // 没有dump足够的行数便遇到了结尾
            printf("bad coredump section!\n");
            parser_state = PARSER_TO_SECTION;
            break;
          }

          // printf("base64: %s\n", base64_content);

          uint32_t sec_data_decode_len, crc32_cal;

          base64decode((uint8_t *)base64_content, base64_len, sec_len, sec_data,
                       &sec_data_decode_len);
          free(base64_content);
          base64_content = NULL;

          if (sec_data_decode_len != sec_len) {
            printf("bad decode data\n");
            parser_state = PARSER_TO_SECTION;
            break;
          }

          crc32_cal = utils_crc32(sec_data, sec_data_decode_len);

          uint32_t crc32_decode, crc32_decode_len;

          char *match_str = DUP_MATCH_STRING(1);
          base64decode((uint8_t *)match_str, strlen(match_str),
                       sizeof(crc32_decode), (uint8_t *)&crc32_decode,
                       &crc32_decode_len);
          free(match_str);

          if (crc32_cal != crc32_decode) {
            printf("crc32 mismatch!\n");
            parser_state = PARSER_TO_SECTION;
          }

          // section ok
          struct coredump_section *cur_sec =
              malloc(sizeof(struct coredump_section));
          struct coredump_section *origin_sec = NULL;
          if (cur_sec == NULL) {
            perror("malloc");
            exit(-1);
          }

          cur_sec->h.addr = sec_addr;
          cur_sec->h.length = sec_len;
          cur_sec->payload = sec_data;
          sec_data = NULL;
          origin_sec = RB_INSERT(section_tree, &curr_coredump->root, cur_sec);
          if (origin_sec == NULL) {
            curr_coredump->count++;
            curr_coredump->total_size += sec_len;
          } else {
            printf("Duplicate Address: 0x%x\n", sec_addr);
            free(cur_sec->payload);
            free(cur_sec);
          }

          parser_state = PARSER_TO_SECTION;
        } else {
          printf("%s:%d\n", __func__, __LINE__);
        }
      } while (0);
      break;

    default:
      abort();
      break;
    }

  _cont:
    i += strlen(line);
    line = strtok_r(NULL, "\r\n", &next_line);
  }

  regfree(regex_dump_hdr);
  regfree(regex_sec_hdr);
  regfree(regex_sec_end);

  munmap(content, statbuf.st_size);
  close(fd);

  char filename[64];
  int i = 0, j;

  // dump coredump
  struct coredump *np;
  STAILQ_FOREACH(np, &coredump_list_head, entry) {
    struct coredump_header *cd_header;
    struct coredump_section *cd_section;
    uint8_t *cd_payload;

    struct coredump_section *nnp;
    snprintf(filename, sizeof(filename), "coredump-%d.bin", i++);

    fd = open(filename, O_CREAT | O_RDWR, 0655);
    if (fd < 0) {
      perror("open");
      exit(-1);
    }

    printf("write to coredump: %s\n", filename);

    np->total_size +=
        sizeof(struct coredump_header) + np->count * sizeof(nnp->h);

    ret = ftruncate(fd, np->total_size);
    if (ret) {
      perror("ftruncate");
      exit(-1);
    }

    content =
        mmap(NULL, np->total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (content == (void *)-1) {
      perror("mmap");
      exit(-1);
    }

    cd_header = (struct coredump_header *)content;
    cd_header->magic[0] = 'B';
    cd_header->magic[1] = 'L';
    cd_header->magic[2] = 'C';
    cd_header->magic[3] = 'D';
    cd_header->version = 1;
    cd_header->count = np->count;

    cd_section = (struct coredump_section *)(cd_header + 1);
    cd_payload =
        (uint8_t *)((uint8_t *)cd_section + cd_header->count * sizeof(nnp->h));

    j = 0;
    RB_FOREACH(nnp, section_tree, &np->root) {
      memcpy((uint8_t *)cd_section + j * sizeof(nnp->h), &nnp->h,
             sizeof(nnp->h));

      memcpy(cd_payload, nnp->payload, nnp->h.length);
      cd_payload += nnp->h.length;

      j++;
    }

    munmap(content, np->total_size);
    close(fd);
  }

  return 0;
}
