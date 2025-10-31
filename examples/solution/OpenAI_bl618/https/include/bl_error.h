
#ifndef __BL_ERROR_H__
#define __BL_ERROR_H__

#define BL_TCP_NO_ERROR                 (0)      /**< No error */
#define BL_TCP_ARG_INVALID             (-2)      /**< Invalid parameter  */
#define BL_TCP_CREATE_CONNECT_ERR     (-21)      /**< Error creating TCP connection */
#define BL_TCP_SEND_ERR               (-23)      /**< TCP sending failed  */
#define BL_TCP_READ_ERR               (-25)      /**< TCP reading failed */
#define BL_TCP_CONNECT_TIMEOUT        (-26)      /**< TCP connection timeout  */
#define BL_TCP_CONNECT_ERR            (-27)      /**< TCP connection failed  */
#define BL_TCP_CONNECTING             (-28)      /**< TCP connecting in progress    */
#define BL_TCP_READ_INCOMPLETED       (-29)      /**< Incomplete TCP packet reading  */
#define BL_TCP_CREATE_SERVER_ERR      (-31)      /**< Error creating TCP server  */
#define BL_TCP_SERVER_WAIT_CONNECT    (-32)      /**< TCP waiting for client connection  */
#define BL_TCP_DNS_PARSING            (-35)      /**< DNS parsing in progress */
#define BL_TCP_DNS_PARSE_ERR          (-36)      /**< DNS parsing failed */
#define BL_TCP_DNS_PARSE_TIMEOUT      (-37)      /**< DNS parsing timeout */

#endif /* #ifndef __BL_ERROR_H__ */
