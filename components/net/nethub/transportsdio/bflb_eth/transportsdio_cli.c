
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "transportsdio.h"
#include <nethub.h>

int cmd_wifieth(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: wifieth <sta|ap>\n");
        return -1;
    }

    if (strcmp(argv[1], "sta") == 0) {
        printf("wifieth: set wifi sta...\n");
        nethub_update_wifichannel(NHIF_TYPE_STA);
    } else if (strcmp(argv[1], "ap") == 0) {
        printf("wifieth: set wifi ap...\n");
        nethub_update_wifichannel(NHIF_TYPE_AP);
    } else {
        printf("Invalid argument: %s\n", argv[1]);
        printf("Usage: wifieth <sta|ap>\n");
        return -1;
    }
}

/* Register shell command */
SHELL_CMD_EXPORT_ALIAS(cmd_wifieth, wifieth, wifieth commands);

