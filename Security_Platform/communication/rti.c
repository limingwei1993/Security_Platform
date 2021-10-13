/*
 * rti.c
 *
 *  Created on: 2021Äê10ÔÂ9ÈÕ
 *      Author: 15755
 */

#include"rti.h"
#include"HL_rti.h"
extern uint32_t lwip_localtime;
void WatchDog_init(void)
{
    dwwdInit(rtiREG1, Generate_Reset , 4095, Size_100_Percent);
    dwdCounterEnable(rtiREG1);
}
void WatchDog_reset(void)
{
    dwdReset(rtiREG1);
}


void CPU_reboot()
{
    dwdGenerateSysReset(rtiREG1);
}


void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
{
  if(rtiREG==rtiREG1)
  {
      if(notification==rtiNOTIFICATION_COMPARE0)   /*1ms*/
      {
          lwip_localtime++;
      }
  }
}
