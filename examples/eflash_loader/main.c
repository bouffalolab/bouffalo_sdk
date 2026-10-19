#include "flashloader_commands.h"
#include "flashloader_flash.h"
#include "flashloader_port.h"
#include "flashloader_protocol.h"
#include "flashloader_transport.h"

int main(void)
{
    flashloader_port_init();
    flashloader_transport_init();
    flashloader_commands_init();

    (void)flashloader_flash_init();
    flashloader_protocol_run();
}
