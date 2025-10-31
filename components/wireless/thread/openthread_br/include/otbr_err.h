
#ifndef __OTBR_ERR_H__
#define __OTBR_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

enum otbrError
{
    OTBR_ERROR_NONE               = 0, ///< No error.

    OTBR_ERROR_ERRNO              = -1,  ///< Error defined by errno.
    OTBR_ERROR_DBUS               = -2,  ///< DBus error.
    OTBR_ERROR_MDNS               = -3,  ///< mDNS error.
    OTBR_ERROR_OPENTHREAD         = -4,  ///< OpenThread error.
    OTBR_ERROR_REST               = -5,  ///< Rest Server error.
    OTBR_ERROR_SMCROUTE           = -6,  ///< SMCRoute error.
    OTBR_ERROR_NOT_FOUND          = -7,  ///< Not found.
    OTBR_ERROR_PARSE              = -8,  ///< Parse error.
    OTBR_ERROR_NOT_IMPLEMENTED    = -9,  ///< Not implemented error.
    OTBR_ERROR_INVALID_ARGS       = -10, ///< Invalid arguments error.
    OTBR_ERROR_DUPLICATED         = -11, ///< Duplicated operation, resource or name.
    OTBR_ERROR_ABORTED            = -12, ///< The operation is aborted.
    OTBR_ERROR_INVALID_STATE      = -13, ///< The target isn't in a valid state.
    OTBR_ERROR_INFRA_LINK_CHANGED = -14, ///< The infrastructure link is changed.
    OTBR_ERROR_NO_MEM             = -15, ///< No memory available.
};

#ifdef __cplusplus
}
#endif

#endif /* __OTBR_ERR_H__ */