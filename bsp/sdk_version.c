#include <sdk_version.h>

#ifndef BFLB_GIT_SUFFIX
#define BFLB_GIT_SUFFIX ""
#endif

static const char *version_sdk __attribute__((used, section(".version.sdk"))) = "component_version_sdk: " PROJECT_SDK_VERSION BFLB_GIT_SUFFIX;
