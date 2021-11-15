/*
 * FlexRay.h
 *
 *  Created on: 2021年8月6日
 *      Author: 15755
 */

#ifndef FLEXRAY_H_
#define FLEXRAY_H_
#include "HL_sys_common.h"

typedef volatile struct FlexRayBase
{
    uint32 GSN0;              /**< 0x0000:     Global Static Number 0                         */
    uint32 GSN1;              /**< 0x0004:     Global Static Number 1                         */
    uint32 RESERVE1;          /**< 0x0008:     useless                        */
    uint32 RESERVE2;          /**< 0x000c:     useless                        */
    uint32 GCS;               /**< 0x0010:     Global Control Set                        */
    uint32 GCR;               /**< 0x0014:     Global Control Reset                        */
    uint32 TSCB;              /**< 0x0018:     Transfer Status Current Buffer                        */
    uint32 LTBCC;             /**< 0x001C:     Last Transferred Buffer to Communication Controller                        */
    uint32 LTBSM;             /**< 0x0020:     Last Transferred Buffer to System Memory                        */
    uint32 TBA;               /**< 0x0024:     Transfer Base Address                        */
    uint32 NTBA;              /**< 0x0028:     Next Transfer Base Address                        */
    uint32 BAMS;              /**< 0x002C:     Base Address of Mirrored Status                        */
    uint32 SAMP;              /**< 0x0030:     Start Address of Memory Protection                        */
    uint32 EAMP;              /**< 0x0034:     End Address of Memory Protection                        */
    uint32 RESERVE3;          /**< 0x0038:     useless                        */
    uint32 RESERVE4;          /**< 0x003C:     useless                        */
    uint32 TSMO[4];           /**< 0x0040:     Transfer to System Memory Occurred                       */
    uint32 TCCO[4];           /**< 0x0050:     Transfer to Communication Controller Occurred                        */
    uint32 TOOFF;             /**< 0x0060:     Transfer Occurred Offset                        */
    uint32 RESERVE5;          /**< 0x0064:     useless                        */
    uint32 RESERVE6;          /**< 0x0068:     useless                        */
    uint32 TSBESTAT;          /**< 0x006C:     TCR ECC Single-Bit Error Status                        */
    uint32 PEADR;             /**< 0x0070:     ECC Error Address                        */
    uint32 TEIF;              /**< 0x0074:     Transfer Error Interrupt                        */
    uint32 TEIRES;            /**< 0x0078:     Transfer Error Interrupt Enable Set                        */
    uint32 TEIRER;            /**< 0x007C:     Transfer Error Interrupt Enable Reset                        */
    uint32 TTSMS1;            /**< 0x0080:     Trigger Transfer to System Memory Set 1                        */
    uint32 TTSMR1;            /**< 0x0084:     Trigger Transfer to System Memory Reset 1                        */
    uint32 TTSMS2;            /**< 0x0088:     Trigger Transfer to System Memory Set 2                        */
    uint32 TTSMR2;            /**< 0x008C:     Trigger Transfer to System Memory Reset 2                        */
    uint32 TTSMS3;            /**< 0x0090:     Trigger Transfer to System Memory Set 3                        */
    uint32 TTSMR3;            /**< 0x0094:     Trigger Transfer to System Memory Reset 3                        */
    uint32 TTSMS4;            /**< 0x0098:     Trigger Transfer to System Memory Set 4                        */
    uint32 TTSMR4;            /**< 0x009C:     Trigger Transfer to System Memory Reset 4                        */
    uint32 TTCCS1;            /**< 0x00A0:     Trigger Transfer to Communication Controller Set 1                       */
    uint32 TTCCR1;            /**< 0x00A4:     Trigger Transfer to Communication Controller Reset 1                       */
    uint32 TTCCS2;            /**< 0x00A8:     Trigger Transfer to Communication Controller Set 2                       */
    uint32 TTCCR2;            /**< 0x00AC:     Trigger Transfer to Communication Controller Reset 2                       */
    uint32 TTCCS3;            /**< 0x00B0:     Trigger Transfer to Communication Controller Set 2                       */
    uint32 TTCCR3;            /**< 0x00B4:     Trigger Transfer to Communication Controller Reset 3                       */
    uint32 TTCCS4;            /**< 0x00B8:     Trigger Transfer to Communication Controller Set 4                       */
    uint32 TTCCR4;            /**< 0x00BC:     Trigger Transfer to Communication Controller Reset 4                       */
    uint32 ETESMS1;           /**< 0x00C0:     Enable Transfer on Event to System Memory Set 1                       */
    uint32 ETESMR1;           /**< 0x00C4:     Enable Transfer on Event to System Memory Reset 1                      */
    uint32 ETESMS2;           /**< 0x00C8:     Enable Transfer on Event to System Memory Set 2                       */
    uint32 ETESMR2;           /**< 0x00CC:     Enable Transfer on Event to System Memory Reset 2                      */
    uint32 ETESMS3;           /**< 0x00D0:     Enable Transfer on Event to System Memory Set 3                       */
    uint32 ETESMR3;           /**< 0x00D4:     Enable Transfer on Event to System Memory Reset 3                      */
    uint32 ETESMS4;           /**< 0x00D8:     Enable Transfer on Event to System Memory Set 4                       */
    uint32 ETESMR4;           /**< 0x00DC:     Enable Transfer on Event to System Memory Reset 4                      */
    uint32 CESMS1;            /**< 0x00E0:     Clear on Event to System Memory Set 1                       */
    uint32 CESMR1;            /**< 0x00E4:     Clear on Event to System Memory Reset 1                      */
    uint32 CESMS2;            /**< 0x00E8:     Clear on Event to System Memory Set 2                       */
    uint32 CESMR2;            /**< 0x00EC:     Clear on Event to System Memory Reset 2                      */
    uint32 CESMS3;            /**< 0x00F0:     Clear on Event to System Memory Set 3                       */
    uint32 CESMR3;            /**< 0x00F4:     Clear on Event to System Memory Reset 3                      */
    uint32 CESMS4;            /**< 0x00F8:     Clear on Event to System Memory Set 4                       */
    uint32 CESMR4;            /**< 0x00FC:     Clear on Event to System Memory Reset 4                      */
    uint32 TSMIES1;           /**< 0x0100:    Transfer to System Memory Interrupt Enable Set 1                      */
    uint32 TSMIER1;           /**< 0x0104:    Transfer to System Memory Interrupt Enable Reset 1                     */
    uint32 TSMIES2;           /**< 0x0108:    Transfer to System Memory Interrupt Enable Set 2                      */
    uint32 TSMIER2;           /**< 0x010C:    Transfer to System Memory Interrupt Enable Reset 2                     */
    uint32 TSMIES3;           /**< 0x0110:    Transfer to System Memory Interrupt Enable Set 3                      */
    uint32 TSMIER3;           /**< 0x0114:    Transfer to System Memory Interrupt Enable Reset 3                     */
    uint32 TSMIES4;           /**< 0x0118:    Transfer to System Memory Interrupt Enable Set 4                      */
    uint32 TSMIER4;           /**< 0x011C:    Transfer to System Memory Interrupt Enable Reset 4                     */
    uint32 TCCIES1;           /**< 0x0120:    Transfer to Communication Controller Interrupt Enable Set 1                      */
    uint32 TCCIER1;           /**< 0x0124:    Transfer to Communication Controller Interrupt Enable Reset 1                    */
    uint32 TCCIES2;           /**< 0x0120:    Transfer to Communication Controller Interrupt Enable Set 2                      */
    uint32 TCCIER2;           /**< 0x0124:    Transfer to Communication Controller Interrupt Enable Reset 2                    */
    uint32 TCCIES3;           /**< 0x0120:    Transfer to Communication Controller Interrupt Enable Set 3                      */
    uint32 TCCIER3;           /**< 0x0124:    Transfer to Communication Controller Interrupt Enable Reset 3                    */
    uint32 TCCIES4;           /**< 0x0120:    Transfer to Communication Controller Interrupt Enable Set 4                      */
    uint32 TCCIER4;           /**< 0x0124:    Transfer to Communication Controller Interrupt Enable Reset 4                    */

}FlexRayBASE_t;


typedef volatile struct FlexRayComBase
{
    uint32 ECC_CTRL;              /**< 0x0000:     ECC Control Register                         */
    uint32 ECCDSTAT;              /**< 0x0004:     ECC Diagnostic Status Register                         */
    uint32 ECCTEST;               /**< 0x0008:     ECC Test Register                         */
    uint32 SBESTAT;               /**< 0x000C:     Single-Bit Error Status Register                         */
    uint32 TEST1;                 /**< 0x0010:     Test Register 1                         */
    uint32 TEST2;                 /**< 0x0014:     Test Register 2                         */
    uint32 RESERVE5;              /**< 0x0018:     useless                        */
    uint32 LCK;                   /**< 0x001C:     Lock Register 1                         */
    uint32 EIR;                   /**< 0x0020:     Error Interrupt Register                        */
    uint32 SIR;                   /**< 0x0024:     Status Interrupt Register                        */
    uint32 EILS;                  /**< 0x0028:     Error Interrupt Line Select Register                        */
    uint32 SILS;                  /**< 0x002C:     Status Interrupt Line Select Register                       */
    uint32 EIES;                  /**< 0x0030:     Error Interrupt Enable Set Register                       */
    uint32 EIER;                  /**< 0x0034:     Error Interrupt Enable Reset Register                      */
    uint32 SIES;                  /**< 0x0038:     Status Interrupt Enable Set Register                     */
    uint32 SIER;                  /**< 0x003C:     Status Interrupt Enable Reset Register                     */
    uint32 ILE;                   /**< 0x0040:     Interrupt Line Enable Register                     */
    uint32 T0C;                   /**< 0x0044:     Timer 0 Configuration Register                     */
    uint32 T1C;                   /**< 0x0048:     Timer 1 Configuration Register                     */
    uint32 STPW1;                 /**< 0x004C:     Stop Watch Register 1                    */
    uint32 STPW2;                 /**< 0x0050:     Stop Watch Register 2                    */
    uint32 RESERVE6[11];          /**< 0x0054:     useless                        */
    uint32 SUCC[3];               /**< 0x0080:     SUC Configuration Register                       */
    uint32 NEMC;                  /**< 0x008C:     NEM Configuration Register                      */
    uint32 PRTC1;                 /**< 0x0090:     PRT Configuration Register 1                      */
    uint32 PRTC2;                 /**< 0x0094:     PRT Configuration Register 2                      */
    uint32 MHDC;                  /**< 0x0098:     MHD Configuration Register 1                     */
    uint32 RESERVE7;              /**< 0x009C:     useless                        */
    uint32 GTUC[11];              /**< 0x00AO:     GTU Configuration Register                     */
    uint32 RESERVE8[13];          /**< 0x0054:     useless                        */
    uint32 CCSV;                  /**< 0x0100:     Communication Controller Status Vector Register                    */
    uint32 CCEV;                  /**< 0x0104:     Communication Controller Error Vector Register                    */
    uint32 RESERVE9[2];           /**< 0x0108:     useless                        */
    uint32 SCV;                   /**< 0x0110:     Slot Counter Value Register                    */
    uint32 MTCCV;                 /**< 0x0114:     Macrotick and Cycle Counter Value Register                    */
    uint32 RCV;                   /**< 0x0118:     Rate Correction Value Register                  */
    uint32 OCV;                   /**< 0x011C:     Offset Correction Value Register                  */
    uint32 SFS;                   /**< 0x0120:     Sync Frame Status Register                  */
    uint32 SWNIT;                 /**< 0x0124:     Symbol Window and NIT Status Register                  */
    uint32 ACS;                   /**< 0x0128:     Aggregated Channel Status Register                  */
    uint32 RESERVE10;              /**< 0x012C:     useless                        */
    uint32 ESID[15];              /**< 0x0130:     Even Sync ID Register                 */
    uint32 RESERVE11;              /**< 0x016C:     useless                        */
    uint32 OSID[15];              /**< 0x0170:     Odd Sync ID Register                 */
    uint32 RESERVE12;             /**< 0x01AC:     useless                        */
    uint32 NMV[3];                /**< 0x01B0:     Network Management Vector Register                */

}FlexRayComBASE_t;

typedef volatile struct FlexRayBuffBase
{
    uint32 MRC;              /**< 0x0300:    Message RAM Configuration Register                         */
    uint32 FRF;              /**< 0x0304:    FIFO Rejection Filter Register                         */
    uint32 FRFM;             /**< 0x0308:    FIFO Rejection Filter Mask Register                         */
    uint32 FCL;              /**< 0x030c:    FIFO Critical Level Register                         */
    uint32 MHDS;             /**< 0x0310:    Message Handler Status                         */
    uint32 LDTS;             /**< 0x0314:    Last Dynamic Transmit Slot                         */
    uint32 FSR;              /**< 0x0318:    FIFO Status Register                         */
    uint32 MHDF;             /**< 0x031C:    Message Handler Constraints Flags                         */
    uint32 TXRQ[4];          /**< 0x0320:    Transmission Request Register                          */
    uint32 NDAT[4];          /**< 0x0330:    New Data Register                          */
    uint32 MBSC[4];          /**< 0x0340:    Message Buffer Status Changed Register                          */
    uint32 RESERVE11[40];    /**< 0x0350:    useless                        */
    uint32 CREL;             /**< 0x03F0:    Core Release Register                        */
    uint32 ENDN;             /**< 0x03F4:    Endian Registerr                        */
    uint32 RESERVE12[2];     /**< 0x03F8:    useless                        */
    uint32 WRDS[64];         /**< 0x0400:    Write Data Section Registerr                          */
    uint32 WRHS[3];          /**< 0x0500:    Write Header Section Register                          */
    uint32 RESERVE13;        /**< 0x050C:    useless                        */
    uint32 IBCM;             /**< 0x0510:    Input Buffer Command Mask Register                       */
    uint32 IBCR;             /**< 0x0514:    Input Buffer Command Request Register                       */
    uint32 RESERVE14[58];    /**< 0x0518:    useless                        */
    uint32 RDDS[64];         /**< 0x0600:    Read Data Section Register                        */
    uint32 RDHS[3];          /**< 0x0700:    Read Header Section Register                        */
    uint32 MBS;              /**< 0x070C:    Message Buffer Status Register                       */
    uint32 OBCM;             /**< 0x0710:    Output Buffer Command Mask Register                       */
    uint32 OBCR;             /**< 0x0714:    Output Buffer Command Request Register                      */


}FlexRayBuffBASE_t;

typedef volatile struct FlexRay_RAM_Base
{
    uint32 TCR[128];         /**< 0x0000:    Transfer Configuration RAM                    */
    uint32 TCR_ECC[127];     /**< 0x0200:   TCR ECC Test Mode                   */

}FlexRay_RAM_BASE_t;

#define FlexRayREG ((FlexRayBASE_t *)0xFFF7A000U)
#define FlexRayRAM ((FlexRay_RAM_BASE_t *)0x00000000U)
#define FlexRayCOM ((FlexRayComBASE_t *)0xFFF7C800U)
#define FlexRayBUFF ((FlexRayBuffBASE_t *)0xFFF7CB00U)

#define CMD_command_not_accepted             0x0
#define CMD_CONFIG                           0x1
#define CMD_READY                            0x2
#define CMD_WAKEUP                           0x3
#define CMD_RUN                              0x4
#define CMD_ALL_SLOTS                        0x5
#define CMD_HALT                             0x6
#define CMD_FREEZE                           0x7
#define CMD_SEND_MTS                         0x8
#define CMD_ALLOW_COLDSTART                  0x9
#define CMD_RESET_STATUS_INDICATORS          0xA
#define CMD_MONITOR_MODE                     0xB
#define CMD_CLEAR_RAMS                       0xC
#define CMD_ASYNCHRONOUS_TRANSFER_MODE       0xE
typedef struct _FLEXRAY_INFO  /*FLEXRAY 参数信息*/
{
    uint8 ch;
    uint32 burt;
}FLEXRAY_Info;

typedef struct _FLEXRAY_MCANMESSAGE_BOX /*FLEXRAY 发送数据的ID和邮箱关系表信息*/
{
    uint32 ID; /**/
    uint8_t BOX_num;  /**/
} FLEXRAY_McanMESSAGE_BOX;
#define FLEXRAY_MCANMESSAGE_BOX_NUM  64
typedef volatile struct _wrhs
{
    int mbi;
    int txm;
    int ppit;
    int cfg;
    int chb;
    int cha;
    int cyc;
    int fid;
    int pl;
    int crc;
    int dp;
    int sync;
    int sfi;
} wrhs;

typedef volatile struct _bc
{
    int ibrh;
    int stxrh;
    int ldsh;
    int lhsh;
    int ibsyh;
    int ibsys;
    int obrs;
    int rdss;
    int rhss;
} bc;
enum FLEXRAY_NUM  /*FLEXRAY 编号*/
{
    FLEXRAYA=1,
    FLEXRAYB=2,
};
enum FLEXRAY_DIR  /*FLEXRAY 数据传输方向*/
{
    INPUT=1,
    OUTPUT=2,
};
#define FLEXRAY_QUEUEMAX  10  /*FLEXRAY 保存数据队列深度*/
typedef  struct _FLEXRAY_REVICE_BUFF  /*FLEXRAY 保存每帧数据信息*/
{
    uint8 len;
    uint32 ID;
    uint32 revice_data[64];
} FLEXRAY_REVICE_BUFF;
typedef  struct _FLEXRAY_MSG_BUFF /*FLEXRAY 保存数据队列信息*/
{
    uint8 head;
    uint8 tail;
    FLEXRAY_REVICE_BUFF frame[FLEXRAY_QUEUEMAX];
} FLEXRAY_MSG_BUFF;










    void FLEXRAY_Tx_bind(FLEXRAY_Info flesxrayx, uint32 ID,uint8 messagebox);
    uint8 Get_FlexrayMcanMESSAGE_BOX_Num(FLEXRAY_Info flesxrayx,uint32 ID);
    int header_crc(wrhs *Fr_LPduPtr);
    void FlexRay_Set(uint8 direction,wrhs *Fr_LPduPtr);
    void  FlexRay_Send( uint8 boxnum);
    void Fr_TransmitTxRx(uint8 direction, bc *Fr_LSduPtr);
    int Fr_ControllerInit(void);
    int Fr_AllowColdStart(void);
#endif /* FLEXRAY_H_ */
