#include "usbd_core.h"

#define WCID_VENDOR_CODE 0x17

__ALIGN_BEGIN const uint8_t  WCID_StringDescriptor_MSOS [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// MS OS string descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DESCRIPTOR_TYPE_STRING,                       /* bDescriptorType */
  /* MSFT100 */
  'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00,       /* wcChar_7 */
  '1', 0x00, '0', 0x00, '0', 0x00,                  /* wcChar_7 */
  WCID_VENDOR_CODE,                                 /* bVendorCode */
  0x00,                                             /* bReserved */
};
__ALIGN_BEGIN const uint8_t WINUSB_WCIDDescriptor [88] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID descriptor
  ///////////////////////////////////////
  0x58, 0x00, 0x00, 0x00,                           /* dwLength */
  0x00, 0x01,                                       /* bcdVersion */
  0x04, 0x00,                                       /* wIndex */
  0x03,                                             /* bCount */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         /* bReserved_7 */
  
  ///////////////////////////////////////
  /// WCID function descriptor
  ///////////////////////////////////////
  0x00,                                             /* bFirstInterfaceNumber */
  0x01,                                             /* bReserved */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               /* bReserved_6 */
  
  ///////////////////////////////////////
  /// WCID function descriptor
  ///////////////////////////////////////
  0x01,                                             /* bFirstInterfaceNumber */
  0x01,                                             /* bReserved */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               /* bReserved_6 */
  
  ///////////////////////////////////////
  /// WCID function descriptor
  ///////////////////////////////////////
  0x02,                                             /* bFirstInterfaceNumber */
  0x01,                                             /* bReserved */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               /* bReserved_6 */
};
__ALIGN_BEGIN const uint8_t WINUSB_IF0_WCIDProperties [142] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID property descriptor
  ///////////////////////////////////////
  0x8e, 0x00, 0x00, 0x00,                           /* dwLength */
  0x00, 0x01,                                       /* bcdVersion */
  0x05, 0x00,                                       /* wIndex */
  0x01, 0x00,                                       /* wCount */
  
  ///////////////////////////////////////
  /// registry propter descriptor
  ///////////////////////////////////////
  0x84, 0x00, 0x00, 0x00,                           /* dwSize */
  0x01, 0x00, 0x00, 0x00,                           /* dwPropertyDataType */
  0x28, 0x00,                                       /* wPropertyNameLength */
  /* DeviceInterfaceGUID */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcName_20 */
  'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcName_20 */
  't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcName_20 */
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcName_20 */
  'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00,      /* wcName_20 */
  0x4e, 0x00, 0x00, 0x00,                           /* dwPropertyDataLength */
  /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
  '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcData_39 */
  'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcData_39 */
  '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcData_39 */
  '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcData_39 */
  '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcData_39 */
  'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcData_39 */
  '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcData_39 */
  'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcData_39 */
  'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcData_39 */
  '6', 0x00, '}', 0x00, 0x00, 0x00,                 /* wcData_39 */
};

struct usb_msosv1_descriptor msosv1_desc =
{
    .string = (uint8_t*)WCID_StringDescriptor_MSOS,
    .string_len = 18,
    .vendor_code = 0x03,
    .compat_id = (uint8_t*)WINUSB_WCIDDescriptor,
    .compat_id_len = sizeof(WINUSB_WCIDDescriptor)
};

#define  WINUSB20_WCID_DESC_SET_SIZE  (490)
__ALIGN_BEGIN const uint8_t WINUSB20_WCIDDescriptorSet [490] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID20 descriptor set descriptor
  ///////////////////////////////////////
  0x0a, 0x00,                                       /* wLength */
  0x00, 0x00,                                       /* wDescriptorType */
  0x00, 0x00, 0x03, 0x06,                           /* dwWindowsVersion */
  0xea, 0x01,                                       /* wDescriptorSetTotalLength */
  
  ///////////////////////////////////////
  /// WCID20 function subset descriptor
  ///////////////////////////////////////
  0x08, 0x00,                                       /* wLength */
  0x02, 0x00,                                       /* wDescriptorType */
  0x00,                                             /* bFirstInterface */
  0x00,                                             /* bReserved */
  0xa0, 0x00,                                       /* wSubsetLength */
  
  ///////////////////////////////////////
  /// WCID20 compatible ID descriptor
  ///////////////////////////////////////
  0x14, 0x00,                                       /* wLength */
  0x03, 0x00,                                       /* wDescriptorType */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  
  ///////////////////////////////////////
  /// WCID20 registry property descriptor
  ///////////////////////////////////////
  0x84, 0x00,                                       /* wLength */
  0x04, 0x00,                                       /* wDescriptorType */
  0x07, 0x00,                                       /* wPropertyDataType */
  0x2a, 0x00,                                       /* wPropertyNameLength */
  /* DeviceInterfaceGUIDs */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcPropertyName_21 */
  'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcPropertyName_21 */
  't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcPropertyName_21 */
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcPropertyName_21 */
  'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00,       /* wcPropertyName_21 */
  0x00, 0x00,                                       /* wcPropertyName_21 */
  0x50, 0x00,                                       /* wPropertyDataLength */
  /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
  '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcPropertyData_40 */
  '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcPropertyData_40 */
  '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcPropertyData_40 */
  'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcPropertyData_40 */
  '6', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,     /* wcPropertyData_40 */
  
  ///////////////////////////////////////
  /// WCID20 function subset descriptor
  ///////////////////////////////////////
  0x08, 0x00,                                       /* wLength */
  0x02, 0x00,                                       /* wDescriptorType */
  0x01,                                             /* bFirstInterface */
  0x00,                                             /* bReserved */
  0xa0, 0x00,                                       /* wSubsetLength */
  
  ///////////////////////////////////////
  /// WCID20 compatible ID descriptor
  ///////////////////////////////////////
  0x14, 0x00,                                       /* wLength */
  0x03, 0x00,                                       /* wDescriptorType */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  
  ///////////////////////////////////////
  /// WCID20 registry property descriptor
  ///////////////////////////////////////
  0x84, 0x00,                                       /* wLength */
  0x04, 0x00,                                       /* wDescriptorType */
  0x07, 0x00,                                       /* wPropertyDataType */
  0x2a, 0x00,                                       /* wPropertyNameLength */
  /* DeviceInterfaceGUIDs */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcPropertyName_21 */
  'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcPropertyName_21 */
  't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcPropertyName_21 */
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcPropertyName_21 */
  'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00,       /* wcPropertyName_21 */
  0x00, 0x00,                                       /* wcPropertyName_21 */
  0x50, 0x00,                                       /* wPropertyDataLength */
  /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
  '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcPropertyData_40 */
  '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcPropertyData_40 */
  '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcPropertyData_40 */
  'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcPropertyData_40 */
  '6', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,     /* wcPropertyData_40 */
  
  ///////////////////////////////////////
  /// WCID20 function subset descriptor
  ///////////////////////////////////////
  0x08, 0x00,                                       /* wLength */
  0x02, 0x00,                                       /* wDescriptorType */
  0x02,                                             /* bFirstInterface */
  0x00,                                             /* bReserved */
  0xa0, 0x00,                                       /* wSubsetLength */
  
  ///////////////////////////////////////
  /// WCID20 compatible ID descriptor
  ///////////////////////////////////////
  0x14, 0x00,                                       /* wLength */
  0x03, 0x00,                                       /* wDescriptorType */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  
  ///////////////////////////////////////
  /// WCID20 registry property descriptor
  ///////////////////////////////////////
  0x84, 0x00,                                       /* wLength */
  0x04, 0x00,                                       /* wDescriptorType */
  0x07, 0x00,                                       /* wPropertyDataType */
  0x2a, 0x00,                                       /* wPropertyNameLength */
  /* DeviceInterfaceGUIDs */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcPropertyName_21 */
  'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcPropertyName_21 */
  't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcPropertyName_21 */
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcPropertyName_21 */
  'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00,       /* wcPropertyName_21 */
  0x00, 0x00,                                       /* wcPropertyName_21 */
  0x50, 0x00,                                       /* wPropertyDataLength */
  /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
  '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcPropertyData_40 */
  '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcPropertyData_40 */
  '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcPropertyData_40 */
  'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcPropertyData_40 */
  '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcPropertyData_40 */
  'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcPropertyData_40 */
  'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcPropertyData_40 */
  '6', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00,     /* wcPropertyData_40 */
};
struct usb_msosv2_descriptor msosv2_desc = 
{
    .compat_id = (uint8_t*)WINUSB20_WCIDDescriptorSet,
    .compat_id_len = 490
};

__ALIGN_BEGIN const uint8_t WINUSB20_WCIDBOS [33] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID20 BOS descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  USB_DESCRIPTOR_TYPE_BINARY_OBJECT_STORE,          /* bDescriptorType */
  0x21, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumDeviceCaps */
  
  ///////////////////////////////////////
  /// WCID20 device capability descriptor
  ///////////////////////////////////////
  0x1c,                                             /* bLength */
  0x10,                                             /* bDescriptorType */
  0x05,                                             /* bDevCapabilityType */
  0x00,                                             /* bReserved */
  0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c,   /* bPlatformCapabilityUUID_16 */
  0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f,   /* bPlatformCapabilityUUID_16 */
  0x00, 0x00, 0x03, 0x06,                           /* dwWindowsVersion */
  LO_BYTE(WINUSB20_WCID_DESC_SET_SIZE), HI_BYTE(WINUSB20_WCID_DESC_SET_SIZE),/* wDescriptorSetTotalLength */
  WCID_VENDOR_CODE,                                 /* bVendorCode */
  0x00,                                             /* bAltEnumCode */
};

// static struct usb_bos_descriptor wcid_bos= 
// {
//     .bos_id = (uint8_t*)WINUSB20_WCIDBOS,
//     .bos_id_len = 33
// };



