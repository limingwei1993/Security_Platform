/*
 * gpio.h
 *
 *  Created on: 2021Äê8ÔÂ3ÈÕ
 *      Author: 15755
 */

#ifndef COMMUNICATION_GPIO_H_
#define COMMUNICATION_GPIO_H_

#include "HL_hal_stdtypes.h"
#define GPIOA    1
#define GPIOB    2
#define N2HET1   3
#define N2HET2   4
#define INPUT_IO_NUM 8
#define OUTPUT_IO_NUM 8
typedef struct _GIO_INFO
{
    uint32 port;  /**/
    uint32 pin; /**/

} GIO_Info;

typedef enum _IO_STATE
{
    IO_LOW=0,
    IO_HIG=1,

} IO_STATE;

void GPO_Input_init(GIO_Info* pch);
void GPO_Output_init(GIO_Info* pch);
IO_STATE GPO_get(GIO_Info input_ch);
void GPO_set(GIO_Info output_ch, IO_STATE target);
#endif /* COMMUNICATION_GPIO_H_ */
