/*
 * rti.c
 *
 *  Created on: 2021��10��9��
 *      Author: 15755
 */

#include"rti.h"
#include"HL_rti.h"
#include"ETH_EMAC.h"
extern uint32_t lwip_localtime; /*ΪDM900��LWIPά���ṩʼ��*/
/******************
 * ������void WatchDog_init(void)
 * ���ܣ���ʼ�����Ź�
 * ���룺��
 * �������
 * *******************/
void WatchDog_init(void)
{
    dwwdInit(rtiREG1, Generate_Reset , 4095, Size_100_Percent);
    dwdCounterEnable(rtiREG1);
}
/******************
 * ������void WatchDog_reset(void)
 * ���ܣ�ι��
 * ���룺��
 * �������
 * *******************/
void WatchDog_reset(void)
{
    dwdReset(rtiREG1);
}

/******************
 * ������void CPU_reboot()
 * ���ܣ������λ
 * ���룺��
 * �������
 * *******************/
void CPU_reboot()
{
    dwdGenerateSysReset(rtiREG1);
}

/******************
 * ������void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
 * ���ܣ���ʱ���жϷ�����
 * ���룺rtiREG����ʱ����ַ. ΨһrtiREG1
 *     notification���ж�Դ��rtiNOTIFICATION_COMPARE0��rtiNOTIFICATION_COMPARE1��
 *                         rtiNOTIFICATION_COMPARE2��rtiNOTIFICATION_COMPARE3
 * �������
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
