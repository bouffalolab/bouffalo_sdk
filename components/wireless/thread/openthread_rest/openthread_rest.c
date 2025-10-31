#include <string.h>
#include <cJSON.h>

#include <lwip/err.h>
#include <lwip/ip6_addr.h>
#include <lwip/timeouts.h>

#include <openthread/border_agent.h>
#include <openthread/border_router.h>

#include <openthread/dataset.h>
#include <openthread/dataset_ftd.h>
#include <openthread/thread_ftd.h>
#include <openthread_port.h>
#include <openthread_br.h>

#include "openthread_httpd.h"

#ifndef OPENTHREAD_REST_FORMAT_STR_LEN
#define OPENTHREAD_REST_FORMAT_STR_LEN 64
#endif

#ifndef OPENTHREAD_RESP_HDR_LEN
#define OPENTHREAD_RESP_HDR_LEN 256
#endif

#ifndef HTTP_VERSION
#define HTTP_VERSION "HTTP/1.1"
#endif

#ifndef HTTP_SERVER_CONTENT_TYPE 
#define VERSION_STRING_HELPER(x, y, z) #x "." #y "." #z
#define VERSION_STRING(x, y, z) VERSION_STRING_HELPER(x, y, z)
#define VERSION_REST_STRING VERSION_STRING(VERSION_OT_UTILS_MAJOR, VERSION_OT_UTILS_MINOR, VERSION_OT_UTILS_PATCH)

#define HTTP_SERVER_CONTENT_TYPE \
"Server: bflb-otbr/" VERSION_REST_STRING "\r\n"\
"Content-Type: application/json\r\n"
#endif

typedef int (* http_rest_func_t)(http_accept_type_t accept_type, void *, char **);

typedef struct _rest_process {
    const char                  *uri;
    const uint32_t              uri_len;
    const http_rest_func_t      func;
} rest_request_t;

typedef struct _http_resp_state_line_t {
  u16_t state;
  u16_t txt_len;
  const char * txt;
}http_resp_state_line_t;

static const http_resp_state_line_t http_resp_state_lines[] = {
  {
    .state = 200,
    .txt_len = sizeof("200 OK"),
    .txt = "200 OK",
  },
  {
    .state = 201,
    .txt_len = sizeof("201 Created"),
    .txt = "201 Created",
  },
  {
    .state = 204,
    .txt_len = sizeof("204 No Content"),
    .txt = "204 No Content",
  },
  {
    .state = 400,
    .txt_len = sizeof("204 Bad Request"),
    .txt = "400 Bad Request",
  },
  {
    .state = 401,
    .txt_len = sizeof("401 Forbidden"),
    .txt = "401 Forbidden",
  },
  {
    .state = 404,
    .txt_len = sizeof("401 Not Found"),
    .txt = "404 Not Found",
  },
  {
    .state = 405,
    .txt_len = sizeof("405 Method Not Allowed"),
    .txt = "405 Method Not Allowed",
  },
  {
    .state = 408,
    .txt_len = sizeof("408 Request Timeout"),
    .txt = "408 Request Timeout",
  },
  {
    .state = 409,
    .txt_len = sizeof("409 Conflict"),
    .txt = "409 Conflict",
  },
  {
    .state = 500,
    .txt_len = sizeof("401 Internal Server Error"),
    .txt = "500 Internal Server Error",
  },
  {
    .state = 501,
    .txt_len = sizeof("501 Not Implemented"),
    .txt = "501 Not Implemented",
  }
};

static const char role_name_list[][10] = {
    "disabled",
    "detached",
    "child",
    "router",
    "leader"
};

static int hex_to_bytes(char *aHex, uint8_t *aBytes, uint16_t aBytesLength)
{
    size_t      hexLength = strlen(aHex);
    const char *hexEnd    = aHex + hexLength;
    uint8_t *   cur       = aBytes;
    uint8_t     numChars  = hexLength & 1;
    uint8_t     byte      = 0;

    if ((hexLength + 1) / 2 > aBytesLength) {
        return -1;
    }

    while (aHex < hexEnd) {
        if ('A' <= *aHex && *aHex <= 'F') {
            byte |= 10 + (*aHex - 'A');
        }
        else if ('a' <= *aHex && *aHex <= 'f') {
            byte |= 10 + (*aHex - 'a');
        }
        else if ('0' <= *aHex && *aHex <= '9') {
            byte |= *aHex - '0';
        }
        else {
            return -1;
        }

        aHex++;
        numChars++;

        if (numChars >= 2) {
            numChars = 0;
            *cur++   = byte;
            byte     = 0;
        }
        else {
            byte <<= 4;
        }
    }

    return (int)(cur - aBytes);
}

static cJSON *array_to_json(uint8_t * aArray, size_t len) 
{
    static const char   hex_chars[] = "0123456789ABCDEF";
    cJSON               * json = NULL;
    char                * hex_string = NULL;
    bool                is_done = false;
    int                 i = 0;

    VerifyOrExit(aArray != NULL && len != 0);

    hex_string = (char *) malloc (len * 2 + 1);
    VerifyOrExit(hex_string != NULL);

    for (i = 0; i < len; i++) {
        hex_string[i * 2] = hex_chars[aArray[i] >> 4];
        hex_string[i * 2 + 1] = hex_chars[aArray[i] & 0x0F];
    }
    hex_string[i * 2] = '\0';

    json = cJSON_CreateString(hex_string);
    VerifyOrExit(json != NULL);

    is_done = true;

exit:
    if (false == is_done && json) {
        cJSON_Delete(json);
        json = NULL;
    }
    if (hex_string) {
        free(hex_string);
    }

    return json;
}

static cJSON * ip6addr_to_json(otIp6Address * aIp6addr) 
{
    cJSON               * json = NULL;
    char                * ip6addr_str = NULL;
    bool                is_done = false;
    ip6_addr_t          ip6addr;

    ip6addr_str = (char *) malloc(40);
    VerifyOrExit(ip6addr_str != NULL && aIp6addr != NULL);

    memset(&ip6addr, 0, sizeof(ip6_addr_t));
    memcpy(&ip6addr, aIp6addr->mFields.m8, OT_IP6_ADDRESS_SIZE);
    VerifyOrExit(ip6addr_ntoa_r(&ip6addr, ip6addr_str, 40) != NULL);

    json = cJSON_CreateString((const char *)ip6addr_str);
    VerifyOrExit(json != NULL);

    is_done = true;
    
exit:
    if (false == is_done && json) {
        cJSON_Delete(json);
        json = NULL;
    }
    if (ip6addr_str) {
        free(ip6addr_str);
    }
    
    return json;
}

// static bool json_to_ip6addr(cJSON * json, otIp6Address * aIp6addr)
// {
//     ip6_addr_t          ip6addr;

//     if (false == ip6addr_aton((const char *)json->valuestring, &ip6addr)) {
//         return false;
//     }

//     memcpy(aIp6addr->mFields.m8, ip6addr.addr, OT_IP6_ADDRESS_SIZE);

//     return true;
// }

static cJSON * ip6prefix_to_json(otIp6NetworkPrefix * ip6prefix) 
{
    cJSON               * json = NULL;
    char                * ip6addr_str = NULL;
    bool                is_done = false;
    ip6_addr_t          ip6addr;
    int                 num_stop = 0;
    int                 i = 0;

    ip6addr_str = (char *) malloc(40);
    VerifyOrExit(ip6addr_str != NULL && ip6prefix != NULL);

    memset(&ip6addr, 0, sizeof(ip6_addr_t));
    memcpy(&ip6addr, ip6prefix->m8, OT_IP6_PREFIX_SIZE);
    VerifyOrExit(ip6addr_ntoa_r(&ip6addr, ip6addr_str, 40) != NULL);

    for (i = 0; i < strlen(ip6addr_str); i ++) {
        if (ip6addr_str[i] == ':') {
            num_stop ++;
        }
        if (num_stop >= (OT_IP6_PREFIX_SIZE - 1) / 2) {
            break;
        }
    }
    VerifyOrExit(i < strlen(ip6addr_str));

    i += ( 1 + snprintf(ip6addr_str + i + 1, 40 - i - 1, ":/%d", (OT_IP6_ADDRESS_SIZE - OT_IP6_PREFIX_SIZE) * 8));
    ip6addr_str[i] = '\0';

    json = cJSON_CreateString((const char *)ip6addr_str);
    VerifyOrExit(json != NULL);

    is_done = true;
    
exit:
    if (false == is_done && json) {
        cJSON_Delete(json);
        json = NULL;
    }
    if (ip6addr_str) {
        free(ip6addr_str);
    }
    
    return json;
}

static bool json_to_ip6prefix(cJSON * json, otIp6NetworkPrefix * aIpPrefix)
{
    char                * p = json->valuestring;
    int                 i = 0;

    for (i = 0; i < strlen(p) && p[i] != '/'; i ++) {}

    if (i >= strlen(p)) {
        return false;
    }

    if (OT_IP6_PREFIX_SIZE != atoi(p + i + 1)) {
        return false;
    }
    p[i] = '\0';

    return OT_IP6_PREFIX_SIZE == hex_to_bytes(p, aIpPrefix->m8, OT_IP6_PREFIX_SIZE);
}

static cJSON * timestamp_to_json(otTimestamp * aTimestamp)
{
    cJSON       *json = cJSON_CreateObject(), *j;
    bool        is_done = false;

    VerifyOrExit(aTimestamp != NULL && json != NULL);

    VerifyOrExit((j = cJSON_CreateNumber(aTimestamp->mSeconds)) != NULL);
    cJSON_AddItemToObject(json, "Seconds", j);

    VerifyOrExit((j = cJSON_CreateNumber(aTimestamp->mTicks)) != NULL);
    cJSON_AddItemToObject(json, "Ticks", j);

    VerifyOrExit((j = cJSON_CreateNumber(aTimestamp->mAuthoritative)) != NULL);
    cJSON_AddItemToObject(json, "Authoritative", j);

    is_done = true;

exit:
    if (false == is_done && json) {
        cJSON_Delete(json);
        json = NULL;
    }

    return json;
}

static bool json_to_timestamp(cJSON *json, otTimestamp * aTimestamp)
{
    cJSON *j = NULL;

    j = cJSON_GetObjectItemCaseSensitive(json, "Seconds");
    if (cJSON_IsNumber(j)) {
        aTimestamp->mSeconds = (uint64_t)(j->valuedouble);
    }
    else if (j) {
        return false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "Ticks");
    if (cJSON_IsNumber(j)) {
        aTimestamp->mTicks = (uint16_t)(j->valueint);
    }
    else if (j) {
        return false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "Authoritative");
    aTimestamp->mAuthoritative = cJSON_IsTrue(j);

    return true;
}

static cJSON * security_policy_to_json(otSecurityPolicy * aSecurityPolicy)
{
    cJSON               *json = cJSON_CreateObject(), *j;
    bool                is_done = false;

    VerifyOrExit(aSecurityPolicy != NULL && json != NULL);

    VerifyOrExit((j = cJSON_CreateNumber(aSecurityPolicy->mRotationTime)) != NULL);
    cJSON_AddItemToObject(json, "RotationTime", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mObtainNetworkKeyEnabled)) != NULL);
    cJSON_AddItemToObject(json, "ObtainNetworkKey", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mNativeCommissioningEnabled)) != NULL);
    cJSON_AddItemToObject(json, "NativeCommissioning", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mRoutersEnabled)) != NULL);
    cJSON_AddItemToObject(json, "Routers", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mExternalCommissioningEnabled)) != NULL);
    cJSON_AddItemToObject(json, "ExternalCommissioning", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mCommercialCommissioningEnabled)) != NULL);
    cJSON_AddItemToObject(json, "CommercialCommissioning", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mAutonomousEnrollmentEnabled)) != NULL);
    cJSON_AddItemToObject(json, "AutonomousEnrollment", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mNetworkKeyProvisioningEnabled)) != NULL);
    cJSON_AddItemToObject(json, "NetworkKeyProvisioning", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mTobleLinkEnabled)) != NULL);
    cJSON_AddItemToObject(json, "TobleLink", j);

    VerifyOrExit((j = cJSON_CreateBool(aSecurityPolicy->mNonCcmRoutersEnabled)) != NULL);
    cJSON_AddItemToObject(json, "NonCcmRouters", j);

    is_done = true;

exit:
    if (false == is_done && json) {
        cJSON_Delete(json);
        json = NULL;
    }

    return json;

    cJSON *securityPolicy = cJSON_CreateObject();


    return securityPolicy;
}

static bool json_to_security_policy(cJSON * json, otSecurityPolicy * aSecurityPolicy)
{
    cJSON               * j = NULL;

    j = cJSON_GetObjectItemCaseSensitive(json, "RotationTime");
    if (cJSON_IsNumber(j)) {
        aSecurityPolicy->mRotationTime = (uint16_t)(j->valueint);
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "ObtainNetworkKey");
    aSecurityPolicy->mObtainNetworkKeyEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "NativeCommissioning");
    aSecurityPolicy->mNativeCommissioningEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "Routers");
    aSecurityPolicy->mRoutersEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "ExternalCommissioning");
    aSecurityPolicy->mExternalCommissioningEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "CommercialCommissioning");
    aSecurityPolicy->mCommercialCommissioningEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "AutonomousEnrollment");
    aSecurityPolicy->mAutonomousEnrollmentEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "NetworkKeyProvisioning");
    aSecurityPolicy->mNetworkKeyProvisioningEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "TobleLink");
    aSecurityPolicy->mTobleLinkEnabled = cJSON_IsTrue(j);

    j = cJSON_GetObjectItemCaseSensitive(json, "NonCcmRouters");
    aSecurityPolicy->mNonCcmRoutersEnabled = cJSON_IsTrue(j);

    return true;
}

static cJSON * active_dataset_to_json(otOperationalDataset *aDataset) 
{
    cJSON               * json = cJSON_CreateObject(), *j;
    bool                is_done = false;

    VerifyOrExit(json != NULL && aDataset != NULL);

    if (aDataset->mComponents.mIsActiveTimestampPresent) {
        VerifyOrExit((j = timestamp_to_json(&aDataset->mActiveTimestamp)) != NULL);
        cJSON_AddItemToObject(json, "ActiveTimestamp", j);
    }

    if (aDataset->mComponents.mIsNetworkKeyPresent) {
        VerifyOrExit((j = array_to_json((uint8_t *)aDataset->mNetworkKey.m8, OT_NETWORK_KEY_SIZE)) != NULL);
        cJSON_AddItemToObject(json, "NetworkKey", j);
    }

    if (aDataset->mComponents.mIsNetworkNamePresent) {
        VerifyOrExit((j = cJSON_CreateString(aDataset->mNetworkName.m8)) != NULL);
        cJSON_AddItemToObject(json, "NetworkName", j);
    }

    if (aDataset->mComponents.mIsExtendedPanIdPresent) {
        VerifyOrExit((j = array_to_json((uint8_t *)aDataset->mExtendedPanId.m8, OT_EXT_PAN_ID_SIZE)) != NULL);
        cJSON_AddItemToObject(json, "ExtPanId", j);
    }

    if (aDataset->mComponents.mIsMeshLocalPrefixPresent) {
        VerifyOrExit((j = ip6prefix_to_json(&aDataset->mMeshLocalPrefix)) != NULL);
        cJSON_AddItemToObject(json, "MeshLocalPrefix", j);
    }

    if (aDataset->mComponents.mIsPanIdPresent) {
        VerifyOrExit((j = cJSON_CreateNumber(aDataset->mPanId)) != NULL);
        cJSON_AddItemToObject(json, "PanId", j);
    }

    if (aDataset->mComponents.mIsChannelPresent) {
        VerifyOrExit((j = cJSON_CreateNumber(aDataset->mChannel)) != NULL);
        cJSON_AddItemToObject(json, "Channel", j);
    }

    if (aDataset->mComponents.mIsPskcPresent) {
        VerifyOrExit((j = array_to_json((uint8_t *)aDataset->mPskc.m8, OT_PSKC_MAX_SIZE)) != NULL);
        cJSON_AddItemToObject(json, "PSKc", j);
    }

    if (aDataset->mComponents.mIsSecurityPolicyPresent) {
        VerifyOrExit((j = security_policy_to_json(&aDataset->mSecurityPolicy)) != NULL);
        cJSON_AddItemToObject(json, "SecurityPolicy", j);
    }

    if (aDataset->mComponents.mIsChannelMaskPresent) {
        VerifyOrExit((j = cJSON_CreateNumber(aDataset->mChannelMask)) != NULL);
        cJSON_AddItemToObject(json, "ChannelMask", j);
    }

    is_done = true;
exit:
    
    if (false == is_done) {
        cJSON_Delete(json);
        json = NULL;
    }

    return json;
}

static bool json_to_active_dataset(cJSON * json, otOperationalDataset * aDataset)
{
    cJSON * j = NULL;

    if (otDatasetCreateNewNetwork(otrGetInstance(), aDataset) != OT_ERROR_NONE) {
        return false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "ActiveTimestamp");
    if (cJSON_IsObject(j)) {
        if (false == json_to_timestamp(j, &aDataset->mActiveTimestamp)) {
            return false;
        }
        aDataset->mComponents.mIsActiveTimestampPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsActiveTimestampPresent = false;
    }
    else if (j) {
        return false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "NetworkKey");
    if (cJSON_IsString(j)) {
        VerifyOrExit(j->valuestring != NULL);
        if (0 != memcmp(j->valuestring, "random", sizeof("random"))) {
            VerifyOrExit(hex_to_bytes(j->valuestring, aDataset->mNetworkKey.m8, OT_NETWORK_KEY_SIZE));
        }
        aDataset->mComponents.mIsNetworkKeyPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsNetworkKeyPresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "ExtPanId");
    if (cJSON_IsString(j)) {
        VerifyOrExit(j->valuestring != NULL);
        if (0 != memcmp(j->valuestring, "random", sizeof("random"))) {
            VerifyOrExit(hex_to_bytes(j->valuestring, aDataset->mExtendedPanId.m8, OT_EXT_PAN_ID_SIZE) 
                         == OT_EXT_PAN_ID_SIZE);
        }
        aDataset->mComponents.mIsExtendedPanIdPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsExtendedPanIdPresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "MeshLocalPrefix");
    if (cJSON_IsString(j)) {
        if (0 != memcmp(j->valuestring, "random", sizeof("random"))) {
            VerifyOrExit(json_to_ip6prefix(j, &aDataset->mMeshLocalPrefix) == true);
        }
        aDataset->mComponents.mIsMeshLocalPrefixPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsMeshLocalPrefixPresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "PanId");
    if (cJSON_IsNumber(j)) {
        if (0 != memcmp(j->valuestring, "random", sizeof("random"))) {
            aDataset->mPanId                      = (otPanId)(j->valueint);
        }
        aDataset->mComponents.mIsPanIdPresent = true;
    }
    else if (cJSON_IsString(j)) {
        if (0 != memcmp(j->valuestring, "random", sizeof("random"))) {
            return false;
        }
        aDataset->mComponents.mIsPanIdPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsPanIdPresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "NetworkName");
    if (cJSON_IsString(j)) {
        VerifyOrExit(j->valuestring != NULL);
        if (0 == memcmp(j->valuestring, "OpenThread-<PanId>", sizeof("OpenThread-<PanId>"))) {
            snprintf(aDataset->mNetworkName.m8, OT_NETWORK_NAME_MAX_SIZE, "OpenThread-%04x", aDataset->mPanId);
        }
        else {
            VerifyOrExit(strlen(j->valuestring) <= OT_NETWORK_NAME_MAX_SIZE);
            strncpy(aDataset->mNetworkName.m8, j->valuestring, OT_NETWORK_NAME_MAX_SIZE);
        }
        aDataset->mComponents.mIsNetworkNamePresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsNetworkNamePresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "Channel");
    if (cJSON_IsNumber(j)) {
        aDataset->mChannel                      = (uint16_t)(j->valueint);
        aDataset->mComponents.mIsChannelPresent = true;
    }
    else if (cJSON_IsString(j)) {
        if (0 != memcmp(j->valuestring, "random", sizeof("random"))) {
            return false;
        }
        aDataset->mComponents.mIsChannelPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsChannelPresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "PSKc");
    if (cJSON_IsString(j)) {
        VerifyOrExit(j->valuestring != NULL);
        if (0 != memcmp(j->valuestring, "random", sizeof("random"))) {
            VerifyOrExit(hex_to_bytes(j->valuestring, aDataset->mPskc.m8, OT_PSKC_MAX_SIZE) == OT_PSKC_MAX_SIZE);
        }
        aDataset->mComponents.mIsPskcPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsPskcPresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "SecurityPolicy");
    if (cJSON_IsObject(j)) {
        VerifyOrExit(json_to_security_policy(j, &aDataset->mSecurityPolicy));
        aDataset->mComponents.mIsSecurityPolicyPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsSecurityPolicyPresent = false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "ChannelMask");
    if (cJSON_IsNumber(j)) {
        aDataset->mChannelMask                      = j->valueint;
        aDataset->mComponents.mIsChannelMaskPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsChannelMaskPresent = false;
    }

    return true;

exit: 
    return false;
}

static cJSON * pending_dataset_to_json(otOperationalDataset *aDataset) 
{
    cJSON               * json = NULL, *j = NULL;
    bool                is_done = false;

    VerifyOrExit((json = j = active_dataset_to_json(aDataset)) != NULL);

    VerifyOrExit((json = cJSON_CreateObject()) != NULL);
    cJSON_AddItemToObject(json, "ActiveDataset", j);

    if (aDataset->mComponents.mIsPendingTimestampPresent) {
        VerifyOrExit((j = timestamp_to_json(&aDataset->mPendingTimestamp)) != NULL);
        cJSON_AddItemToObject(json, "PendingTimestamp", j);
    }

    if (aDataset->mComponents.mIsDelayPresent) {
        VerifyOrExit((j = cJSON_CreateNumber(aDataset->mDelay)) != NULL);
        cJSON_AddItemToObject(json, "Delay", j);
    }

    is_done = true;
exit:
    
    if (false == is_done) {
        cJSON_Delete(json);
        json = NULL;
    }

    return json;
}

static bool json_to_pending_datase(cJSON * json, otOperationalDataset * aDataset)
{
    cJSON               * j = NULL;
    otTimestamp         timestamp;

    j = cJSON_GetObjectItemCaseSensitive(json, "ActiveDataset");
    if (cJSON_IsObject(j)) {
        VerifyOrExit(json_to_active_dataset(j, aDataset));
    }
    else if (cJSON_IsString(j)) {
        otOperationalDatasetTlvs datasetTlvs;

        datasetTlvs.mLength = hex_to_bytes(j->valuestring, datasetTlvs.mTlvs, OT_OPERATIONAL_DATASET_MAX_LENGTH);
        VerifyOrExit(datasetTlvs.mLength > 0);

        VerifyOrExit(otDatasetParseTlvs(&datasetTlvs, aDataset) == OT_ERROR_NONE);
    }
    else {
        return false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "PendingTimestamp");
    if (cJSON_IsObject(j)) {
        VerifyOrExit(json_to_timestamp(j, &timestamp));
        aDataset->mPendingTimestamp                      = timestamp;
        aDataset->mComponents.mIsPendingTimestampPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsPendingTimestampPresent = false;
    }
    else {
        return false;
    }

    j = cJSON_GetObjectItemCaseSensitive(json, "Delay");
    if (cJSON_IsNumber(j)) {
        aDataset->mDelay                      = j->valueint;
        aDataset->mComponents.mIsDelayPresent = true;
    }
    else if (cJSON_IsNull(j)) {
        aDataset->mComponents.mIsDelayPresent = false;
    }

    return true;

exit: 
    return false;
}

static inline char * json_to_string(cJSON *json) 
{
    char                * json_str = NULL;

    if (json) {
        json_str = cJSON_Print(json);
        cJSON_Delete(json);
    }

    return json_str;
}

static cJSON * get_border_agent_id_json(void) 
{
    otBorderAgentId     baId;

    otrLock();
    if (otBorderAgentGetId(otrGetInstance(), &baId) != OT_ERROR_NONE) {
        otrUnlock();
        return NULL;
    }
    otrUnlock();

    return array_to_json(baId.mId, sizeof(baId.mId));
}

static cJSON * get_device_role_json(void) 
{
    otDeviceRole        role = otThreadGetDeviceRole(otrGetInstance());

    otrLock();
    if ((int)role >= sizeof(role_name_list) / sizeof(role_name_list[0])) {
        otrUnlock();
        return NULL;
    }
    otrUnlock();

    return cJSON_CreateString(role_name_list[(int)role]);
}

static cJSON * get_num_of_router_json(void) 
{
    otRouterInfo        routerInfo;
    int                 numOfRouter = 0;

    otrLock();
    for (int i = 0; i <= otThreadGetMaxRouterId(otrGetInstance()); ++i) {
        if (otThreadGetRouterInfo(otrGetInstance(), i, &routerInfo) != OT_ERROR_NONE) {
            continue;
        }
        ++numOfRouter;
    }
    otrUnlock();

    return cJSON_CreateNumber(numOfRouter);
}

static cJSON * get_rloc_json(void) 
{
    otIp6Address        ip6addr;

    otrLock();
    memcpy(&ip6addr, otThreadGetRloc(otrGetInstance()), sizeof(otIp6Address));
    otrUnlock();

    return ip6addr_to_json(&ip6addr);
}

static cJSON * get_extended_address_json(void) 
{
    otExtAddress        extAddr;

    memcpy(&extAddr, otLinkGetExtendedAddress(otrGetInstance()), sizeof(otExtAddress));

    return array_to_json(extAddr.m8, sizeof(otExtAddress));
}

static cJSON * get_network_name_json(void) 
{
    return cJSON_CreateString((const char *)otThreadGetNetworkName(otrGetInstance()));
}

static cJSON * get_rloc16_json(void) 
{
    return cJSON_CreateNumber(otThreadGetRloc16(otrGetInstance()));
}

static cJSON * get_leader_data_json(void) 
{
    otLeaderData        leaderData;
    bool                is_done;
    cJSON               * json = NULL, *j = NULL;

    memset(&leaderData, 0, sizeof(leaderData));

    otrLock();
    otThreadGetLeaderData(otrGetInstance(), &leaderData);
    otrUnlock();

    VerifyOrExit((json = cJSON_CreateObject()) != NULL);

    VerifyOrExit((j = cJSON_CreateNumber(leaderData.mPartitionId)) != NULL);
    cJSON_AddItemToObject(json, "PartitionId", j);

    VerifyOrExit((j = cJSON_CreateNumber(leaderData.mWeighting)) != NULL);
    cJSON_AddItemToObject(json, "Weighting", j);

    VerifyOrExit((j = cJSON_CreateNumber(leaderData.mDataVersion)) != NULL);
    cJSON_AddItemToObject(json, "DataVersion", j);

    VerifyOrExit((j = cJSON_CreateNumber(leaderData.mStableDataVersion)) != NULL);
    cJSON_AddItemToObject(json, "StableDataVersion", j);

    VerifyOrExit((j = cJSON_CreateNumber(leaderData.mLeaderRouterId)) != NULL);
    cJSON_AddItemToObject(json, "LeaderRouterId", j);

    is_done = true;

exit:

    if (false == is_done && json) {
        cJSON_Delete(json);
        json = NULL;
    }

    return json;
}

static cJSON * get_extended_panid_json(void) 
{
    otExtendedPanId     extPanid;

    memcpy(&extPanid, otThreadGetExtendedPanId(otrGetInstance()), sizeof(otExtendedPanId));

    return array_to_json(extPanid.m8, sizeof(otExtendedPanId));
}

static int openthread_rest_get_node(http_accept_type_t accept_type, void * req, char **body) 
{
    cJSON               * json = NULL, *j;

    json = cJSON_CreateObject();
    VerifyOrExit(json != NULL && body != NULL);

    VerifyOrExit((j = get_border_agent_id_json()) != NULL);
    cJSON_AddItemToObject(json, "BaId", j);

    VerifyOrExit((j = get_device_role_json()) != NULL);
    cJSON_AddItemToObject(json, "State", j);

    VerifyOrExit((j = get_num_of_router_json()) != NULL);
    cJSON_AddItemToObject(json, "NumOfRouter", j);

    VerifyOrExit((j = get_rloc_json()) != NULL);
    cJSON_AddItemToObject(json, "RlocAddress", j);

    VerifyOrExit((j = get_extended_address_json()) != NULL);
    cJSON_AddItemToObject(json, "ExtAddress", j);

    VerifyOrExit((j = get_network_name_json()) != NULL);
    cJSON_AddItemToObject(json, "NetworkName", j);

    VerifyOrExit((j = get_rloc_json()) != NULL);
    cJSON_AddItemToObject(json, "Rloc16", j);

    VerifyOrExit((j = get_leader_data_json()) != NULL);
    cJSON_AddItemToObject(json, "LeaderData", j);

    VerifyOrExit((j = get_extended_panid_json()) != NULL);
    cJSON_AddItemToObject(json, "ExtPanId", j);

    VerifyOrExit((*body = json_to_string(json)) != NULL);

    return http_resp_state_ok;

exit:
    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_baid(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_border_agent_id_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_rloc(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_rloc_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_rloc16(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_rloc16_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_extaddr(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_extended_address_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_state(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_device_role_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_networkname(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_network_name_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_leaderdata(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_leader_data_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_num_of_router(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_num_of_router_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static int openthread_rest_get_node_extpanid(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str;

    if (body && (json_str = json_to_string(get_extended_panid_json())) != NULL) {
        *body = json_str;
        return http_resp_state_ok;
    }

    return http_resp_state_internal_server_error;
}

static char * get_dataset_json_string(http_accept_type_t accept_type, bool isActive) 
{
    cJSON               * json = NULL;
    otError             err;

    if (accept_type == http_accept_type_txt) {
        otOperationalDatasetTlvs    datasetTlvs;

        otrLock();
        if (isActive) {
            err = otDatasetGetActiveTlvs(otrGetInstance(), &datasetTlvs);
        }
        else {
            err = otDatasetGetPendingTlvs(otrGetInstance(), &datasetTlvs);
        }
        otrUnlock();

        if (err == OT_ERROR_NONE) {
            json = array_to_json(datasetTlvs.mTlvs, datasetTlvs.mLength);
        }
        else if (err == OT_ERROR_NOT_FOUND) {
            json = cJSON_CreateNull();
        }
    }
    else {
        otOperationalDataset dataset;

        otrLock();
        if (isActive) {
            err = otDatasetGetActive(otrGetInstance(), &dataset);
        }
        else {
            err = otDatasetGetPending(otrGetInstance(), &dataset);
        }
        otrUnlock();

        if (err == OT_ERROR_NONE) { 
            if (isActive) {
                json = active_dataset_to_json(&dataset);
            }
            else {
                json = pending_dataset_to_json(&dataset);
            }
        }
        else if (err == OT_ERROR_NOT_FOUND) {
            json = cJSON_CreateNull();
        }
    }

    if (json) {
        return json_to_string(json);
    }

    return NULL;
}

static int openthread_rest_get_node_dataset_active(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str = NULL;

    if (accept_type != http_accept_type_txt) {
        accept_type = http_accept_type_json;
    }

    json_str = get_dataset_json_string(accept_type, true);
    if (json_str) {
        *body = json_str;
        return http_resp_state_ok;
    }
    else {
        return http_resp_state_internal_server_error;
    }
}

static int openthread_rest_get_node_dataset_pending(http_accept_type_t accept_type, void * req, char **body) 
{
    char                * json_str = NULL;

    if (accept_type != http_accept_type_txt) {
        accept_type = http_accept_type_json;
    }

    json_str = get_dataset_json_string(accept_type, false);
    if (json_str) {
        *body = json_str;
        return http_resp_state_ok;
    }
    else {
        return http_resp_state_internal_server_error;
    }
}

static void openthread_reset_state_callback(uint32_t arg) 
{
    if (arg) {
        if (!otIp6IsEnabled(otrGetInstance())) {
            if (otIp6SetEnabled(otrGetInstance(), true) == OT_ERROR_NONE) {
                otThreadSetEnabled(otrGetInstance(), true);
            }
        }
        else {
            otThreadSetEnabled(otrGetInstance(), true);
        }
    }
    else {
        if (otThreadSetEnabled(otrGetInstance(), false) == OT_ERROR_NONE) {
            otIp6SetEnabled(otrGetInstance(), false);
        }
    }
}

static int openthread_rest_put_node_state(http_accept_type_t accept_type, void * req, char ** body) 
{
    http_resp_state_t   resp_state = http_resp_state_ok;
    char                * p = NULL;
    cJSON               * json = NULL; 

    if (req == NULL) {
        return http_resp_state_bad_request;
    }

    if (accept_type == http_accept_type_txt) {
        p = (char *) req;
    }
    else {
        json = cJSON_Parse((const char *)req); 
        if (NULL == json ) {
            return http_resp_state_bad_request;
        }

        if (false == cJSON_IsString(json)) {
            cJSON_Delete(json);
            return http_resp_state_bad_request;
        }

        p = json->valuestring;
    }

    if (0 == memcmp("disable", p, sizeof("disable"))) {
        otbr_netif_request_action(openthread_reset_state_callback, false);
    }
    else if (0 == memcmp("enable", p, sizeof("enable"))) {
        otbr_netif_request_action(openthread_reset_state_callback, true);
    }
    else {
        resp_state = http_resp_state_bad_request;
    }

    if (json) {
        cJSON_Delete(json);
    }

    return resp_state;
}

static int openthread_rest_put_dataset_raw (http_accept_type_t accept_type, void * aDataset, bool isActive) 
{
    http_resp_state_t           resp_state = http_resp_state_ok;
    char                        * str = NULL;
    cJSON                       * json = NULL;
    otOperationalDataset        dataset   = {};
    otOperationalDatasetTlvs    datasetTlvs;
    otOperationalDatasetTlvs    datasetUpdateTlvs;
    otError                     errorOt = OT_ERROR_NONE;

    if (aDataset == NULL) {
        return http_resp_state_bad_request;
    }

    if (accept_type == http_accept_type_txt) {
        str = (char *) aDataset;
    }
    else {

        json = cJSON_Parse((const char *)aDataset); 
        if (NULL == json) {
            return http_resp_state_bad_request;
        }

        if (cJSON_IsString(json)) {
            str = json->valuestring;
        }
        else if (false == cJSON_IsObject(json)) {
            cJSON_Delete(json);
            return http_resp_state_bad_request;
        }
    }

    otrLock();

    if (isActive) {
        VerifyOrExit(otThreadGetDeviceRole(otrGetInstance()) == OT_DEVICE_ROLE_DISABLED, 
                     resp_state = http_resp_state_conflict);
        errorOt = otDatasetGetActiveTlvs(otrGetInstance(), &datasetTlvs);
    }
    else {
        errorOt = otDatasetGetPendingTlvs(otrGetInstance(), &datasetTlvs);
    }

    if (errorOt == OT_ERROR_NOT_FOUND) {
        VerifyOrExit(otDatasetCreateNewNetwork(otrGetInstance(), &dataset) == OT_ERROR_NONE, 
                     resp_state = http_resp_state_internal_server_error);
        VerifyOrExit(otDatasetConvertToTlvs(&dataset, &datasetTlvs) == OT_ERROR_NONE, 
                     resp_state = http_resp_state_internal_server_error);
        resp_state = http_resp_state_created;
    }

    if (str) {
        int iBytes = hex_to_bytes(str, datasetUpdateTlvs.mTlvs, OT_OPERATIONAL_DATASET_MAX_LENGTH);
        VerifyOrExit(iBytes >= 0, resp_state = http_resp_state_bad_request);
        datasetUpdateTlvs.mLength = iBytes;

        VerifyOrExit(otDatasetParseTlvs(&datasetUpdateTlvs, &dataset) == OT_ERROR_NONE, 
                     resp_state = http_resp_state_bad_request);
        VerifyOrExit(otDatasetUpdateTlvs(&dataset, &datasetTlvs) == OT_ERROR_NONE, 
                     resp_state = http_resp_state_bad_request);
    }
    else {
        if (isActive) {
            VerifyOrExit(json_to_active_dataset(json, &dataset), 
                         resp_state = http_resp_state_bad_request);
        }
        else {
            VerifyOrExit(json_to_pending_datase(json, &dataset), 
                         resp_state = http_resp_state_bad_request);
        }
        VerifyOrExit(otDatasetUpdateTlvs(&dataset, &datasetTlvs) == OT_ERROR_NONE, 
                     resp_state = http_resp_state_bad_request);
    }

    if (isActive) {
        VerifyOrExit(otDatasetSetActiveTlvs(otrGetInstance(), &datasetTlvs) == OT_ERROR_NONE,
                     resp_state = http_resp_state_bad_request);
    }
    else {
        VerifyOrExit(otDatasetSetPendingTlvs(otrGetInstance(), &datasetTlvs) == OT_ERROR_NONE, 
                     resp_state = http_resp_state_bad_request);
    }

exit:
    otrUnlock();

    return resp_state;
}

static int openthread_rest_put_node_dataset_active(http_accept_type_t accept_type, void * req, char ** body) 
{
    return openthread_rest_put_dataset_raw(accept_type, req, true);
}

static int openthread_rest_put_node_dataset_pending(http_accept_type_t accept_type, void * req, char ** body) 
{
    return openthread_rest_put_dataset_raw(accept_type, req, false);
}

static void delete_node_reset_delay_timercb(void *arg) 
{
    otrLock();
    otInstanceReset(otrGetInstance());
    otrUnlock();
}

static int openthread_rest_delete_node(http_accept_type_t accept_type, void * req, char ** body) 
{
    http_resp_state_t resp_state = http_resp_state_ok;

    otrLock();

    if (!otIp6IsEnabled(otrGetInstance())) {
        VerifyOrExit(otIp6SetEnabled(otrGetInstance(), false) == OT_ERROR_NONE, 
                     resp_state = http_resp_state_internal_server_error);
    }

    VerifyOrExit(otThreadSetEnabled(otrGetInstance(), false) == OT_ERROR_NONE, 
                 resp_state = http_resp_state_internal_server_error);

    VerifyOrExit(otInstanceErasePersistentInfo(otrGetInstance()) == OT_ERROR_NONE, 
                 resp_state = http_resp_state_internal_server_error);

    sys_timeout(1000, delete_node_reset_delay_timercb, NULL);
exit:
    otrUnlock();

    return resp_state;
}

static const rest_request_t rest_request_list_get[] = {
    {
        .uri = OT_REST_RESOURCE_PATH_NODE,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE),
        .func = openthread_rest_get_node,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_BAID,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_BAID),
        .func = openthread_rest_get_node_baid,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_RLOC,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_RLOC),
        .func = openthread_rest_get_node_rloc,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_RLOC16,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_RLOC16),
        .func = openthread_rest_get_node_rloc16,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_EXTADDRESS,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_EXTADDRESS),
        .func = openthread_rest_get_node_extaddr,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_STATE,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_STATE),
        .func = openthread_rest_get_node_state,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_NETWORKNAME,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_NETWORKNAME),
        .func = openthread_rest_get_node_networkname,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_LEADERDATA,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_LEADERDATA),
        .func = openthread_rest_get_node_leaderdata,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_NUMOFROUTER,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_NUMOFROUTER),
        .func = openthread_rest_get_node_num_of_router,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_EXTPANID,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_EXTPANID),
        .func = openthread_rest_get_node_extpanid,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_DATASET_ACTIVE,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_DATASET_ACTIVE),
        .func = openthread_rest_get_node_dataset_active,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_DATASET_PENDING,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_DATASET_PENDING),
        .func = openthread_rest_get_node_dataset_pending,
    },
    {
        .uri = NULL,
        .uri_len = 0,
        .func = NULL,
    }
};

static const rest_request_t rest_request_list_put[] = {
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_STATE,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_STATE),
        .func = openthread_rest_put_node_state,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_DATASET_ACTIVE,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_DATASET_ACTIVE),
        .func = openthread_rest_put_node_dataset_active,
    },
    {
        .uri = OT_REST_RESOURCE_PATH_NODE_DATASET_PENDING,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE_DATASET_PENDING),
        .func = openthread_rest_put_node_dataset_pending,
    },
    {
        .uri = NULL,
        .uri_len = 0,
        .func = NULL,
    }
};

static const rest_request_t rest_request_list_delete[] = {
    {
        .uri = OT_REST_RESOURCE_PATH_NODE,
        .uri_len = sizeof(OT_REST_RESOURCE_PATH_NODE),
        .func = openthread_rest_delete_node,
    },
    {
        .uri = NULL,
        .uri_len = 0,
        .func = NULL,
    }
};

static const rest_request_t * rest_request_list[] = {
    rest_request_list_get,
    rest_request_list_put,
    rest_request_list_delete
};

bool openthread_rest_request(void *connection, http_method_type_t method, 
                             http_accept_type_t accept_type, char *uri, char *body,
                             http_continue_func_t continue_func) 
{
    http_resp_state_t       ret_state = http_resp_state_not_found;
    const rest_request_t    *req_list = rest_request_list[(int)method];
    char                    *resp_body = NULL;
    int                     i = 0;

    if ((int)method >= sizeof(rest_request_list) / sizeof(rest_request_list[0])) {
        return false;
    }

    do {
        while (req_list[i].uri) {
            if (0 == memcmp(req_list[i].uri, uri, req_list[i].uri_len)) {
                break;
            }
            i++;
        }
        if (NULL == req_list[i].uri) {
            ret_state = http_resp_state_forbidden;
            return http_resp_state_not_found;
        }

        ret_state = req_list[i].func(accept_type, body, &resp_body);

    } while (0);

    return ERR_OK == http_setup_file(connection, ret_state, resp_body);
}


char * openthread_rest_construct_resp(http_resp_state_t resp_state, char * body) 
{
    char * resp = NULL, *p;
    int body_len = body? strlen(body): 0;
    uint32_t resp_len = body_len + OPENTHREAD_RESP_HDR_LEN + 1;

    VerifyOrExit((int)resp_state < sizeof(http_resp_state_lines) /  sizeof(http_resp_state_lines[0]));

    resp = p = (char *)malloc(resp_len);
    VerifyOrExit(resp);

    memcpy(p, HTTP_VERSION, sizeof(HTTP_VERSION));
    p += sizeof(HTTP_VERSION) - 1;
    p[0] = ' ';
    p ++;
    
    memcpy(p, http_resp_state_lines[resp_state].txt, http_resp_state_lines[resp_state].txt_len);
    p += http_resp_state_lines[resp_state].txt_len - 1;
    p[0] = '\r';
    p[1] = '\n';
    p += 2;

    memcpy(p, HTTP_SERVER_CONTENT_TYPE, sizeof(HTTP_SERVER_CONTENT_TYPE));
    p += sizeof(HTTP_SERVER_CONTENT_TYPE) - 1;

    memcpy(p, HTTP_CONTENT_LEN, sizeof(HTTP_CONTENT_LEN));
    p += sizeof(HTTP_CONTENT_LEN) - 1;

    if (resp_state == http_resp_state_no_content) {
        p += snprintf(p, resp_len - (size_t)(p - resp), "%d\r\n\r\n", 0);
    } 
    else if (resp_state == http_resp_state_ok) {
        p += snprintf(p, resp_len - (size_t)(p - resp), "%d\r\n\r\n", body_len);
        VerifyOrExit((size_t)(p - resp) + body_len + 1 < resp_len, (free(resp), resp = NULL));
        strncpy(p, body, body_len);
    }
    else {
        body = p + 16;
        body_len = snprintf(body, resp_len - (size_t)(body - resp), 
                    "{\"ErrorCode\": %d, \"ErrorMessage\": \"%s\"}\r\n", 
                    http_resp_state_lines[resp_state].state, http_resp_state_lines[resp_state].txt);

        p += snprintf(p, 16, "%d\r\n\r\n", body_len);
        memmove(p, body, body_len + 1);
    }
    p[body_len] = '\0';

exit:
    return resp;
}