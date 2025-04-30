typedef struct 
{
    const char * mKey;
    const uint8_t * mData;
    uint32_t mDataSize;
} txt_item_t;

typedef struct {
    char * mInstanceName;
    char * mType;
    char * mProtocol;
    char * mHostName;
    uint16_t mPort;
    uint32_t mTtl;
    int mAddressNum;
    ip_addr_t *mAddress;
    int mTxtItemNum;
    txt_item_t *mTxtItems;
} mdns_discover_result_t;

typedef void (*mdns_result_callback_t)(int aNumResult, mdns_discover_result_t *aResult, struct _mdns_discover_param * aDiscoverParam);

typedef struct _mdns_discover_param {
    char * mInstanceName;
    char * mType;
    char * mProtocol;
    bool  mIsBrowse;
    uint16_t mAddressType;
    mdns_result_callback_t mCallback;
    void * arg1;
    void * arg2;
} mdns_discover_param_t;

otbrError WiFiDnssdPublishService(const char * aHostName, const char *aInstanceName, const char *aServiceType, const char *aProtocol, 
    int aPort, txt_item_t *aTxtItems, int aTxtItemNum, const char **aSubTypes, int aSubTypesNum);

otbrError WiFiDnssdRemoveServices();

otbrError WiFiDnssdBrowse(const char * aType, const char *aProtocol, enum lwip_ip_addr_type aAddrType, mdns_result_callback_t aCallback, 
    uint32_t aTimeout, int aMaxResult, void * arg1, void *arg2);

otbrError WiFiDnssdResolve(const char * aInstanceName, const char *aType, const char * aProtocol, mdns_result_callback_t aCallback, 
    uint32_t aTimeout, int aMaxResult, void * arg1, void * arg2);
