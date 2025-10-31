/**
 ****************************************************************************************
 *
 * @file fhost_ipc.h
 *
 * @brief IPC task for Fully Hosted firmware.
 *
 *
 ****************************************************************************************
 */
#ifndef _FHOST_IPC_H_
#define _FHOST_IPC_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "macsw.h"
#include "rtos_al.h"

/**
 ****************************************************************************************
 * @brief Find the interface index
 *
 * @param[in] name Name of the interace
 * @return Index of the FHOST wifi interface or -1 if interface doesn't exist.
 ****************************************************************************************
*/
int fhost_search_itf(char *name);

/**
 ****************************************************************************************
 * @brief Find the first valid network interface.
 *
 * @return Index of the FHOST wifi interface.
 ****************************************************************************************
*/
int fhost_search_first_valid_itf();

/**
 ****************************************************************************************
 * @brief read AKM from string and convert it to bitfield
 *
 * Several AKM can be present in the string, separated by coma.
 * Invalid AKM are ignored.
 *
 * @param[in]  akm_str  String containing AKM list
 * @param[out] akm      Bitfield of AKM read
 * @param[in]  show_error  Print error message if invalid cipher found
 *
 * @return number of invalid AKM found
 ****************************************************************************************
*/
int fhost_ipc_read_akm(char *akm_str, uint32_t *akm, bool show_error);

/**
 ****************************************************************************************
 * @defgroup FHOST_IPC FHOST_IPC
 * @ingroup FHOST
 *
 * This module gather all code related to the "IPC" application which is the main
 * application used to test the fully hosted firmware.
 * This application provides a simple "Command line interface" to launch different type
 * of tests. The list of supported commands is defined in @ref ipc_commands
 *
 * @{
 ****************************************************************************************
*/

/*
 * DEFINITIONS
 ****************************************************************************************
 */
/// IPC Command result
enum fhost_ipc_res {
    FHOST_IPC_SUCCESS,
    FHOST_IPC_ERROR,
    FHOST_IPC_UNKWN_CMD,
    FHOST_IPC_NO_RESP,
    FHOST_IPC_SHOW_USAGE,
};

/// Sigkill callback function type
typedef int (*fhost_ipc_sigkill_cb)(rtos_task_handle);
/// Start task function type
typedef rtos_task_handle (*fhost_ipc_start_task_cb)(void *);

/**
 ****************************************************************************************
 * @brief Send a message to the IPC TASK.
 *
 * Push a ipc message from the host to the IPC task.
 * To be called from the IPC irq handler, hence it cannot wait and will returned an error
 * if there is no more space to write this message.
 * IPC task will acknowledge the buffer to host after reading it.
 *
 * @param[in] id       Message id. (Used by host to identify the response)
 * @param[in] msg_data Buffer containing the message.
 * @param[in] len      Length, in bytes, of the message.
 * @param[in] isr      flag of irq
 *
 * @return 0 on success and != 0 if error occured.
 ****************************************************************************************
 */
int fhost_ipc_write_msg(int id, void *msg_data, int len, bool isr);

/**
 ****************************************************************************************
 * @brief Indicate to the host that a FW dump is available
 *
 * Simply inform host that firmware uploaded a dump.
 * Include size of dump so that host can check that its dump structure definition is
 * aligned.
 ****************************************************************************************
 */
void fhost_ipc_error_ind(void);

/**
 ****************************************************************************************
 * @brief Send Command confirmation to host
 *
 * Simply send to the host a confirmation for the message handled by the task with
 * task handle provided as input.
 *
 * @param[in] handle  Task handle of RTOS task sending the confirmation
 * @param[in] status  Confirmation status (@ref fhost_ipc_res)
 ****************************************************************************************
 */
void fhost_ipc_send_cmd_cfm(rtos_task_handle handle, uint32_t status);

/**
 ****************************************************************************************
 * @brief Convert string containing MAC address
 *
 * The string may should be of the form xx:xx:xx:xx:xx:xx
 *
 * @param[in]  str   String to parse
 * @param[out] addr  Updated with MAC address
 * @return 0 if string contained what looks like a valid MAC address and -1 otherwise
 ****************************************************************************************
 */
int fhost_ipc_parse_mac_addr(const char *str, struct mac_addr *addr);

/**
 ****************************************************************************************
 * @brief print AKM suite name
 *
 * @param[in] suite  AKM suite value
 * @param[in] sep    Whether to print separator first
 ****************************************************************************************
 */
void fhost_ipc_print_akm(enum mac_akm_suite suite, bool sep);

/**
 ****************************************************************************************
 * @brief print Cipher suite name
 *
 * @param[in] suite  Cipher suite value
 * @param[in] sep    Whether to print separator first
 ****************************************************************************************
 */
void fhost_ipc_print_cipher(enum mac_cipher_suite suite, bool sep);

/**
 ****************************************************************************************
 * @brief Convert string containing ip address
 *
 * The string may should be of the form a.b.c.d/e (/e being optional)
 *
 * @param[in]  str   String to parse
 * @param[out] ip    Updated with the numerical value of the ip address
 * @param[out] mask  Updated with the numerical value of the network mask
 *                   (or 32 if not present)
 * @return 0 if string contained what looks like a valid ip address and -1 otherwise
 ****************************************************************************************
 */
int fhost_ipc_parse_ip4(char *str, uint32_t *ip, uint32_t *mask);

/**
 ****************************************************************************************
 * @brief Start an ipc task saving the task handle.
 *
 * @param[in] msg_id            Message ID
 * @param[in] start_task_func   Function that starts the ipc task
 * @param[in] start_task_args   Argument pointer to pass to start_task_func
 * @param[in] sigkill_func      Callback for SIGKILL signal
 *
 * @return FHOST_IPC_NO_RESP if successful, FHOST_IPC_ERROR otherwise.
 ****************************************************************************************
 */
int fhost_ipc_start_task(int msg_id, fhost_ipc_start_task_cb start_task_func,
						 void *start_task_args, fhost_ipc_sigkill_cb sigkill_func);

char *fhost_ipc_next_token(char **params);

/**
 * @}
 */
#endif /* _FHOST_IPC_H_ */
