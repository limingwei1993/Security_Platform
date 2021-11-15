/*
 * ADC.h
 *
 *  Created on: 2021��10��12��
 *      Author: 15755
 */

#ifndef COMMUNICATION_ADC_H_
#define COMMUNICATION_ADC_H_
#include "HL_hal_stdtypes.h"

#define DMA_ADC1_Single_channel            DMA_CH30 /*ADC DMA���βɼ�ʱ��������DMA��ͨ��*/
#define DMA_ADC1_Continues_channel         DMA_CH31 /*ADC DMA�����ɼ�ʱ��������DMA��ͨ��*/
enum ADC_CONVERSION_RESULT  /*ADC���βɼ���״̬*/
{
    CONVERSION_NO_FINISHED=0,
    CONVERSION_COMPLETE=1
};
#define ADC_CONTINUE_SDATA_Num             3   /*ADC�����ɼ�ʱ�ı������ݸ���*/
#define ADC_CONTINUE_SOURCE_Num            4   /*ADC�����ɼ�ʱ��ͨ����*/
enum adc1_Source  /*ADC�Ĳɼ�Դ*/
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
/*ADCѡ������*/
enum ADC_GROUP
{
    adcGROUP_EVENT=0,
    adcGROUP_1=1,
    adcGROUP_2=2,
};
/*ADC���εĲ���*/
typedef struct  _ADC_SINGLE_CHANNEL_INFO
{
    uint8 ADC_group; /*ADC��*/
    uint8 source; /*�ɼ�Դ*/
    uint32 ch;  /*ADCͨ�� ,��0~31λ�ֱ��Ӧ��0~31ͨ������1������*/
    uint32 data[32]; /*ADC ת�������*/
} ADC_Single_Channel_Info;
/*ADC���������Ĳ���*/
typedef struct  _ADC_CONTINUES_CHANNEL_INFO
{
    uint8 ADC_group; /*ADC��*/
    uint8 source; /*�ɼ�Դ*/
    uint32 chs; /*ADCͨ�� ,��0~31λ�ֱ��Ӧ��0~31ͨ������1������*/
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
