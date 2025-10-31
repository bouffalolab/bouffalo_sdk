#ifndef __MATTER_FACTORY_DATA_H
#define __MATTER_FACTORY_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION_MFD_MAJOR 1
#define VERSION_MFD_MINOR 6
#define VERSION_MFD_PATCH 1

bool mfd_init(void);

int mfd_getDacCert(uint8_t *p, uint32_t size);
uint8_t *mfd_getDacCertPtr(uint32_t *psize);
int mfd_getDacPrivateKey(uint8_t *p, uint32_t size);
uint8_t *mfd_getDacPrivateKeyPtr(uint32_t *psize);
int mfd_getPasscode(uint8_t *p, uint32_t size);
int mfd_getPaiCert(uint8_t *p, uint32_t size);
int mfd_getCd(uint8_t *p, uint32_t size);
int mfd_getSerialNumber(char *p, uint32_t size);
int mfd_getDiscriminator(uint8_t *p, uint32_t size);
int mfd_getRotatingDeviceIdUniqueId(uint8_t *p, uint32_t size);
int mfd_getSapke2It(uint8_t *p, uint32_t size);
int mfd_getSapke2Salt(uint8_t *p, uint32_t size);
int mfd_getSapke2Verifier(uint8_t *p, uint32_t size);
/** from device tree */
int mfd_getVendorName (char * buf, uint32_t size);
int mfd_getVendorId(uint8_t *buf, uint32_t size);
int mfd_getProductName(char * buf, uint32_t size);
int mfd_getProductId(uint8_t *buf, uint32_t size);
int mfd_getPartNumber(char *buf, uint32_t size);
int mfd_getProductUrl(char * buf, uint32_t size);
int mfd_getProductLabel(char * buf, uint32_t size);
bool mfd_getManufacturingDate(uint16_t *pYear, uint8_t *pMonth, uint8_t *pDay);
int mfd_getHardwareVersion(uint8_t * buf, uint32_t size);
int mfd_getHardwareVersionString(char * buf, uint32_t size);

int mfd_getElementById(int16_t id, uint8_t * buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
