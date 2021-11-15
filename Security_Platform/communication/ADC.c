/*
 * ADC.c
 *
 *  Created on: 2021年10月12日
 *      Author: 15755
 */

#include"ADC.h"
#include"HL_adc.h"
#include"HL_reg_adc.h"
#include "HL_reg_dma.h"
#include "HL_sys_dma.h"
g_dmaCTRL g_dmaCTRLPKT_ADC1_Single;    /*ADC单次转换的DMA配置句柄*/
g_dmaCTRL g_dmaCTRLPKT_ADC1_Continues; /*ADC连续转换的DMA配置句柄*/
uint8_t DMA_ADC1_Single_REQUEST_LINE[3]={DMA_REQ7,DMA_REQ10,DMA_REQ11};/*ADC连续转换的DMA中断线*/
uint8_t DMA_ADC1_Continues_REQUEST_LINE[3]={DMA_REQ7,DMA_REQ10,DMA_REQ11};/*ADC连续转换的DMA中断线*/
extern  uint32 s_adcSelect[2U][3U];    /*ADC采样的通道标志，【2】--ADC1、ADC2；【3】--EVENT、goup1、group2*/
/******************
 * 函数：void ADC_Single_Init(uint8_t group,uint8_t Source)
 * 功能：ADC单次采样初始化
 * 输入：group： 使用ADC的那个组；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *     Source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
 * 输出：无
 * *******************/
void ADC_Single_Init(uint8_t group,uint8_t Source)
{
    /** - Reset ADC module */
    adcREG1->RSTCR = 1U;
    adcREG1->RSTCR = 0U;
    /** - Enable 12-BIT ADC  */
    adcREG1->OPMODECR |= 0x80000000U;
    /** - Setup prescaler */
    adcREG1->CLOCKCR = 7U;
    /** - Setup memory boundaries */
    adcREG1->BNDCR  = (uint32)((uint32)8U << 16U) | (8U + 8U);
    adcREG1->BNDEND = (adcREG1->BNDEND & 0xFFFF0000U) | (2U);
    switch(group)
    {
        case adcGROUP_EVENT:
            /** - Setup event group conversion mode
           *     - Setup data format
           *     - Enable/Disable channel id in conversion result
           *     - Enable/Disable continuous conversion
           */
           adcREG1->GxMODECR[0U] = (uint32)ADC_12_BIT
                                 | (uint32)0x00000020U  /*Enable channel id*/
                                 | (uint32)0x00000000U; /*Disable continuous*/
           /** - Setup event group hardware trigger
            *     - Setup hardware trigger edge
            *     - Setup hardware trigger source
            */
           adcREG1->EVSRC = (uint32)0x00000008U  /*Rising Edge*/
                          | (uint32)Source;  /*trigger source*/
           /** - Setup event group sample window */
           adcREG1->EVSAMP = 1U;
           /** - Setup event group sample discharge
           *     - Setup discharge prescaler
           *     - Enable/Disable discharge
           */
           adcREG1->EVSAMPDISEN = (uint32)((uint32)0U << 8U)
                                    | (uint32)0x00000000U;
        break;
        case adcGROUP_1:
            /** - Setup group 1 conversion mode
           *     - Setup data format
           *     - Enable/Disable channel id in conversion result
           *     - Enable/Disable continuous conversion
           */
           adcREG1->GxMODECR[1U] = (uint32)ADC_12_BIT
                                 | (uint32)0x00000020U  /*Enable channel id*/
                                 | (uint32)0x00000008U  /*hardware-triggered*/
                                 | (uint32)0x00000000U; /*Disable continuous*/

           /** - Setup group 1 hardware trigger
            *     - Setup hardware trigger edge
            *     - Setup hardware trigger source
            */
           adcREG1->G1SRC = (uint32)0x00000008U  /*Rising Edge*/
                          | (uint32)Source;  /*trigger source*/

           /** - Setup group 1 sample window */
           adcREG1->G1SAMP = 1U;

           /** - Setup group 1 sample discharge
           *     - Setup discharge prescaler
           *     - Enable/Disable discharge
           */
           adcREG1->G1SAMPDISEN = (uint32)((uint32)0U << 8U)
                                    | (uint32)0x00000000U;
        break;
        case adcGROUP_2:
            /** - Setup group 2 conversion mode
            *     - Setup data format
            *     - Enable/Disable channel id in conversion result
            *     - Enable/Disable continuous conversion
            */
           adcREG1->GxMODECR[2U] = (uint32)ADC_12_BIT
                                 | (uint32)0x00000020U  /*Enable channel id*/
                                 | (uint32)0x00000008U  /*hardware-triggered*/
                                 | (uint32)0x00000000U; /*Disable continuous*/

           /** - Setup group 2 hardware trigger
           *     - Setup hardware trigger edge
           *     - Setup hardware trigger source
           */
           adcREG1->G2SRC = (uint32)0x00000008U  /*Rising Edge*/
                          | (uint32)Source;  /*trigger source*/

           /** - Setup group 2 sample window */
           adcREG1->G2SAMP = 1U;

           /** - Setup group 2 sample discharge
           *     - Setup discharge prescaler
           *     - Enable/Disable discharge
           */
           adcREG1->G2SAMPDISEN = (uint32)((uint32)0U << 8U)
                                    | (uint32)0x00000000U;
        break;
        default:
        break;
    }
    /** - ADC1 EVT pin output value */
    adcREG1->EVTOUT = 0U;  /*Output logic LOW*/

    /** - ADC1 EVT pin direction */
    adcREG1->EVTDIR = 0U;  /*ADEVT is an input pin*/

    /** - ADC1 EVT pin open drain enable */
    adcREG1->EVTPDR = 0U;   /*Output logic LOW*/

    /** - ADC1 EVT pin pullup / pulldown selection */
    adcREG1->EVTPSEL = 1U;

    /** - ADC1 EVT pin pullup / pulldown enable*/
    adcREG1->EVTDIS = 0U;

    /** - Enable ADC module */
    adcREG1->OPMODECR |= 0x80140001U;/*The ADC core and digital logic are configured to be in 12-bit resolution // Input impedance measurement mode is disabled //  ADC Enable*/

    /** - Wait for buffer initialization complete */
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    while (((adcREG1->BNDEND & 0xFFFF0000U) >> 16U ) != 0U)
    {
    } /* Wait */

    /** - Setup parity */
    adcREG1->PARCR = 0x00000005U;
}
/******************
 * 函数：void ADC_Continuous_Init(uint8_t group,uint8_t Source)
 * 功能：ADC连续采样初始化
 * 输入：group： 使用ADC的那个组；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *     Source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
 * 输出：无
 * *******************/
void ADC_Continuous_Init(uint8_t group,uint8_t Source)
{
    /** - Reset ADC module */
    adcREG1->RSTCR = 1U;
    adcREG1->RSTCR = 0U;
    /** - Enable 12-BIT ADC  */
    adcREG1->OPMODECR |= 0x80000000U;
    /** - Setup prescaler */
    adcREG1->CLOCKCR = 7U;
    /** - Setup memory boundaries */
    adcREG1->BNDCR  = (uint32)((uint32)8U << 16U) | (8U + 8U);
    adcREG1->BNDEND = (adcREG1->BNDEND & 0xFFFF0000U) | (2U);
    switch(group)
    {
        case adcGROUP_EVENT:
            /** - Setup event group conversion mode
           *     - Setup data format
           *     - Enable/Disable channel id in conversion result
           *     - Enable/Disable continuous conversion
           */
           adcREG1->GxMODECR[0U] = (uint32)ADC_12_BIT
                                 | (uint32)0x00000020U  /*Enable channel id*/
                                 | (uint32)0x00000002U; /*Enable continuous*/
           /** - Setup event group hardware trigger
            *     - Setup hardware trigger edge
            *     - Setup hardware trigger source
            */
           adcREG1->EVSRC = (uint32)0x00000008U  /*Rising Edge*/
                          | (uint32)Source;  /*trigger source*/
           /** - Setup event group sample window */
           adcREG1->EVSAMP = 1U;
           /** - Setup event group sample discharge
           *     - Setup discharge prescaler
           *     - Enable/Disable discharge
           */
           adcREG1->EVSAMPDISEN = (uint32)((uint32)0U << 8U)
                                    | (uint32)0x00000000U;
        break;
        case adcGROUP_1:
            /** - Setup group 1 conversion mode
           *     - Setup data format
           *     - Enable/Disable channel id in conversion result
           *     - Enable/Disable continuous conversion
           */
           adcREG1->GxMODECR[1U] = (uint32)ADC_12_BIT
                                 | (uint32)0x00000020U  /*Enable channel id*/
                                 | (uint32)0x00000008U  /*hardware-triggered*/
                                 | (uint32)0x00000002U; /*Enable continuous*/

           /** - Setup group 1 hardware trigger
            *     - Setup hardware trigger edge
            *     - Setup hardware trigger source
            */
           adcREG1->G1SRC = (uint32)0x00000008U  /*Rising Edge*/
                          | (uint32)Source;  /*trigger source*/

           /** - Setup group 1 sample window */
           adcREG1->G1SAMP = 1U;

           /** - Setup group 1 sample discharge
           *     - Setup discharge prescaler
           *     - Enable/Disable discharge
           */
           adcREG1->G1SAMPDISEN = (uint32)((uint32)0U << 8U)
                                    | (uint32)0x00000000U;
        break;
        case adcGROUP_2:
            /** - Setup group 2 conversion mode
            *     - Setup data format
            *     - Enable/Disable channel id in conversion result
            *     - Enable/Disable continuous conversion
            */
           adcREG1->GxMODECR[2U] = (uint32)ADC_12_BIT
                                 | (uint32)0x00000020U  /*Enable channel id*/
                                 | (uint32)0x00000008U  /*hardware-triggered*/
                                 | (uint32)0x00000002U; /*Enable continuous*/

           /** - Setup group 2 hardware trigger
           *     - Setup hardware trigger edge
           *     - Setup hardware trigger source
           */
           adcREG1->G2SRC = (uint32)0x00000008U  /*Rising Edge*/
                          | (uint32)Source;  /*trigger source*/

           /** - Setup group 2 sample window */
           adcREG1->G2SAMP = 1U;

           /** - Setup group 2 sample discharge
           *     - Setup discharge prescaler
           *     - Enable/Disable discharge
           */
           adcREG1->G2SAMPDISEN = (uint32)((uint32)0U << 8U)
                                    | (uint32)0x00000000U;
        break;
        default:
        break;
    }
    /** - ADC1 EVT pin output value */
    adcREG1->EVTOUT = 0U;  /*Output logic LOW*/

    /** - ADC1 EVT pin direction */
    adcREG1->EVTDIR = 0U;  /*ADEVT is an input pin*/

    /** - ADC1 EVT pin open drain enable */
    adcREG1->EVTPDR = 0U;   /*Output logic LOW*/

    /** - ADC1 EVT pin pullup / pulldown selection */
    adcREG1->EVTPSEL = 1U;

    /** - ADC1 EVT pin pullup / pulldown enable*/
    adcREG1->EVTDIS = 0U;

    /** - Enable ADC module */
    adcREG1->OPMODECR |= 0x80140001U;/*The ADC core and digital logic are configured to be in 12-bit resolution // Input impedance measurement mode is disabled //  ADC Enable*/

    /** - Wait for buffer initialization complete */
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    while (((adcREG1->BNDEND & 0xFFFF0000U) >> 16U ) != 0U)
    {
    } /* Wait */

    /** - Setup parity */
    adcREG1->PARCR = 0x00000005U;
}
/******************
 * 函数：void ADC_DMA_Single_Channel_init(ADC_Single_Channel_Info* pch)
 * 功能：ADC采用DMA单次采样初始化
 * 输入：pch：配置参数句柄。->ADC_group:采用的组别；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *                    ->ch：采集的通道参数；第0~31位分别对应了0~31通道，置1则启用。
 *                    ->source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
                       ->data：保存采集结果
 * 输出：无
 * *******************/
void ADC_DMA_Single_Channel_init(ADC_Single_Channel_Info* pch)
{
    uint8 i=0;
    uint8 CH_num=0;
    for(i=0;i<32;i++)
    {
        if(( 0x0001 << i) &pch->ch)
        {
            CH_num++;
        }
    }
    if(pch->source>adcGROUP_2) return;
    s_adcSelect[0][pch->ADC_group]=pch->ch;
    ADC_Single_Init(pch->ADC_group,pch->source);
    dmaReqAssign(DMA_ADC1_Single_channel, DMA_ADC1_Single_REQUEST_LINE[pch->ADC_group]);
    g_dmaCTRLPKT_ADC1_Single.SADD      =((uint32_t)(&(adcREG1->GxBUF[pch->ADC_group].BUF0)))  ;
    g_dmaCTRLPKT_ADC1_Single.DADD      =(uint32)(pch->data) ;
    g_dmaCTRLPKT_ADC1_Single.CHCTRL    = 0;
    g_dmaCTRLPKT_ADC1_Single.FRCNT = CH_num;
    g_dmaCTRLPKT_ADC1_Single.ELCNT = 1;
    g_dmaCTRLPKT_ADC1_Single.ELDOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Single.ELSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Single.FRDOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Single.FRSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Single.PORTASGN  = PORTB_READ_PORTA_WRITE;
    g_dmaCTRLPKT_ADC1_Single.RDSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Single.WRSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Single.TTYPE     = FRAME_TRANSFER ;
    g_dmaCTRLPKT_ADC1_Single.ADDMODERD = ADDR_FIXED;
    g_dmaCTRLPKT_ADC1_Single.ADDMODEWR = ADDR_INC1;
    g_dmaCTRLPKT_ADC1_Single.AUTOINIT  = AUTOINIT_OFF;
    dmaSetCtrlPacket(DMA_ADC1_Single_channel,g_dmaCTRLPKT_ADC1_Single);
    switch(pch->ADC_group)
    {
     case adcGROUP_EVENT:
         adcREG1->EVDMACR |=(0x0000000F);
     break;
     case adcGROUP_1:
         adcREG1->G1DMACR |=(0x00000001 | (CH_num << 16));
     break;
     case adcGROUP_2:
         adcREG1->G2DMACR |=(0x0000000F);
     break;

    }
}

/******************
 * 函数：void ADC_DMA_Continues_Channel_init(ADC_Continues_Channel_Info * pch)
 * 功能：ADC采用DMA连续采样初始化
 * 输入：pch：配置参数句柄。->ADC_group:采用的组别；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *                    ->ch：采集的通道参数；第0~31位分别对应了0~31通道，置1则启用。
 *                    ->source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
                       ->data：保存采集结果。data[0][]--保存第1次采样；data[1][]--保存第2次采样；
 * 输出：无
 * 注意：使用此函数时，应在ADC.h中修改ADC_CONTINUE_SDATA_Num 和 define ADC_CONTINUE_SOURCE_Num 的值。
 *     ADC_CONTINUE_SDATA_Num--连续采集的次数；define ADC_CONTINUE_SOURCE_Num--采集通道的个数。
 * *******************/
void ADC_DMA_Continues_Channel_init(ADC_Continues_Channel_Info * pch)
{
    uint8 i=0;
    uint8 CH_num=0;
    for(i=0;i<32;i++)
    {
        if(( 0x0001 << i) &pch->chs)
        {
            CH_num++;
        }
    }
    if(pch->source>adcGROUP_2) return;
    s_adcSelect[0][pch->ADC_group]=pch->chs;
    ADC_Continuous_Init(pch->ADC_group,pch->source);
    dmaReqAssign(DMA_ADC1_Continues_channel, DMA_ADC1_Continues_REQUEST_LINE[pch->ADC_group]);
    g_dmaCTRLPKT_ADC1_Continues.SADD      =((uint32_t)(&(adcREG1->GxBUF[pch->ADC_group].BUF0)))  ;
    g_dmaCTRLPKT_ADC1_Continues.DADD      =(uint32)(pch->data) ;
    g_dmaCTRLPKT_ADC1_Continues.CHCTRL    = 0;
    g_dmaCTRLPKT_ADC1_Continues.FRCNT = (CH_num*ADC_CONTINUE_SDATA_Num);
    g_dmaCTRLPKT_ADC1_Continues.ELCNT = 1;
    g_dmaCTRLPKT_ADC1_Continues.ELDOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Continues.ELSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Continues.FRDOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Continues.FRSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Continues.PORTASGN  = PORTB_READ_PORTA_WRITE;
    g_dmaCTRLPKT_ADC1_Continues.RDSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Continues.WRSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Continues.TTYPE     = FRAME_TRANSFER ;
    g_dmaCTRLPKT_ADC1_Continues.ADDMODERD = ADDR_FIXED;
    g_dmaCTRLPKT_ADC1_Continues.ADDMODEWR = ADDR_INC1;
    g_dmaCTRLPKT_ADC1_Continues.AUTOINIT  = AUTOINIT_ON;
    dmaSetCtrlPacket(DMA_ADC1_Continues_channel,g_dmaCTRLPKT_ADC1_Continues);
    switch(pch->ADC_group)
    {
     case adcGROUP_EVENT:
         adcREG1->EVDMACR |=(0x0000000F);
     break;
     case adcGROUP_1:
         adcREG1->G1DMACR |=(0x00000001 | (CH_num << 16));
     break;
     case adcGROUP_2:
         adcREG1->G2DMACR |=(0x0000000F);
     break;

    }

}
/******************
 * 函数：void ADC_Continues_start(ADC_Continues_Channel_Info* pch)
 * 功能：ADC连续采样开始
 * 输入：pch：配置参数句柄。->ADC_group:采用的组别；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *                    ->ch：采集的通道参数；第0~31位分别对应了0~31通道，置1则启用。
 *                    ->source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
                       ->data：保存采集结果。data[0][]--保存第1次采样；data[1][]--保存第2次采样；
 * 输出：无
 * 注意：使用此函数时，应在ADC.h中修改ADC_CONTINUE_SDATA_Num 和 define ADC_CONTINUE_SOURCE_Num 的值。
 *     ADC_CONTINUE_SDATA_Num--连续采集的次数；define ADC_CONTINUE_SOURCE_Num--采集通道的个数。
 * *******************/
void ADC_Continues_start(ADC_Continues_Channel_Info* pch)
{
    adcREG1->GxINTCR[pch->ADC_group] = 16;
    adcREG1->GxSEL[pch->ADC_group] = s_adcSelect[0][pch->ADC_group];;
    dmaSetChEnable(DMA_ADC1_Continues_channel, DMA_HW);

}
/******************
 * 函数：void ADC_Continues_stop(ADC_Continues_Channel_Info* pch )
 * 功能：ADC连续采样结束
 * 输入：pch：配置参数句柄。->ADC_group:采用的组别；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *                    ->ch：采集的通道参数；第0~31位分别对应了0~31通道，置1则启用。
 *                    ->source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
                       ->data：保存采集结果。data[0][]--保存第1次采样；data[1][]--保存第2次采样；
 * 输出：无
 * 注意：使用此函数时，应在ADC.h中修改ADC_CONTINUE_SDATA_Num 和 define ADC_CONTINUE_SOURCE_Num 的值。
 *     ADC_CONTINUE_SDATA_Num--连续采集的次数；define ADC_CONTINUE_SOURCE_Num--采集通道的个数。
 * *******************/
void ADC_Continues_stop(ADC_Continues_Channel_Info* pch )
{
    adcREG1->GxSEL[pch->ADC_group] = 0U;

}
/******************
 * 函数：void ADC_Single_start(ADC_Single_Channel_Info* pch)
 * 功能：ADC单次采样开始
 * 输入：pch：配置参数句柄。->ADC_group:采用的组别；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *                    ->ch：采集的通道参数；第0~31位分别对应了0~31通道，置1则启用。
 *                    ->source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
                       ->data：保存采集结果。
 * 输出：无
 * *******************/
void ADC_Single_start(ADC_Single_Channel_Info* pch)
{
    adcREG1->GxINTCR[pch->ADC_group] = 16;
    adcREG1->GxSEL[pch->ADC_group] = s_adcSelect[0][pch->ADC_group];
    dmaSetChEnable(DMA_ADC1_Single_channel, DMA_HW);
}
/******************
 * 函数：uint8 ADC_Single_isfinish(ADC_Single_Channel_Info* pch)
 * 功能：查询ADC单次采样是否结束
 * 输入：pch：配置参数句柄。->ADC_group:采用的组别；可选：adcGROUP_EVENT、adcGROUP_1、adcGROUP_2。
 *                    ->ch：采集的通道参数；第0~31位分别对应了0~31通道，置1则启用。
 *                    ->source：采样的输入源；可选：ADC_EVENT 、ADC_HET1_8、ADC_HET1_10、ADC_RTI_COMP0、ADC_HET1_12 、
                             ADC_HET1_14、ADC_GIOB0、ADC_GIOB1、ADC_HET2_5、ADC_HET1_27、ADC_HET1_17
                             ADC_HET1_19、ADC_HET1_11、ADC_HET2_13、ADC_EPWM_B、ADC_EPWM_A1、ADC_HET2_1
                             ADC_EPWM_A2、ADC_EPWM_AB
                       ->data：保存采集结果。
 * 输出：返回单次采样是否结束结果；CONVERSION_COMPLETE--完成；CONVERSION_NO_FINISHED--没有完成
 * *******************/
uint8 ADC_Single_isfinish(ADC_Single_Channel_Info* pch)
{
    uint32 result=0;
    result= adcREG1->GxINTFLG[pch->ADC_group] & 8U;
    if(result!=0)
    {
        return CONVERSION_COMPLETE;
    }
    else
    {
        return CONVERSION_NO_FINISHED;
    }

}
