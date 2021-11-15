/*
 * rti.c
 *
 *  Created on: 2021年10月9日
 *      Author: 15755
 */

#include"rti.h"
#include"HL_rti.h"
#include"ETH_EMAC.h"
extern uint32_t lwip_localtime; /*为DM900的LWIP维护提供始终*/
/******************
 * 函数：void WatchDog_init(void)
 * 功能：初始化看门狗
 * 输入：无
 * 输出：无
 * *******************/
void WatchDog_init(void)
{
    dwwdInit(rtiREG1, Generate_Reset , 4095, Size_100_Percent);
    dwdCounterEnable(rtiREG1);
}
/******************
 * 函数：void WatchDog_reset(void)
 * 功能：喂狗
 * 输入：无
 * 输出：无
 * *******************/
void WatchDog_reset(void)
{
    dwdReset(rtiREG1);
}

/******************
 * 函数：void CPU_reboot()
 * 功能：软件复位
 * 输入：无
 * 输出：无
 * *******************/
void CPU_reboot()
{
    dwdGenerateSysReset(rtiREG1);
}

/******************
 * 函数：void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
 * 功能：定时器中断服务函数
 * 输入：rtiREG：定时器地址. 唯一rtiREG1
 *     notification：中断源。rtiNOTIFICATION_COMPARE0、rtiNOTIFICATION_COMPARE1、
 *                         rtiNOTIFICATION_COMPARE2、rtiNOTIFICATION_COMPARE3
 * 输出：无
 * *******************/
void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
{
  if(rtiREG==rtiREG1)
  {
      if(notification==rtiNOTIFICATION_COMPARE0)   /*1ms*/
      {
          lwip_localtime++;
      }
      if(notification==rtiNOTIFICATION_COMPARE3)   /*10ms*/
      {
          EMAC_lwip_periodic_handle();
      }
  }
}
