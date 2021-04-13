/**
 * @file
 * @brief USB Audio Device Class public header
 *
 * Header follows below documentation:
 * - USB Device Class Definition for Audio Devices (audio10.pdf)
 *
 * Additional documentation considered a part of USB Audio v1.0:
 * - USB Device Class Definition for Audio Data Formats (frmts10.pdf)
 * - USB Device Class Definition for Terminal Types (termt10.pdf)
 */

#ifndef _USBD_AUDIO_H_
#define _USBD_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Audio Interface Subclass Codes
 * Refer to Table A-2 from audio10.pdf
 */
enum usb_audio_int_subclass_codes {
	USB_AUDIO_SUBCLASS_UNDEFINED	= 0x00,
	USB_AUDIO_AUDIOCONTROL		= 0x01,
	USB_AUDIO_AUDIOSTREAMING	= 0x02,
	USB_AUDIO_MIDISTREAMING		= 0x03
};

/** Audio Class-Specific AC Interface Descriptor Subtypes
 * Refer to Table A-5 from audio10.pdf
 */
enum usb_audio_cs_ac_int_desc_subtypes {
	USB_AUDIO_AC_DESCRIPTOR_UNDEFINED	= 0x00,
	USB_AUDIO_HEADER			= 0x01,
	USB_AUDIO_INPUT_TERMINAL		= 0x02,
	USB_AUDIO_OUTPUT_TERMINAL		= 0x03,
	USB_AUDIO_MIXER_UNIT			= 0x04,
	USB_AUDIO_SELECTOR_UNIT			= 0x05,
	USB_AUDIO_FEATURE_UNIT			= 0x06,
	USB_AUDIO_PROCESSING_UNIT		= 0x07,
	USB_AUDIO_EXTENSION_UNIT		= 0x08
};

/** Audio Class-Specific AS Interface Descriptor Subtypes
 * Refer to Table A-6 from audio10.pdf
 */
enum usb_audio_cs_as_int_desc_subtypes {
	USB_AUDIO_AS_DESCRIPTOR_UNDEFINED	= 0x00,
	USB_AUDIO_AS_GENERAL			= 0x01,
	USB_AUDIO_FORMAT_TYPE			= 0x02,
	USB_AUDIO_FORMAT_SPECIFIC		= 0x03
};

/** Audio Class-Specific Request Codes
 * Refer to Table A-9 from audio10.pdf
 */
enum usb_audio_cs_req_codes {
	USB_AUDIO_REQUEST_CODE_UNDEFINED	= 0x00,
	USB_AUDIO_SET_CUR			= 0x01,
	USB_AUDIO_GET_CUR			= 0x81,
	USB_AUDIO_SET_MIN			= 0x02,
	USB_AUDIO_GET_MIN			= 0x82,
	USB_AUDIO_SET_MAX			= 0x03,
	USB_AUDIO_GET_MAX			= 0x83,
	USB_AUDIO_SET_RES			= 0x04,
	USB_AUDIO_GET_RES			= 0x84,
	USB_AUDIO_SET_MEM			= 0x05,
	USB_AUDIO_GET_MEM			= 0x85,
	USB_AUDIO_GET_STAT			= 0xFF
};

/** Feature Unit Control Selectors
 * Refer to Table A-11 from audio10.pdf
 */
enum usb_audio_fucs {
	USB_AUDIO_FU_CONTROL_UNDEFINED		= 0x00,
	USB_AUDIO_FU_MUTE_CONTROL		= 0x01,
	USB_AUDIO_FU_VOLUME_CONTROL		= 0x02,
	USB_AUDIO_FU_BASS_CONTROL		= 0x03,
	USB_AUDIO_FU_MID_CONTROL		= 0x04,
	USB_AUDIO_FU_TREBLE_CONTROL		= 0x05,
	USB_AUDIO_FU_GRAPHIC_EQUALIZER_CONTROL	= 0x06,
	USB_AUDIO_FU_AUTOMATIC_GAIN_CONTROL	= 0x07,
	USB_AUDIO_FU_DELAY_CONTROL		= 0x08,
	USB_AUDIO_FU_BASS_BOOST_CONTROL		= 0x09,
	USB_AUDIO_FU_LOUDNESS_CONTROL		= 0x0A
};

/** USB Terminal Types
 * Refer to Table 2-1 - Table 2-4 from termt10.pdf
 */
enum usb_audio_terminal_types {
	/* USB Terminal Types */
	USB_AUDIO_USB_UNDEFINED   = 0x0100,
	USB_AUDIO_USB_STREAMING   = 0x0101,
	USB_AUDIO_USB_VENDOR_SPEC = 0x01FF,

	/* Input Terminal Types */
	USB_AUDIO_IN_UNDEFINED      = 0x0200,
	USB_AUDIO_IN_MICROPHONE     = 0x0201,
	USB_AUDIO_IN_DESKTOP_MIC    = 0x0202,
	USB_AUDIO_IN_PERSONAL_MIC   = 0x0203,
	USB_AUDIO_IN_OM_DIR_MIC     = 0x0204,
	USB_AUDIO_IN_MIC_ARRAY      = 0x0205,
	USB_AUDIO_IN_PROC_MIC_ARRAY = 0x0205,

	/* Output Terminal Types */
	USB_AUDIO_OUT_UNDEFINED		= 0x0300,
	USB_AUDIO_OUT_SPEAKER		= 0x0301,
	USB_AUDIO_OUT_HEADPHONES	= 0x0302,
	USB_AUDIO_OUT_HEAD_AUDIO	= 0x0303,
	USB_AUDIO_OUT_DESKTOP_SPEAKER	= 0x0304,
	USB_AUDIO_OUT_ROOM_SPEAKER	= 0x0305,
	USB_AUDIO_OUT_COMM_SPEAKER	= 0x0306,
	USB_AUDIO_OUT_LOW_FREQ_SPEAKER	= 0x0307,

	/* Bi-directional Terminal Types */
	USB_AUDIO_IO_UNDEFINED			= 0x0400,
	USB_AUDIO_IO_HANDSET			= 0x0401,
	USB_AUDIO_IO_HEADSET			= 0x0402,
	USB_AUDIO_IO_SPEAKERPHONE_ECHO_NONE	= 0x0403,
	USB_AUDIO_IO_SPEAKERPHONE_ECHO_SUP	= 0x0404,
	USB_AUDIO_IO_SPEAKERPHONE_ECHO_CAN	= 0x0405,
};

enum usb_audio_direction {
	USB_AUDIO_IN = 0x00,
	USB_AUDIO_OUT = 0x01
};

/**
 * Addressable logical object inside an audio function.
 * Entity is one of: Terminal or Unit.
 * Refer to 1.4 Terms and Abbreviations from audio10.pdf
 */
struct usb_audio_entity {
	enum usb_audio_cs_ac_int_desc_subtypes subtype;
	uint8_t id;
};

/**
 * @warning Size of baInterface is 2 just to make it useable
 * for all kind of devices: headphones, microphone and headset.
 * Actual size of the struct should be checked by reading
 * .bLength.
 */
struct cs_ac_if_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdADC;
	uint16_t wTotalLength;
	uint8_t bInCollection;
	uint8_t baInterfaceNr[2];
} __packed;

struct input_terminal_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bTerminalID;
	uint16_t wTerminalType;
	uint8_t bAssocTerminal;
	uint8_t bNrChannels;
	uint16_t wChannelConfig;
	uint8_t iChannelNames;
	uint8_t iTerminal;
} __packed;

/**
 * @note Size of Feature unit descriptor is not fixed.
 * This structure is just a helper not a common type.
 */
struct feature_unit_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bUnitID;
	uint8_t bSourceID;
	uint8_t bControlSize;
	uint16_t bmaControls[1];
} __packed;

struct output_terminal_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bTerminalID;
	uint16_t wTerminalType;
	uint8_t bAssocTerminal;
	uint8_t bSourceID;
	uint8_t iTerminal;
} __packed;

struct as_cs_interface_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bDescriptorSubtype;
	uint8_t  bTerminalLink;
	uint8_t  bDelay;
	uint16_t wFormatTag;
} __packed;

struct format_type_i_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bFormatType;
	uint8_t bNrChannels;
	uint8_t bSubframeSize;
	uint8_t bBitResolution;
	uint8_t bSamFreqType;
	uint8_t tSamFreq[3];
} __packed;

struct std_as_ad_endpoint_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
	uint8_t bRefresh;
	uint8_t bSynchAddress;
} __packed;

struct cs_as_ad_ep_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmAttributes;
	uint8_t bLockDelayUnits;
	uint16_t wLockDelay;
} __packed;
#ifdef __cplusplus
}
#endif

#endif /* _USB_AUDIO_H_ */
