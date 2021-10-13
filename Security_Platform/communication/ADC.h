/*
 * ADC.h
 *
 *  Created on: 2021Äê10ÔÂ12ÈÕ
 *      Author: 15755
 */

#ifndef COMMUNICATION_ADC_H_
#define COMMUNICATION_ADC_H_
#include "HL_hal_stdtypes.h"

#define ADC1_SINGLE_DATA_LEN     8
#define ADC1_CONTINUES_DATA_LEN  8
#define ADC1_GROUP_1               1
#define ADC1_GROUP_2               2
#define DMA_ADC1_Single_channel         DMA_CH30
#define DMA_ADC1_Single_REQUEST_LINE    DMA_REQ11
#define DMA_ADC1_Continues_channel         DMA_CH31
#define DMA_ADC1_Continues_REQUEST_LINE    DMA_REQ10
#define CONVERSION_NO_FINISHED   0
#define CONVERSION_COMPLETE      1
#define ADCFIFOSIZE              16
#define ADCChanelNum             0x00000000
enum ADC_GROUP
{
    adcGROUP_EVENT=0,
    adcGROUP_1=1,
    adcGROUP_2=2,
};
typedef struct  _ADC_SINGLE_CHANNEL_INFO
{
    uint8 ch;
    uint32 data;

} ADC_Single_Channel_Info;

typedef struct  _ADC_CONTINUES_CHANNEL_INFO
{
    uint8 chs[ADC1_CONTINUES_DATA_LEN];
    uint32 data[ADC1_CONTINUES_DATA_LEN];

} ADC_Continues_Channel_Info;



#endif /* COMMUNICATION_ADC_H_ */
