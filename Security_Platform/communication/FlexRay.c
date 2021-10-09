/*
 * FlexRay.c
 *
 *  Created on: 2021年8月6日
 *      Author: 15755
 */

/****
 * GSN0-GSN1 数据传输的准确性
 *
 *
 *
 * ****/

#include"FlexRay.h"
#include"HL_sys_vim.h"
FLEXRAY_McanMESSAGE_BOX FLEXRAYA_McanMESSAGE_BOX_TABLE[FLEXRAY_MCANMESSAGE_BOX_NUM]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
FLEXRAY_McanMESSAGE_BOX FLEXRAYB_McanMESSAGE_BOX_TABLE[FLEXRAY_MCANMESSAGE_BOX_NUM]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

wrhs Fr_LPduPtr;
bc Fr_LSduPtr;
FLEXRAY_MSG_BUFF FLEXRAYA_Msg_Buff;
FLEXRAY_MSG_BUFF FLEXRAYB_Msg_Buff;
void FLEXRAY_init(FLEXRAY_Info flexrayx)
{
    FlexRayREG->GCS |=  (uint32)((uint32)1U << 31U) |        /*Endianness Correction on VBusp Master  1--ON*/
                        (uint32)((uint32)1U << 30U) |        /*Endianness correction on VBusp Slave   1--ON*/
                        (uint32)((uint32)1U << 28U) |        /*Endianness Correction for No (header or payload) Data Sink Access */
                        (uint32)((uint32)0U << 26U) |        /*Endianness Correction for Header */
                        (uint32)((uint32)0U << 24U) |        /*Endianness Correction for Payload. */
                        (uint32)((uint32)0U << 21U) |        /*Transfer Priority */
                        (uint32)((uint32)0U << 16U) |        /*ECC Lock */
                        (uint32)((uint32)1U << 14U) |        /*Clear ETESM Register */
                        (uint32)((uint32)1U << 13U) |        /*Clear TTCC Register */
                        (uint32)((uint32)1U << 12U) |        /*Clear TTSM Register */
                        (uint32)((uint32)0U << 8U) |         /*Enable Transfer Status Mirrored. */
                        (uint32)((uint32)1U << 5U) |         /*Status Interrupt Line Enable. */
                        (uint32)((uint32)0U << 4U) |         /*Error Interrupt Line Enable. */
                        (uint32)((uint32)0U << 1U) |         /*Transfer Unit Halted. */
                        (uint32)((uint32)1U << 0U) ;         /*Transfer Unit Enabled. */

    FlexRayREG->GCR |= (uint32)((uint32)0U << 31U) |        /*Endianness Correction on VBusp Master  1--ON*/
                       (uint32)((uint32)0U << 30U) |        /*Endianness correction on VBusp Slave   1--ON*/
                       (uint32)((uint32)0U << 28U) |        /*Endianness Correction for No (header or payload) Data Sink Access */
                       (uint32)((uint32)1U << 26U) |        /*Endianness Correction for Header */
                       (uint32)((uint32)1U << 24U) |        /*Endianness Correction for Payload. */
                       (uint32)((uint32)1U << 21U) |        /*Transfer Priority */
                       (uint32)((uint32)1U << 16U) |        /*ECC Lock */
                       (uint32)((uint32)0U << 14U) |        /*Clear ETESM Register */
                       (uint32)((uint32)0U << 13U) |        /*Clear TTCC Register */
                       (uint32)((uint32)0U << 12U) |        /*Clear TTSM Register */
                       (uint32)((uint32)1U << 8U) |         /*Enable Transfer Status Mirrored. */
                       (uint32)((uint32)0U << 5U) |         /*Status Interrupt Line Enable. */
                       (uint32)((uint32)1U << 4U) |         /*Error Interrupt Line Enable. */
                       (uint32)((uint32)1U << 1U) |         /*Transfer Unit Halted. */
                       (uint32)((uint32)0U << 0U) ;         /*Transfer Unit Enabled. */
    FlexRayREG->TSCB |= (uint32)((uint32)1U << 8U) ;         /*Detects Transfer State Machine State IDLE */
    // Wait for PBSY bit to clear - POC not busy.
    // 1: Signals that the POC is busy and cannot accept a command from the host. CMD(3-0) is locked against write accesses.
    while(((FlexRayCOM->SUCC[0]) & 0x00000080) != 0);
    FlexRayCOM->GTUC[0] =0x00036B00; // pMicroPerCycle = 224000d = 36B00h (has to be x40 of MacroPerCyle)
                                     // [19:0]: These bits configure the duration of the communication cycle in microticks
    FlexRayCOM->GTUC[1] = 0x000F15E0; // gSyncodeMax = Fh, gMacroPerCycle = 5600d = 15E0h  (cycle period, 5.6us)
                                        //[13:0]: Macrotick per cycle (in macroticks). These bits configure the duration of one communication cycle
                                        //        in macroticks. The cycle length must be identical in all nodes of a cluster.
                                        //[19:16]: Sync node max (in frames). These bits configure the maximum number of frames within a cluster
                                        //         with sync frame indicator bit SYN set. The number of frames must be identical in all nodes of a cluster.

    FlexRayCOM->GTUC[2 ]=0x00061818; // gMacroInitialOffset = 6h, pMicroInitialOffset = 24d = 18h
    FlexRayCOM->GTUC[3]  = 0x0AE40AE3; // gOffsetCorrectionStart - 1 = 2788d = AE4h, gMacroPerCycle - gdNIT - 1 = 2787d = AE3h
    FlexRayCOM->GTUC[4]  = 0x33010303; // pDecodingCorrection = 51d = 33h, pClusterDriftDamping = 1h, pDelayCompensation = 3h
    FlexRayCOM->GTUC[5]  = 0x01510081; // pdMaxDrift = 337d = 151h, pdAcceptedStartupRange = 129d = 81h

    FlexRayCOM->GTUC[6]  = 0x00080056; // gNumberOfStaticSlots = 8h, gdStaticSlot = 86d = 56h
                                          // [25:16]: These bits configure the number of static slots in a cycle.
                                          // [9:0]: These bits configure the duration of a static slot (macroticks).

    FlexRayCOM->GTUC[7]  = 0x015A0004; // gNumberOfMinislots = 346d = 15Ah, gdMinislot = 4h
                                          // [28:16]:These bits configure the number of minislots in the dynamic segment of a cycle
                                          // [5:0]: These bits configure the duration of a minislot

    FlexRayCOM->GTUC[8]  = 0x00010204; // gdDynamicSlotIdlePhase = 1, gdMinislotActionPointOffset = 2, gdActionPointOffset = 4h
    FlexRayCOM->GTUC[9] = 0x015100CD; // pRateCorrectionOut = 337d = 151h, pOffsetCorrectionOut = 205d = CDh
    FlexRayCOM->GTUC[10] = 0x00000000; // pExternRateCorrection = 0, pExternOffsetCorrection = 0, no ext. clk. corr.


    FlexRayCOM->SUCC[1]=0x0F036DA2; // gListenNoise = Fh, pdListenTimeout = 224674d = 36DA2h
                                    //LTN [27:24]: Listen timeout noise. Configures the upper limit for the startup and wakeup listen timeout in the
                                    //presence of noise. Must be identical in all nodes of a cluster.
                                    //The wakeup / startup noise timeout is calculated as follows: LT[20:0] � (LTN[3:0] + 1)
                                    // LT[20:0]: Listen timeout. Configures the upper limit of the startup and wakeup listen timeout.
    FlexRayCOM->SUCC[2] = 0x000000FF; // gMaxWithoutClockCorrectionFatal = Fh , passive = Fh
                                            //WCF[7:4]: Maximum without clock correction fatal. These bits define the number of consecutive even/odd
                                            //cycle pairs with missing clock correction terms that will cause a transition from
                                            //NORMAL_ACTIVE or NORMAL_PASSIVE state.

                                            //WCP[3:0]: Maximum without clock correction passive. These bits define the number of consecutive
                                            //even/odd cycle pairs with missing clock correction terms that will cause a transition from
                                            //NORMAL_ACTIVE to NORMAL_PASSIVE to HALT state.
    FlexRayCOM->PRTC1 = 0x084C000A; // pWakeupPattern = 2h, gdWakeupSymbolRxWindow = 76d, BRP = 0, gdTSSTransmitter = Ah
                                            //BRP[15:14]; Baud rate prescaler. These bits configure the baud rate on the FlexRay bus. The baud rates
                                            //listed below are valid with a sample clock of 80 MHz. One bit time always consists of 8 samples
                                            //independent of the configured baud rate.  =0 ->10Mb/s

    FlexRayCOM->PRTC2 = 0x3CB41212; // gdWakeupSymbolTxLow = 60d, gdWakeupSymbolTxIdle = 180d, gdWakeupSymbolRxLow = 18d, gdWakeupSymbolRxIdle = 18d

    FlexRayCOM->MHDC = 0x010D0009; // pLatestTransmit = 269d = 010Dh, gPayloadLengthStatic = 9h
                                            //Start of latest transmit (in minislots). These bits configure the maximum minislot value allowed
                                            //minislots before inhibiting new frame transmissions in the Dynamic Segment of the cycle.
                                            //[7:0]: Static frame data length.

    FlexRayBUFF->MRC   = 0x00174004; // LCB=23d, FFB=64d, FDB=4d (0..3 static, 4..23 dyn., 0 fifo)
    /*interrupt*/
    FlexRayCOM->SILS = 0; /* 0-- CC_int0  1--CC_int1*/
    FlexRayCOM->SIES = (uint32)( 1<<2 ) |  /* Cycle start interrupt enable.*/
            (uint32)( 1<<3 ) |             /* Transmit interrupt enable*/
            (uint32)( 1<<4 ) |             /* Receive interrupt enable.*/
            (uint32)( 1<<5 ) |             /* Receive FIFO not empty interrupt*/
            (uint32)( 1<<6)  |             /* Receive FIFO full interrupt*/
            (uint32)( 1<<10 ) |            /* Transfer input buffer completed interrupt*/
            (uint32)( 1<<11 ) ;            /* Transfer output buffer completed interrupt*/
    FlexRayCOM->ILE =0x01;    /*Interrupt line CC_int1 disable and CC_int0 are enabled; 0--CC_int0 and CC_int1 disable; 1--CC_int0 enable  CC_int1 disable;
                                                                                         2--CC_int0 disenable  CC_int1 able; 3--CC_int0 and CC_int1 enable*/
    /**/

    /*box0*/
    /*Write Header Section 1 (WRHS1)*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 1;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 0;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);

    /*box1*/
    /*Write Header Section 1 (WRHS1)*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 2;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 1;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box2*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 3;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 2;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box3*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 4;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 3;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box4*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 5;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 4;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box5*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 6;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 5;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box6*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 7;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 6;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box7*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 8;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 7;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box8*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 9;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 8;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box9*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 10;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 9;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box10*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 11;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 10;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box11*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 12;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 11;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box12*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 13;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 12;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box13*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 14;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 13;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box14*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 15;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 14;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box15*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 16;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 15;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box16*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 17;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 16;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box17*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 18;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 17;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box18*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 19;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 18;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box19*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 20;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 19;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box20*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 21;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 20;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box21*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 22;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 21;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box22*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 23;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 22;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box23*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 24;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 23;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box24*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 25;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 24;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box25*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 26;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 25;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box26*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 27;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 26;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box27*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 28;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 27;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box28*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 29;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 28;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box29*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 30;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 29;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box30*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 31;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 30;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box31*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 32;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 31;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box32*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 33;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 32;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box33*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 34;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 33;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box34*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 35;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 34;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box35*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 36;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 35;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box36*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 37;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 36;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box37*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 38;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 37;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box38*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 39;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 38;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box39*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 40;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 39;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box40*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 41;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 40;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box41*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 42;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 41;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box42*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 43;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 42;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box43*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 44;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 43;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box44*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 43;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 44;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box45*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 44;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 45;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box46*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 47;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 46;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box47*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 48;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 47;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box48*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 49;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 48;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box49*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 50;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 49;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box50*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 51;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 50;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box51*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 52;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 51;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box52*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 53;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 52;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box53*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 54;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 53;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box54*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 55;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 54;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box55*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 56;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 55;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box56*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 57;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 56;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box57*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 58;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 57;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box58*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 59;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 58;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box59*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 60;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 59;  /* input buffer number */

    FlexRay_Set( INPUT,&Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box60*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 61;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 60;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box61*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 62;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 61;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box62*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 1;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 63;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 62;  /* input buffer number */

    FlexRay_Set(INPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(INPUT,&Fr_LSduPtr);
    /*box63*/
    Fr_LPduPtr.mbi  = 1;   /* message buffer interrupt  0--dis  1--enable*/
    Fr_LPduPtr.txm  = 0;   /* transmission mode - 0--continuous mode  1--single-short*/
    Fr_LPduPtr.ppit = 0;   /* network management Enable  0--dis  1--enable*/
    Fr_LPduPtr.cfg  = 0;   /* message buffer configuration bit (0=RX, 1 = TX)*/
    Fr_LPduPtr.chb  = 0;   /* Ch B*/
    Fr_LPduPtr.cha  = 0;   /* Ch A*/
    if(flexrayx.ch==FLEXRAYA)
    Fr_LPduPtr.cha  = 1;   /* ChA*/
    else if(flexrayx.ch==FLEXRAYB)
    Fr_LPduPtr.chb  = 1;   /* ChB*/
    Fr_LPduPtr.cyc  = 0;   /* Cycle Filtering Code (no cycle filtering)*/
    Fr_LPduPtr.fid  = 64;   /* Frame ID*/
    /*Write Header Section 2 (WRHS2)*/
    Fr_LPduPtr.pl   = 8;   /* Payload Length*/
    Fr_LPduPtr.crc  = header_crc(&Fr_LPduPtr);
    /*Write Header Section 3 (WRHS3)*/
    Fr_LPduPtr.dp   = 0x80;   /* Pointer to start of data in message RAM*/

    Fr_LPduPtr.sfi  = 0;   /* startup frame indicator*/
    Fr_LPduPtr.sync = 0;   /* sync frame indicator*/

    /*Write IBCM*/
    Fr_LSduPtr.stxrh= 0;  /* set transmission request*/
    Fr_LSduPtr.lhsh = 1;  /* load header section*/
    Fr_LSduPtr.ldsh = 0;  /* load data section*/
    /*Write IBCR*/
    Fr_LSduPtr.ibrh = 63;  /* input buffer number */

    FlexRay_Set(OUTPUT, &Fr_LPduPtr);
    Fr_TransmitTxRx(OUTPUT,&Fr_LSduPtr);



    Fr_ControllerInit();

    FlexRayCOM->EIR=0xffffffff;// Clear Error Int.
    FlexRayCOM->SIR=0xFFFFFFFF; // Clear Status Int.
    FlexRayCOM->SILS=0x00000000; // all Status Int. to eray_int0
    FlexRayCOM->SIER== 0xFFFFFFFF; // Disable all Status Int.
    FlexRayCOM->SIES= 0x00000004; // Enable CYCSE Int.
    FlexRayCOM->ILE = 0x00000002; // enable eray_int1
    Fr_AllowColdStart();

}

int header_crc(wrhs *Fr_LPduPtr)
{
  unsigned int header;

  int CrcInit = 0x1A;
  int length  = 20;
  int CrcNext;
  unsigned long CrcPoly  = 0x385;
  unsigned long CrcReg_X = CrcInit;
  unsigned long header_temp, reg_temp;

  header  = ((Fr_LPduPtr->sync & 0x1)  << 19) | ((Fr_LPduPtr->sfi & 0x1) << 18);
  header |= ((Fr_LPduPtr->fid & 0x7FF) <<  7) |  (Fr_LPduPtr->pl & 0x7F);

  header   <<= 11;
  CrcReg_X <<= 21;
  CrcPoly  <<= 21;

  while(length--) {
    header    <<= 1;
    header_temp = header & 0x80000000;
    reg_temp    = CrcReg_X & 0x80000000;

    if(header_temp ^ reg_temp){  // Step 1
      CrcNext = 1;
    } else {
      CrcNext = 0;
    }

    CrcReg_X <<= 1;              // Step 2

    if(CrcNext) {
      CrcReg_X ^= CrcPoly;       // Step 3
    }
  }

  CrcReg_X >>= 21;

  return CrcReg_X;
}


void FlexRay_Set(uint8 direction,wrhs *Fr_LPduPtr)
{
    if(direction==INPUT)
    {
        FlexRayBUFF->WRHS[0] |= (uint32)((uint32)(Fr_LPduPtr->mbi & 0x1) << 29U) |        /*message buffer interrupt  1--disable 0--enable*/
                            (uint32)((uint32)(Fr_LPduPtr->txm & 0x1) << 28U) |         /*Transmission mode  1--Single  0--Continuous*/
                            (uint32)((uint32)(Fr_LPduPtr->ppit & 0x1) << 27U) |        /*Payload Preamble Indicator  1--set  0--no set*/
                            (uint32)((uint32)(Fr_LPduPtr->cfg & 0x1) << 26U) |        /*Message buffer configuration bit  1--send  0--revice*/
                            (uint32)((uint32)(Fr_LPduPtr->chb & 0x1) << 25U) |        /*Channel filter contro  */
                            (uint32)((uint32)(Fr_LPduPtr->cha & 0x1)<< 24U) |        /*Channel filter contro  */
                            (uint32)((uint32)(Fr_LPduPtr->cyc & 0x7F) << 16U) |        /*Cycle code  */
                            (uint32)((uint32) (Fr_LPduPtr->fid & 0x7FF) << 0U) ;        /*Frame ID.  */

       FlexRayBUFF->WRHS[1] |=(uint32)((uint32)(Fr_LPduPtr->pl & 0x7F) << 16U) |        /*Payload length  */
                               (uint32)((uint32)(Fr_LPduPtr->crc & 0x7FF) << 0U) ;        /*Header CRC  */

       FlexRayBUFF->WRHS[2] |=(uint32)((uint32)(Fr_LPduPtr->dp & 0x7FF) << 0U) ;        /*Data pointer  */
    }
    else if(direction==OUTPUT)
    {
        FlexRayBUFF->RDHS[0] |= (uint32)((uint32)(Fr_LPduPtr->mbi & 0x1) << 29U) |        /*message buffer interrupt  1--disable 0--enable*/
                            (uint32)((uint32)(Fr_LPduPtr->txm & 0x1) << 28U) |         /*Transmission mode  1--Single  0--Continuous*/
                            (uint32)((uint32)(Fr_LPduPtr->ppit & 0x1) << 27U) |        /*Payload Preamble Indicator  1--set  0--no set*/
                            (uint32)((uint32)(Fr_LPduPtr->cfg & 0x1) << 26U) |        /*Message buffer configuration bit  1--send  0--revice*/
                            (uint32)((uint32)(Fr_LPduPtr->chb & 0x1) << 25U) |        /*Channel filter contro  */
                            (uint32)((uint32)(Fr_LPduPtr->cha & 0x1)<< 24U) |        /*Channel filter contro  */
                            (uint32)((uint32)(Fr_LPduPtr->cyc & 0x7F) << 16U) |        /*Cycle code  */
                            (uint32)((uint32) (Fr_LPduPtr->fid & 0x7FF) << 0U) ;        /*Frame ID.  */

       FlexRayBUFF->RDHS[1] |=(uint32)((uint32)(Fr_LPduPtr->pl & 0x7F) << 16U) |        /*Payload length  */
                               (uint32)((uint32)(Fr_LPduPtr->crc & 0x7FF) << 0U) ;        /*Header CRC  */

       FlexRayBUFF->RDHS[2] |=(uint32)((uint32)(Fr_LPduPtr->dp & 0x7FF) << 0U) ;        /*Data pointer  */
    }
}

void Fr_TransmitTxRx(uint8 direction, bc *Fr_LSduPtr)
{
    if(direction==INPUT)
    {
        // ensure nothing is pending
        while ((FlexRayBUFF->IBCM & 0x0008000) != 0);
        FlexRayBUFF->IBCM=((Fr_LSduPtr->stxrh & 0x1) << 2) | ((Fr_LSduPtr->ldsh & 0x1) << 1) | (Fr_LSduPtr->lhsh & 0x1);
        FlexRayBUFF->IBCM=(Fr_LSduPtr->ibrh & 0x3F);
        // optimization possible for future by not gating like below
        // wait for completion on host registers
        while ((Fr_LSduPtr->ibsyh != 0) && ((FlexRayBUFF->IBCM & 0x00008000) != 0));
        // wait for completion on shadow registers
        while ((Fr_LSduPtr->ibsys != 0) && ((FlexRayBUFF->IBCM & 0x80000000) != 0));
    }
    else if(direction==OUTPUT)
   {
       // ensure nothing is pending
       while ((FlexRayBUFF->OBCM & 0x0008000) != 0);
       FlexRayBUFF->OBCM=((Fr_LSduPtr->stxrh & 0x1) << 2) | ((Fr_LSduPtr->ldsh & 0x1) << 1) | (Fr_LSduPtr->lhsh & 0x1);
       FlexRayBUFF->OBCM=(Fr_LSduPtr->ibrh & 0x3F);
       // optimization possible for future by not gating like below
       // wait for completion on host registers
       while ((Fr_LSduPtr->ibsyh != 0) && ((FlexRayBUFF->OBCM & 0x00008000) != 0));
       // wait for completion on shadow registers
       while ((Fr_LSduPtr->ibsys != 0) && ((FlexRayBUFF->OBCM & 0x80000000) != 0));
   }
}


void FLEXRAY_Tx(FLEXRAY_Info flexrayx, uint32 ID, uint8* buff, uint32 len)
{
    uint8 i=0;
    uint8 BoxNum=0;
  volatile  uint32 * buffadd =FlexRayBUFF->WRDS;

    for(i=0;i<len;i++)
    {
        * buffadd=buff[i];
         buffadd++;
    }
    BoxNum=Get_FlexrayMcanMESSAGE_BOX_Num(flexrayx,ID);
    FlexRay_Send( BoxNum);
}
void  FlexRay_Send( uint8 boxnum)
{

    // ensure nothing is pending
        while ((FlexRayBUFF->IBCR & 0x0008000) != 0);
        FlexRayBUFF->IBCM=(0x1<< 2) | (0x1 << 1) | 0x1;
        FlexRayBUFF->IBCR=(boxnum & 0x3F);
        // optimization possible for future by not gating like below
        // wait for completion on host registers
      //  while ((Fr_LSduPtr->ibsyh != 0) && ((FlexRayBUFF->IBCR & 0x00008000) != 0));
        // wait for completion on shadow registers
        while (((FlexRayBUFF->IBCR & 0x80000000) != 0));

}
void FLERAY_Rx(void)
{
    while (((FlexRayBUFF->OBCR) & 0x00008000) != 0);
    FlexRayBUFF->OBCM=(( 0x1 << 1) | 0x1);
    FlexRayBUFF->OBCR=((1 << 9) | (63 & 0x3F)); //req=1, view=0
        // wait for completion on shadow registers
   while (((FlexRayBUFF->OBCR) & 0x00008000) != 0);
   FlexRayBUFF->OBCM=(( 0x1 << 1) | 0x1);
   FlexRayBUFF->OBCR=((1 << 8) | (63 & 0x3F)); //req=0, view=1

}

int Fr_ControllerInit(void)
{
    unsigned int error=0;
    uint32 reg=0;
    // write SUCC1 configuration
    FlexRayCOM->SUCC[0] = 0x0F1FFB00 | CMD_CONFIG;
    // Check if POC has accepted last command
    if ((FlexRayCOM->SUCC[0] & 0xF) == 0x0) return 1;
    // Wait for PBSY bit to clear - POC not busy
    while ((FlexRayCOM->SUCC[0] & 0x00000080) != 0x0);

    // unlock CONFIG and enter READY state
    reg= FlexRayCOM->LCK;
    reg=(reg & 0xffffff00) |0xce;
    FlexRayCOM->LCK =0xce;
    reg=(reg & 0xffffff00) |0x31;
    FlexRayCOM->LCK =reg;
    // write SUCC1 configuration
    reg=(FlexRayCOM->SUCC[0] & 0xffff0000) |(0xFB00 | CMD_READY);
    FlexRayCOM->SUCC[0]=reg;
    // Check if POC has accepted last command
    if ((FlexRayCOM->SUCC[0] & 0xF) == 0x0) error = 1;
    // Wait for PBSY bit to clear - POC not busy
    while ((FlexRayCOM->SUCC[0] & 0x00000080) != 0x0);
    return error;
}

int Fr_AllowColdStart(void)
{
    unsigned int error=0;
    uint32 reg=0;
    // write SUCC1 configuration
    reg=(FlexRayCOM->SUCC[0] & 0xffffff00) | CMD_ALLOW_COLDSTART;
    FlexRayCOM->SUCC[0] = reg;
    // Check if POC has accepted last command
    if ((FlexRayCOM->SUCC[0] & 0xF) == 0x0) error = 1;
    // Wait for PBSY bit to clear - POC not busy
    while ((FlexRayCOM->SUCC[0] & 0x00000080) != 0x0);
    return error;
}

int Fr_StartCommunication(void)
{
    unsigned int error=0;
    uint32 reg=0;
    // write SUCC1 configuration
    reg=(FlexRayCOM->SUCC[0] & 0xffffff00) | CMD_RUN;
    (FlexRayCOM->SUCC[0] = reg);
    // Check if POC has accepted last command
    if ((FlexRayCOM->SUCC[0] & 0xF) == 0x0) error = 1;
    return error;
}
uint8 Get_FlexrayMcanMESSAGE_BOX_Num(FLEXRAY_Info flesxrayx,uint32 ID)
{
    uint8 i=0;
    uint8 box=1;
    switch(flesxrayx.ch)
    {
        case FLEXRAYA:
            for(i=0;i<FLEXRAY_MCANMESSAGE_BOX_NUM;i++)
            {
                if(FLEXRAYA_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    box=FLEXRAYA_McanMESSAGE_BOX_TABLE[i].BOX_num;
                    break;
                }
            }
        break;
        case FLEXRAYB:
            for(i=0;i<FLEXRAY_MCANMESSAGE_BOX_NUM;i++)
            {
                if(FLEXRAYB_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    box=FLEXRAYB_McanMESSAGE_BOX_TABLE[i].BOX_num;
                    break;
                }
            }
        break;

    }
    return box;

}
void FLEXRAY_Tx_bind(FLEXRAY_Info flesxrayx, uint32 ID,uint8 messagebox)
{
    uint8 i=0;
    FlexRayBUFF->WRHS[0] &=0xfffff800;
    FlexRayBUFF->WRHS[0] |=(ID & 0x7ff);
    switch(flesxrayx.ch)
    {
        case FLEXRAYA:
            for(i=0;i<FLEXRAY_MCANMESSAGE_BOX_NUM;i++)
            {
                if(FLEXRAYA_McanMESSAGE_BOX_TABLE[i].ID!=ID && FLEXRAYA_McanMESSAGE_BOX_TABLE[i].ID==0)
                {
                    FLEXRAYA_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    FLEXRAYA_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    return;
                }
                else if(FLEXRAYA_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    FLEXRAYA_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    FLEXRAYA_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    return;
                }
            }
            break;
        case FLEXRAYB:
            for(i=0;i<FLEXRAY_MCANMESSAGE_BOX_NUM;i++)
            {
                if(FLEXRAYB_McanMESSAGE_BOX_TABLE[i].ID!=ID && FLEXRAYB_McanMESSAGE_BOX_TABLE[i].ID==0)
                {
                    FLEXRAYB_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    FLEXRAYB_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    return;
                }
                else if(FLEXRAYB_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    FLEXRAYB_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    FLEXRAYB_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    return;
                }
            }
            break;
    }
}
void FLEXRAY_RecvQueueIn(uint8 FLEXRAY_DIR,uint32 ID, uint32 * data,uint8 len)
{
    uint8 i=0;
    if(FLEXRAY_DIR==FLEXRAYA)
    {
         if ((FLEXRAYA_Msg_Buff.tail + 1) % FLEXRAY_QUEUEMAX == FLEXRAYA_Msg_Buff.head)
          {
             FLEXRAYA_Msg_Buff.frame[FLEXRAYA_Msg_Buff.tail].ID=ID;
             FLEXRAYA_Msg_Buff.frame[FLEXRAYA_Msg_Buff.tail].len=len;
             for(i=0;i<len;i++)
             {
                 FLEXRAYA_Msg_Buff.frame[FLEXRAYA_Msg_Buff.tail].revice_data[i]=data[i];
             }
            FLEXRAYA_Msg_Buff.head = (FLEXRAYA_Msg_Buff.head+1) % FLEXRAY_QUEUEMAX;
            FLEXRAYA_Msg_Buff.tail = (FLEXRAYA_Msg_Buff.tail + 1) % FLEXRAY_QUEUEMAX;
          }
          else
          {
              FLEXRAYA_Msg_Buff.frame[FLEXRAYA_Msg_Buff.tail].ID=ID;
               FLEXRAYA_Msg_Buff.frame[FLEXRAYA_Msg_Buff.tail].len=len;
               for(i=0;i<len;i++)
               {
                   FLEXRAYA_Msg_Buff.frame[FLEXRAYA_Msg_Buff.tail].revice_data[i]=data[i];
               }
            FLEXRAYA_Msg_Buff.tail = (FLEXRAYA_Msg_Buff.tail + 1) % FLEXRAY_QUEUEMAX;
          }
    }
    if(FLEXRAY_DIR==FLEXRAYB)
    {
         if ((FLEXRAYB_Msg_Buff.tail + 1) % FLEXRAY_QUEUEMAX == FLEXRAYB_Msg_Buff.head)
          {
             FLEXRAYB_Msg_Buff.frame[FLEXRAYB_Msg_Buff.tail].ID=ID;
             FLEXRAYB_Msg_Buff.frame[FLEXRAYB_Msg_Buff.tail].len=len;
             for(i=0;i<len;i++)
             {
                 FLEXRAYB_Msg_Buff.frame[FLEXRAYB_Msg_Buff.tail].revice_data[i]=data[i];
             }
            FLEXRAYB_Msg_Buff.head = (FLEXRAYB_Msg_Buff.head+1) % FLEXRAY_QUEUEMAX;
            FLEXRAYB_Msg_Buff.tail = (FLEXRAYB_Msg_Buff.tail + 1) % FLEXRAY_QUEUEMAX;
          }
          else
          {
              FLEXRAYB_Msg_Buff.frame[FLEXRAYB_Msg_Buff.tail].ID=ID;
              FLEXRAYB_Msg_Buff.frame[FLEXRAYB_Msg_Buff.tail].len=len;
              for(i=0;i<len;i++)
              {
                  FLEXRAYB_Msg_Buff.frame[FLEXRAYB_Msg_Buff.tail].revice_data[i]=data[i];
              }
            FLEXRAYB_Msg_Buff.tail = (FLEXRAYB_Msg_Buff.tail + 1) % FLEXRAY_QUEUEMAX;
          }
    }

}
#pragma CODE_STATE(FlexRayT0CInterrupt, 32)
#pragma INTERRUPT(FlexRayT0CInterrupt, IRQ)

void FlexRayT0CInterrupt(void)
{
    uint8 len=0;
    uint8 i=0;
    uint32 reID=0;
    uint8 FLEXRAY_DIR;
    uint32 rebuff[64]={0};
    if((FlexRayCOM->SIR & (0x01 << 11))==0x01) /*Transfer output buffer completed.*/
    {
        len=(FlexRayBUFF->RDHS[1] >>24 ) &0x7f;
        reID=FlexRayBUFF->RDHS[1] & 0x7ff;
        if(((FlexRayCOM->SIR >> 17) & 0x01)==1)
            FLEXRAY_DIR=FLEXRAYA;
        else if(((FlexRayCOM->SIR >> 25) & 0x01)==1)
            FLEXRAY_DIR=FLEXRAYB;
        for(i=0;i<len;i++)
        {
            rebuff[i]=FlexRayBUFF->RDDS[i];
        }
    }
    if((FlexRayCOM->SIR & (0x01 << 4))==0x01) /*Receive interrupt*/
    {
        len=(FlexRayBUFF->RDHS[1] >>24 ) &0x7f;
        reID=FlexRayBUFF->RDHS[1] & 0x7ff;
       for(i=0;i<len;i++)
       {
           rebuff[i]=FlexRayBUFF->RDDS[i];
       }
    }
    FLEXRAY_RecvQueueIn(FLEXRAY_DIR,reID,rebuff,len);


}

#pragma CODE_STATE(FlexRayT1CInterrupt, 32)
#pragma INTERRUPT(FlexRayT1CInterrupt, IRQ)

void FlexRayT1CInterrupt(void)
{




}
