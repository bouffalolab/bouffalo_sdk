#include <openthread_port.h>

#include <openthread/cli.h>
#include <openthread/platform/logging.h>

void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    va_list argp;

    va_start(argp, aFormat);
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, argp);
    va_end(argp);
}

void otPlatLogLine(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aLogLine)
{
    otPlatLog(aLogLevel, aLogRegion, "%s", aLogLine);
}
