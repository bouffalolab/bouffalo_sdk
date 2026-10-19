#include "bl602_glb.h"
#include "bl602_mfg_efuse.h"
#include "bl602_ef_ctrl.h"

static uint8_t rf_cal_slots=3;
extern void main(void);
#define RF_CAL_SLOT_CFG_OFFSET      (4*13)
#ifdef BFLB_MCU_SDK
#include "bflb_platform.h"
#define mfg_print   MSG
#else
#define mfg_print   printf
#endif

int8_t mfg_efuse_read_macaddr(uint8_t mac[6],uint8_t reload)
{
    uint8_t slot=0xff;
    BL_Err_Type ret=ERROR;
    uint8_t hdiv=0,bdiv=0;
    GLB_ROOT_CLK_Type rtClk=GLB_Get_Root_CLK_Sel();

    bdiv=GLB_Get_BCLK_Div();
    hdiv=GLB_Get_HCLK_Div();

#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if(rf_cal_slots>=3&&(!EF_Ctrl_Is_MAC_Address_Slot_Empty(2,reload))){
        slot=2;
    }else if(rf_cal_slots>=2&&(!EF_Ctrl_Is_MAC_Address_Slot_Empty(1,reload))){
        slot=1;
    }else if(rf_cal_slots>=1&&(!EF_Ctrl_Is_MAC_Address_Slot_Empty(0,reload))){
        slot=0;
    }

    if(slot!=0xff){
        mfg_print("Read slot:%d\r\n",slot);
        ret=EF_Ctrl_Read_MAC_Address_Opt(slot,mac,reload);
    }else{
        mfg_print("No written slot found\r\n");
    }

    GLB_Set_System_CLK_Div(hdiv,bdiv);
    HBN_Set_ROOT_CLK_Sel((HBN_ROOT_CLK_Type)rtClk);
#endif
    if(ret==SUCCESS){
        return 0;
    }else{
        return -1;
    }
}

