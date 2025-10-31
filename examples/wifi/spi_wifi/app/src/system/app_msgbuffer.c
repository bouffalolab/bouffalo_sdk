
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "shell.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define MAX_BUFFER_SIZE 1024

// Define a global variable to store the handle of the message buffer.
typedef struct _msgbuffer_desc {
    MessageBufferHandle_t msgbuffer;
    uint8_t cache_buffer[MAX_BUFFER_SIZE];
} msgbuffer_desc_t;

static msgbuffer_desc_t *s_tcmb = NULL;

// Initialize the message buffer.
void msgbuffer_init()
{
    if (NULL == s_tcmb) {
        s_tcmb = pvPortMalloc(sizeof(msgbuffer_desc_t));
    }
    // Create a message buffer and specify the buffer size.
    s_tcmb->msgbuffer = xMessageBufferCreate(MAX_BUFFER_SIZE);
    if (s_tcmb->msgbuffer == NULL) {
        printf("Failed to create message buffer\n");
    } else {
        printf("Message buffer initialized\n");
    }
}

// Write data to the message buffer.
void msgbuffer_write(const char *data, size_t data_len)
{
    size_t available_space;
    size_t current_len;
    if (NULL == s_tcmb) {
        printf("write error.\r\n");
        return;
    }

    printf("write data_len:%ld\r\n", data_len);

    available_space = xMessageBufferSpaceAvailable(s_tcmb->msgbuffer);
    printf("msgbuffer used:%d, free:%d(real can write = free - 4)\r\n",
            MAX_BUFFER_SIZE - available_space, available_space);

    // Check if the message buffer has been initialized.
    if (s_tcmb->msgbuffer != NULL) {
        // Write data to the message buffer.
        current_len = xMessageBufferSend(s_tcmb->msgbuffer,
                                        data,
                                        data_len,
                                        0);//portMAX_DELAY
        if (current_len == data_len) {
            printf("Data written to message buffer: %s\n", data);
        } else {
            printf("Failed to write data to msgbuffer, current_len:%d, data_len:%d\r\n",
                    current_len, data_len);
        }
    } else {
        printf("Message buffer not initialized\n");
    }
 
    available_space = xMessageBufferSpaceAvailable(s_tcmb->msgbuffer);
    printf("msgbuffer used:%d, free:%d(real can write = free - 4)\r\n",
            MAX_BUFFER_SIZE - available_space, available_space);
}

// Read data from the message buffer.
void msgbuffer_read()
{
    uint8_t *buffer;
    size_t available_space;

    if (NULL == s_tcmb) {
        printf("write error.\r\n");
        return;
    }

    buffer = s_tcmb->cache_buffer;

    // Check if the message buffer has been initialized.
    if (s_tcmb->msgbuffer != NULL) {
        // Get the available space size in the message buffer.
        available_space = xMessageBufferSpaceAvailable(s_tcmb->msgbuffer);
        printf("msgbuffer used:%d, free:%d(real can write = free - 4)\r\n",
                MAX_BUFFER_SIZE - available_space, available_space);

        // Create a buffer to store the read data.
        // Read data from the message buffer.
        size_t bytes_read = xMessageBufferReceive(s_tcmb->msgbuffer,
                                    buffer,
                                    available_space,
                                    100);//portMAX_DELAY
        if (bytes_read > 0) {
            // Print the read data.
            buffer[bytes_read] = '\0'; // Add a string terminator.
            printf("Data read from message buffer len:%d, buffer: %s\n",
                    bytes_read, buffer);
        } else {
            printf("Failed to read data from message buffer\n");
        }

        available_space = xMessageBufferSpaceAvailable(s_tcmb->msgbuffer);
        printf("msgbuffer used:%d, free:%d(real can write = free - 4)\r\n",
                MAX_BUFFER_SIZE - available_space, available_space);
    } else {
        printf("Message buffer not initialized\n");
    }
}

// Clear the message buffer.
void msgbuffer_cleanup()
{
    if (NULL == s_tcmb) {
        return;
    }
    vMessageBufferDelete(s_tcmb->msgbuffer);
    vPortFree(s_tcmb);
    s_tcmb = NULL;
    printf("Message buffer cleaned up\n");
}

// Command-line function to parse commands and invoke corresponding functions.
void cmd_msgbuffer(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: msgbuffer [init | write <data> | read | cleanup]\n");
        return;
    }
    if (strcmp(argv[1], "init") == 0) {
        msgbuffer_init();
    } else if (strcmp(argv[1], "write") == 0) {
        if (argc < 3) {
            printf("Usage: msgbuffer write <data>\n");
            return;
        }
        msgbuffer_write(argv[2], strlen(argv[2]));
    } else if (strcmp(argv[1], "read") == 0) {
        msgbuffer_read();
    } else if (strcmp(argv[1], "cleanup") == 0) {
        msgbuffer_cleanup();
    } else {
        printf("Unknown command: %s\n", argv[1]);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_msgbuffer, msgbuffer, msgbuffer test.);

void app_msgbuffer_init(void)
{
}
