/*
 * ADC.c
 *
 *  Created on: 2021Äê10ÔÂ12ÈÕ
 *      Author: 15755
 */

#include"ADC.h"
#include"HL_adc.h"
#include"HL_reg_adc.h"
#include "HL_reg_dma.h"
#include "HL_sys_dma.h"
ADC_Single_Channel_Info ADC_Signle_channel[ADC1_SINGLE_DATA_LEN];
g_dmaCTRL g_dmaCTRLPKT_ADC1_Single;
g_dmaCTRL g_dmaCTRLPKT_ADC1_Continues;
void ADC_Single_Init(uint8_t group)
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
                          | (uint32)ADC1_EVENT;  /*trigger source*/
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
                          | (uint32)ADC1_GIOB0;  /*trigger source*/

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
                          | (uint32)ADC1_EVENT;  /*trigger source*/

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

void ADC_Continuous_Init(uint8_t group)
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
                          | (uint32)ADC1_EVENT;  /*trigger source*/
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
                          | (uint32)ADC1_GIOB0;  /*trigger source*/

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
                          | (uint32)ADC1_EVENT;  /*trigger source*/

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

void ADC_DMA_Single_Channel_init(ADC_Single_Channel_Info* pch)
{
    dmaReqAssign(DMA_ADC1_Single_channel, DMA_ADC1_Single_REQUEST_LINE);
    g_dmaCTRLPKT_ADC1_Single.SADD      =((uint32_t)(&(adcREG1->GxBUF[ADC1_GROUP_2].BUF0)))  ;
    g_dmaCTRLPKT_ADC1_Single.DADD      =(uint32)(pch+1) ;
    g_dmaCTRLPKT_ADC1_Single.CHCTRL    = 0;
    g_dmaCTRLPKT_ADC1_Single.FRCNT = ADC1_SINGLE_DATA_LEN;
    g_dmaCTRLPKT_ADC1_Single.ELCNT = 1;
    g_dmaCTRLPKT_ADC1_Single.ELDOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Single.ELSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Single.FRDOFFSET = 1;
    g_dmaCTRLPKT_ADC1_Single.FRSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Single.PORTASGN  = PORTB_READ_PORTA_WRITE;
    g_dmaCTRLPKT_ADC1_Single.RDSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Single.WRSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Single.TTYPE     = FRAME_TRANSFER ;
    g_dmaCTRLPKT_ADC1_Single.ADDMODERD = ADDR_FIXED;
    g_dmaCTRLPKT_ADC1_Single.ADDMODEWR = ADDR_INC1;
    g_dmaCTRLPKT_ADC1_Single.AUTOINIT  = AUTOINIT_OFF;
    dmaSetCtrlPacket(DMA_ADC1_Single_channel,g_dmaCTRLPKT_ADC1_Single);
    adcREG1->G1RAMADDR |=0x00000001;

}

void ADC_DMA_Continues_Channel_init(ADC_Continues_Channel_Info pch)
{

    dmaReqAssign(DMA_ADC1_Continues_channel, DMA_ADC1_Continues_REQUEST_LINE);
    g_dmaCTRLPKT_ADC1_Continues.SADD      =((uint32_t)(&(adcREG1->GxBUF[ADC1_GROUP_1].BUF0)))  ;
    g_dmaCTRLPKT_ADC1_Continues.DADD      =(uint32)(pch.data) ;
    g_dmaCTRLPKT_ADC1_Continues.CHCTRL    = 0;
    g_dmaCTRLPKT_ADC1_Continues.FRCNT = ADC1_CONTINUES_DATA_LEN;
    g_dmaCTRLPKT_ADC1_Continues.ELCNT = 1;
    g_dmaCTRLPKT_ADC1_Continues.ELDOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Continues.ELSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Continues.FRDOFFSET = 1;
    g_dmaCTRLPKT_ADC1_Continues.FRSOFFSET = 0;
    g_dmaCTRLPKT_ADC1_Continues.PORTASGN  = PORTB_READ_PORTA_WRITE;
    g_dmaCTRLPKT_ADC1_Continues.RDSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Continues.WRSIZE    = ACCESS_32_BIT;
    g_dmaCTRLPKT_ADC1_Continues.TTYPE     = FRAME_TRANSFER ;
    g_dmaCTRLPKT_ADC1_Continues.ADDMODERD = ADDR_FIXED;
    g_dmaCTRLPKT_ADC1_Continues.ADDMODEWR = ADDR_INC1;
    g_dmaCTRLPKT_ADC1_Continues.AUTOINIT  = AUTOINIT_ON;
    dmaSetCtrlPacket(DMA_ADC1_Continues_channel,g_dmaCTRLPKT_ADC1_Continues);
    adcREG1->G2RAMADDR |=0x00000001;
 //   dmaSetChEnable(DMA_ADC1_channel, DMA_HW);
 //   dmaEnableInterrupt(DMA_ADC1_channel, BTC, DMA_INTA);

}

void ADC_Continues_start(void )
{
    adcREG1->GxINTCR[ADC1_GROUP_1] = ADCFIFOSIZE;
    adcREG1->GxSEL[ADC1_GROUP_1] = ADCChanelNum;

    dmaSetChEnable(DMA_ADC1_Continues_channel, DMA_HW);

}

void ADC_Continues_stop(void )
{
    adcREG1->GxSEL[ADC1_GROUP_1] = 0U;

}

void ADC_Single_start(ADC_Single_Channel_Info* pch)
{
    adcREG1->GxINTCR[ADC1_GROUP_2] = ADCFIFOSIZE;
    adcREG1->GxSEL[ADC1_GROUP_2] = ADCChanelNum;

    dmaSetChEnable(DMA_ADC1_Single_channel, DMA_HW);
}

uint8 ADC_Single_isfinish()
{
    uint32 result=0;
    result= adcREG1->GxINTFLG[ADC1_GROUP_2] & 8U;
    if(result==8)
    {
        return CONVERSION_COMPLETE;
    }
    else
    {
        return CONVERSION_NO_FINISHED;
    }

}
