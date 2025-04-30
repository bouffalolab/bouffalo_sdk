#ifndef OPENTHREAD_PORT_H
#define OPENTHREAD_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>

#include <openthread-core-config.h>
#include <openthread/thread.h>
#include <openthread/logging.h>

#define VERSION_OT_SRC_MAJOR 1
#define VERSION_OT_SRC_MINOR 6
#define VERSION_OT_SRC_PATCH 18

// #define VERSION_OT_SRC_EXTRA_INFO "customer-1"

#ifndef OT_TASK_SIZE
#define OT_TASK_SIZE 1024
#endif

#ifndef OT_TASK_PRORITY
#define OT_TASK_PRORITY 20
#endif

#ifndef OT_UART_RX_BUFFSIZE
#define OT_UART_RX_BUFFSIZE 256
#endif

#ifndef OTRADIO_RX_FRAME_BUFFER_NUM
#define OTRADIO_RX_FRAME_BUFFER_NUM         8
#endif

typedef union {
    struct {
        uint16_t isCoexEnable:1;
        uint16_t isFtd:1;
        uint16_t isLinkMetricEnable:1;
        uint16_t isCSLReceiverEnable:1;
        uint16_t isTimeSyncEnable:1;
        uint16_t isImmAck4Error:1;
        uint16_t isCslPhaseUpdated:1;
        uint16_t isTxTimestampValid:1;
        uint16_t isWiFiCoexEnable:1;
        uint16_t unused:7;
    } bf;
    uint16_t byte;
} __packed otRadio_opt_t;

typedef enum _ot_system_event {
    OT_SYSTEM_EVENT_NONE                                = 0,

    OT_SYSTEM_EVENT_OT_TASKLET                          = 0x00000001,

    OT_SYSTEM_EVENT_ALARM_MS_EXPIRED                    = 0x00000002,
    OT_SYSTEM_EVENT_ALARM_US_EXPIRED                    = 0x00000004,
    OT_SYSTEM_EVENT_ALARM_ALL_MASK                      = OT_SYSTEM_EVENT_ALARM_MS_EXPIRED | OT_SYSTEM_EVENT_ALARM_US_EXPIRED,

    OT_SYSTEM_EVENT_UART_TXR                            = 0x00000010,
    OT_SYSTEM_EVENT_UART_TXD                            = 0x00000020,
    OT_SYSTEM_EVENT_UART_RXD                            = 0x00000040,
    OT_SYSETM_EVENT_UART_ALL_MASK                       = OT_SYSTEM_EVENT_UART_TXR | OT_SYSTEM_EVENT_UART_TXD | OT_SYSTEM_EVENT_UART_RXD,
    
    OT_SYSTEM_EVENT_SERIAL_RX_DONE                      = 0x00000080,
    OT_SYSTEM_EVENT_SERIAL_ALL_MASK                     = OT_SYSTEM_EVENT_SERIAL_RX_DONE,

    OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ            = 0x00000100,
    OT_SYSTEM_EVENT_RADIO_TX_ERROR                      = 0x00000200,
    OT_SYSTEM_EVENT_RADIO_TX_ACKED                      = 0x00000400,
    OT_SYSTEM_EVENT_RADIO_TX_NO_ACK                     = 0x00000800,
    OT_SYSTEM_EVENT_RADIO_TX_ABORT                      = 0x00001000,
    OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK                   = OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ | 
        OT_SYSTEM_EVENT_RADIO_TX_ERROR | OT_SYSTEM_EVENT_RADIO_TX_ACKED | OT_SYSTEM_EVENT_RADIO_TX_NO_ACK | OT_SYSTEM_EVENT_RADIO_TX_ABORT,

    OT_SYSTEM_EVENT_RADIO_RX_DONE                       = 0x00002000,
    OT_SYSTEM_EVENT_RADIO_RX_CRC_FIALED                 = 0x00004000,
    OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF                    = 0x00008000,
    OT_SYSTEM_EVENT_RADIO_RX_ALL_MASK                   = OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF | 
        OT_SYSTEM_EVENT_RADIO_RX_DONE | OT_SYSTEM_EVENT_RADIO_RX_CRC_FIALED,
    OT_SYSTEM_EVENT_RADIO_ALL_MASK                      = OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK | OT_SYSTEM_EVENT_RADIO_RX_ALL_MASK,

    OT_SYSTEM_EVENT_POLL                                = 0x00010000,
    OT_SYSTEM_EVENT_POLL_DATA_TIMEOUT                   = 0x00020000,
    OT_SYSTEM_EVENT_FULL_STACK                          = 0x00040000,
    OT_SYSTEM_EVENT_RESET_NEXT_POLL                     = 0x00080000,
    OT_SYSTEM_EVENT_MAC_TX_RETRY                        = 0x00100000,
    OT_SYSTEM_EVENT_CSL_TIMER                           = 0x00200000,

    OT_SYSTEM_EVENT_OTBR_MASK                           = 0x00800000,
    OT_SYSTEM_EVENT_APP_MASK                            = 0xff000000,

    OT_SYSTEM_EVENT_ALL                                 = 0xffffffff,
} ot_system_event_t;

extern ot_system_event_t ot_system_event_var;

/****************************************************************************//**
 * @brief  Start OpenThread task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrStart(otRadio_opt_t opt);

/****************************************************************************//**
 * @brief  Get current OpenThread instance.
 *
 * @param  None
 *
 * @return otInstance instance
 *
*******************************************************************************/
otInstance *otrGetInstance();

/****************************************************************************//**
 * @brief  Init openthread tack.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrStackInit(void);

/****************************************************************************//**
 * @brief  Initialize user code with OpenThread related before OpenThread 
 *          main event loop execution. This function is called after
 *          Openthread instance created and by OpenThread task.
 *
 * @param  aInstance, Open Thread instance 
 *
 * @return None
 *
*******************************************************************************/
void otrInitUser(otInstance * instance);

/****************************************************************************//**
 * @brief  Open Thread infra network interface process
 *
 * @param  aInstance, Open Thread instance 
 *
 * @return None
 *
*******************************************************************************/
void otbr_netif_process(otInstance *aInstance);

/****************************************************************************//**
 * @brief  Initalize alarm timer task
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_alarmInit(void);

/****************************************************************************//**
 * @brief  Handle alarm timer task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_alarmTask(ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  Init radio.
 *
 * @param  opt, radio work optional
 *
 * @return None
 *
*******************************************************************************/
void ot_radioInit(otRadio_opt_t opt);

/****************************************************************************//**
 * @brief  Init uart.
 *
 * @param  uart, uart handle
 *
 * @return None
 *
*******************************************************************************/
void ot_uart_init(void *uart);

/****************************************************************************//**
 * @brief  Handle radio task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_radioTask(ot_system_event_t trxEvent);

/****************************************************************************//**
 * @brief  An weak function explore to execute serial protocol code.
 *
 * @param  sevent, event bit called by OT_APP_NOTIFY_ISR/OT_APP_NOTIFY
 *
 * @return None
 *
*******************************************************************************/
void ot_serialProcess(ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  init for openthread cli command.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
extern void otAppCliInit(otInstance *aInstance);

/****************************************************************************//**
 * @brief  init for openthread ncp command.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
extern void otAppNcpInit(otInstance *aInstance);

/****************************************************************************//**
 * @brief  Give semphore to OpenThread task
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otSysEventSignalPending(void);

/****************************************************************************//**
 * @brief  Lock OpenThread task to acquire ciritcal section access
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrLock(void);

/****************************************************************************//**
 * @brief  Unlock OpenThread task to release ciritcal section access
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrUnlock(void);

/****************************************************************************//**
 * @brief  Check whether current task conext is Thread Task
 *
 * @param  None
 *
 * @return ture, it is under Thread Task conext
 *
*******************************************************************************/
bool otrIsThreadTask(void);

/****************************************************************************//**
 * @brief  Check whether Thread stack is idle
 *
 * @param  None
 *
 * @return ture, Thread stack is idle.
 *
*******************************************************************************/
bool otr_isStackIdle(void);

/****************************************************************************//**
 * @brief  Turn off interrupt
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint32_t otrEnterCrit(void);

/****************************************************************************//**
 * @brief  Turn on interrupt
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrExitCrit(uint32_t tag);

/****************************************************************************//**
 * @brief  Post ot_system_event_t event to Thread stack
 *
 * @param  sevent, system event, please refer to ot_system_event_t
 *
 * @return None
 *
*******************************************************************************/
void otrNotifyEvent(ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  Get system event posted by otrNotifyEvent, and clean system event.
 *
 * @param  None
 *
 * @return system event
 *
*******************************************************************************/
ot_system_event_t otrGetNotifyEvent(void);

/****************************************************************************//**
 * @brief  Get OTBR hostname
 *
 * @param  None
 *
 * @return hostname string
 *
*******************************************************************************/
char * otbr_hostname(void);

/****************************************************************************//**
 * @brief  Macro OT_THREAD_SAFE provides a method to access OpenThread with 
 * thread-safe in other tasks running context.
 * 
 * ot_initUser and OpenThread callback functions already are thread-safe protected.
 * 
 * Note, do NOT call return in this macro block.
 * 
 * @param ...  OpenThread api call statement
 *
*******************************************************************************/
#define OT_THREAD_SAFE(...)                 \
    otrLock();                              \
    do                                      \
    {                                       \
        __VA_ARGS__;                        \
    } while (0);                            \
    otrUnlock();                            

/****************************************************************************//**
 * @brief  Macro OT_THREAD_SAFE_RET provides a method to access OpenThread with 
 * thread-safe in other tasks running context.
 * 
 * ot_initUser and OpenThread callback functions already are thread-safe protected.
 * 
 * Note, do NOT call return in this macro block.
 *
 * @param ret   return value
 * @param ...   OpenThread api call statement
 *
*******************************************************************************/
#define OT_THREAD_SAFE_RET(ret, ...)        \
    do                                      \
    {                                       \
        otrLock();                          \
        (ret) = __VA_ARGS__;                \
        otrUnlock();                        \
    } while (0)                             


/****************************************************************************//**
 * @brief  An weak function explore to applicaton layer to execute some application code.
 *          Note,
 *              1, this function is running in openthread main task, it is thread safed and
 *              it doesn't need OT_THREAD_SAFE/OT_THREAD_SAFE_RET protection.
 *              2, as it running with main task event let and radio/timer driver,
 *                  this function should be implemented very light without too
 *                  much execution time cost and delay or pending. And also do NOT
 *                  suspend and stop this task.
 *                  heavy execution or any delay or pending for some state transition should be
 *                  executed in another task.
 *
 * @param  sevent, event bit called by OT_APP_NOTIFY_ISR/OT_APP_NOTIFY from application code
 *
 * @return None
 *
*******************************************************************************/
void otrAppProcess(ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  An interface to application to invoke openthread task to execute otrAppProcess, 
 *          which is used to call in application interrupt context.
 *
 * @param  ebit, event bit for otrAppProcess. 
 *              please reference to OT_SYSTEM_EVENT_APP for valid bits.
 *
 * @return None
 *
*******************************************************************************/
#define OT_APP_NOTIFY_ISR(ebit)             otrNotifyEvent(ebit & OT_SYSTEM_EVENT_APP)


/****************************************************************************//**
 * @brief  An interface to application to invoke openthread task to execute otrAppProcess, 
 *          which is used to call in application task context.
 *
 * @param  ebit, event bit for otrAppProcess. 
 *              please reference to OT_SYSTEM_EVENT_APP for valid bits.
 *
 * @return None
 *
*******************************************************************************/
#define OT_APP_NOTIFY(ebit)                 otrNotifyEvent(ebit & OT_SYSTEM_EVENT_APP)

#if defined (BL702) || defined (BL702L)
void ot_uartSetFd(int fd);
#endif

#ifdef __cplusplus
}
#endif

#endif // OPENTHREAD_PORT_H
