#ifndef __CLI_AL__
#define __CLI_AL__

typedef struct getopt_env {
    char *optarg;    /*!< if the option accepts parameters, then optarg point to the option parameter*/
    int optind;      /*!< current index of argv*/
    int opterr;      /*!< non-zero enable error message output, while 0,no error message output*/
    int optopt;      /*!< contain unrecognized option character*/
    int __optpos;
} getopt_env_t;
void utils_al_parse_number_adv(const char *str, char sep, uint8_t *buf, int buflen, int base, int *count);
void utils_al_parse_number(const char *str, char sep, uint8_t *buf, int buflen, int base);
int utils_al_getopt_init(getopt_env_t *env, int opterr);
int utils_al_getopt(getopt_env_t *env, int argc, char * const argv[], const char *optstring);
char *utils_al_bin2hex(char *dst, const void *src, size_t count);

#endif
