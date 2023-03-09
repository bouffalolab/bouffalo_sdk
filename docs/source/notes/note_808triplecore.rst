BL808 三核启动流程
=======================

本节主要介绍 BL808 三核的启动流程。首先我们需要知道以下信息：

- M0 使用 t-head E907 ,D0 使用 t-head C906，LP 使用 t-head e902，为异构三核，
- M0 和 D0 外设共享，但是中断不共享，M0 能触发的中断不一定能在 D0上触发，比如 UART0，中断只能在 M0 上使用，同理 LP 也是
- M0 会启动其他两个核

SystemInit 干了什么
-------------------------

``SystemInit`` 函数位于 `soc/bl808/startup/m0` 和 `soc/bl808/startup/d0` `soc/bl808/startup/lp` 中的 `system_bl808.c` 中。

系统启动时，会根据 ld 文件中设置的 entry(__start) 去找 ``__start`` 函数, 这个便是我们芯片启动的入口，位于 `start.S` 中。 `start.S` 中只是简单的设置异常和中断向量表以及 sp，最终调用 ``SystemInit`` 函数。
所以我们先了解 ``SystemInit`` 函数干了什么。

- LP ``SystemInit``

初始化 CLIC，其他什么也没有

- D0 ``SystemInit``

一上来就是一个 while 死循环，系统启动时，这个条件一定是不会满足的，所以 D0 一上来就会在这死循环，等待条件成立。

.. code-block:: c
   :linenos:

   while ((BL_RD_WORD(IPC_SYNC_ADDR1) != IPC_SYNC_FLAG) ||
   (BL_RD_WORD(IPC_SYNC_ADDR2) != IPC_SYNC_FLAG)) {
    // clang-format off
    __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP();
    // clang-format on
    /* if cache is off, comment this and this api need to start load itcm*/
    // L1C_DCache_Invalid_By_Addr(IPC_SYNC_ADDR1, 8);
    };
    /* clear this flags for system reboot */
    BL_WR_WORD(IPC_SYNC_ADDR1, 0x0);
    BL_WR_WORD(IPC_SYNC_ADDR2, 0x0);

- M0 ``SystemInit``

主要是 pmp 和 clic 的配置，以及一些寄存器的复位

.. code-block:: c
   :linenos:

    /* CPU Prefetching barrier */
    Tzc_Sec_PSRAMA_Access_Set_Not_Lock(0, 0x0, 64 * 1024 * 1024, 0);
    Tzc_Sec_PSRAMB_Access_Set_Not_Lock(0, 0x0, 64 * 1024 * 1024, 0);
    pmp_init();

    /* enable mstatus FS */
    uint32_t mstatus = __get_MSTATUS();
    mstatus |= (1 << 13);
    __set_MSTATUS(mstatus);

    /* enable mxstatus THEADISAEE */
    uint32_t mxstatus = __get_MXSTATUS();
    mxstatus |= (1 << 22);
    /* enable mxstatus MM */
    mxstatus |= (1 << 15);
    __set_MXSTATUS(mxstatus);

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    /* Every interrupt should be clear by software*/
    for (i = 0; i < IRQn_LAST; i++) {
        CLIC->CLICINT[i].IE = 0;
        CLIC->CLICINT[i].IP = 0;
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
    }

    /* tspend interrupt will be clear auto*/
    /* tspend use positive interrupt */
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x3;

    csi_dcache_enable();
    csi_icache_enable();

    /* enable preload $ AMR for D$ */
    __set_MHINT(0x000c);

    /* disable mexstatus SPUSHEN and SPSWAPEN for ipush/ipop*/
    uint32_t mexstatus = __get_MEXSTATUS();
    mexstatus &= ~(0x3 << 16);
    __set_MEXSTATUS(mexstatus);

根据上述信息我们知道， M0 一定会在某个时刻，向这两个寄存器写入 ``IPC_SYNC_FLAG``, 从而让 D0 运行起来。而 LP 是没有条件限制的，M0 控制运行就可以了。

board_init
------------

然后就是进入 ``main`` 函数了, ``main`` 函数一开始会调用 ``board_init`` 去初始化一些内容，这里我们只需要关心 M0, 因为在此其他两个核还没有开始运行。