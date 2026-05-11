#include "bflbwifid_runtime.h"

#include "nethub_wifi_service.h"

int bflbwifid_runtime_refresh(nethub_wifi_status_t *status)
{
    return nethub_wifi_service_get_status(status);
}
