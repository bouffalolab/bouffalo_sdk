#ifndef _USBD_WINUSB_H
#define _USBD_WINUSB_H

/* WinUSB Microsoft OS 2.0 descriptor request codes */
#define WINUSB_REQUEST_GET_DESCRIPTOR_SET          0x07
#define WINUSB_REQUEST_SET_ALT_ENUM                0x08

/* WinUSB Microsoft OS 2.0 descriptor sizes */
#define WINUSB_DESCRIPTOR_SET_HEADER_SIZE          10
#define WINUSB_FUNCTION_SUBSET_HEADER_SIZE         8
#define WINUSB_FEATURE_COMPATIBLE_ID_SIZE          20


/* WinUSB Microsoft OS 2.0 Descriptor Types */
#define WINUSB_SET_HEADER_DESCRIPTOR_TYPE           0x00
#define WINUSB_SUBSET_HEADER_CONFIGURATION_TYPE     0x01
#define WINUSB_SUBSET_HEADER_FUNCTION_TYPE          0x02
#define WINUSB_FEATURE_COMPATIBLE_ID_TYPE           0x03
#define WINUSB_FEATURE_REG_PROPERTY_TYPE            0x04
#define WINUSB_FEATURE_MIN_RESUME_TIME_TYPE         0x05
#define WINUSB_FEATURE_MODEL_ID_TYPE                0x06
#define WINUSB_FEATURE_CCGP_DEVICE_TYPE             0x07


#define WINUSB_PROP_DATA_TYPE_REG_SZ                0x01
#define WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ          0x07

/* WebUSB Descriptor Types */
#define WEBUSB_DESCRIPTOR_SET_HEADER_TYPE       0x00
#define WEBUSB_CONFIGURATION_SUBSET_HEADER_TYPE 0x01
#define WEBUSB_FUNCTION_SUBSET_HEADER_TYPE      0x02
#define WEBUSB_URL_TYPE                         0x03

/* WebUSB Request Codes */
#define WEBUSB_REQUEST_GET_URL                  0x02

/* bScheme in URL descriptor */
#define WEBUSB_URL_SCHEME_HTTP                  0x00
#define WEBUSB_URL_SCHEME_HTTPS                 0x01

/* WebUSB Descriptor sizes */
#define WEBUSB_DESCRIPTOR_SET_HEADER_SIZE       5
#define WEBUSB_CONFIGURATION_SUBSET_HEADER_SIZE 4
#define WEBUSB_FUNCTION_SUBSET_HEADER_SIZE      3


/* WinUSB Microsoft OS 2.0 descriptor Platform Capability Descriptor */
struct usb_bos_winusb_platform_capability_descriptor {
	struct usb_bos_capability_descriptor winusb_platform;
	uint32_t dwWindowsVersion;
	uint16_t wMSOSDescriptorSetTotalLength;
	uint8_t bMS_VendorCode;
	uint8_t bAltEnumCode;
} __packed;


#endif