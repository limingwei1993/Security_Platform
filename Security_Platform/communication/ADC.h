/*
 * ADC.h
 *
 *  Created on: 2021年10月12日
 *      Author: 15755
 */

#ifndef COMMUNICATION_ADC_H_
#define COMMUNICATION_ADC_H_
#include "HL_hal_stdtypes.h"

#define DMA_ADC1_Single_channel            DMA_CH30 /*ADC DMA单次采集时用来配置DMA的通道*/
#define DMA_ADC1_Continues_channel         DMA_CH31 /*ADC DMA连续采集时用来配置DMA的通道*/
enum ADC_CONVERSION_RESULT  /*ADC单次采集的状态*/
{
    CONVERSION_NO_FINISHED=0,
    CONVERSION_COMPLETE=1
};
#define ADC_CONTINUE_SDATA_Num             3   /*ADC连续采集时的保存数据个数*/
#define ADC_CONTINUE_SOURCE_Num            4   /*ADC连续采集时的通道数*/
enum adc1_Source  /*ADC的采集源*/
{
    ADC_EVENT = 0U,       /**< Alias for event pin             */
    ADC_HET1_8 = 1U,      /**< Alias for HET1 pin 8            */
    ADC_HET1_10 = 2U,     /**< Alias for HET1 pin 10           */
    ADC_RTI_COMP0 = 3U,   /**< Alias for RTI compare 0 match   */
    ADC_HET1_12 = 4U,     /**< Alias for HET1 pin 12           */
    ADC_HET1_14 = 5U,     /**< Alias for HET1 pin 14           */
    ADC_GIOB0 = 6U,       /**< Alias for GIO port b pin 0      */
    ADC_GIOB1 = 7U,       /**< Alias for GIO port b pin 1      */
    ADC_HET2_5 = 1U,      /**< Alias for HET2 pin 5            */
    ADC_HET1_27 = 2U,     /**< Alias for HET1 pin 27           */
    ADC_HET1_17 = 4U,     /**< Alias for HET1 pin 17           */
    ADC_HET1_19 = 5U,     /**< Alias for HET1 pin 19           */
    ADC_HET1_11 = 6U,     /**< Alias for HET1 pin 11           */
    ADC_HET2_13 = 7U,     /**< Alias for HET2 pin 13           */

    ADC_EPWM_B = 1U,      /**< Alias for B Signal EPWM         */
    ADC_EPWM_A1 = 3U,     /**< Alias for A1 Signal EPWM        */
    ADC_HET2_1 = 5U,      /**< Alias for HET2 pin 1            */
    ADC_EPWM_A2 = 6U,     /**< Alias for A2 Signal EPWM        */
    ADC_EPWM_AB = 7U      /**< Alias for AB Signal EPWM        */

};
/*ADC选择的组别*/
enum ADC_GROUP
{
    adcGROUP_EVENT=0,
    adcGROUP_1=1,
    adcGROUP_2=2,
};
/*ADC单次的参数*/
typedef struct  _ADC_SINGLE_CHANNEL_INFO
{
    uint8 ADC_group; /*ADC组*/
    uint8 source; /*采集源*/
    uint32 ch;  /*ADC通道 ,第0~31位分别对应了0~31通道，置1则启用*/
    uint32 data[32]; /*ADC 转换结果，*/
} ADC_Single_Channel_Info;
/*ADC连续采样的参数*/
typedef struct  _ADC_CONTINUES_CHANNEL_INFO
{
    uint8 ADC_group; /*ADC组*/
    uint8 source; /*采集源*/
    uint32 chs; /*ADC通道 ,第0~31位分别对应了0~31通道，置1则启用*/
    uint32 data[ADC_CONTINUE_SDATA_Num][ADC_CONTINUE_SOURCE_Num];
} ADC_Continues_Channel_Info;

void ADC_Single_Init(uint8_t group,uint8_t Source);
void ADC_Continuous_Init(uint8_t group,uint8_t Source);

void ADC_DMA_Single_Channel_init(ADC_Single_Channel_Info* pch);
void ADC_DMA_Continues_Channel_init(ADC_Continues_Channel_Info * pch);
void ADC_Continues_start(ADC_Continues_Channel_Info* pch);
void ADC_Continues_stop(ADC_Continues_Channel_Info* pch );
void ADC_Single_start(ADC_Single_Channel_Info* pch);
uint8 ADC_Single_isfinish(ADC_Single_Channel_Info* pch);
#endif /* COMMUNICATION_ADC_H_ */
