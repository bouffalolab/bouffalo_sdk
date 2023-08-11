#ifndef  _COMMON_CLIENT_H_
#define  _COMMON_CLIENT_H_

#include <stdint.h>
#include <types.h>
#include <stdbool.h> 

#define MESH_TRANS_MIC_SIZE_4    4
#define MESH_TRANS_MIC_SIZE_8    8
#define MESH_GEN_OPCODE_SIZE_1   1
#define MESH_GEN_OPCODE_SIZE_2   2

/** Parameters of Generic OnOff Set. */
typedef struct {
    bool    op_en;      /*!< Indicate if optional parameters are included */
    uint8_t onoff;      /*!< Target value of Generic OnOff state */
    uint8_t tid;        /*!< Transaction ID */
    uint8_t trans_time; /*!< Time to complete state transition (optional) */
    uint8_t delay;      /*!< Indicate message execution delay (C.1) */
} bt_mesh_gen_onoff_set_t;

typedef union {
    bt_mesh_gen_onoff_set_t          onoff_set;            /*!< For BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET & BFL_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK */
} bt_mesh_generic_client_set_state_t;

#endif
