/*
 * gpio.c
 *
 *  Created on: 2021��8��3��
 *      Author: 15755
 */

#include "gpio.h"
#include "HL_reg_gio.h"
#include "HL_reg_het.h"
/*����I/O����Ϣ*/
GIO_Info Input_IO_list[INPUT_IO_NUM]={{GPIOA,0},{GPIOA,1},{GPIOA,2},{GPIOA,3},{GPIOA,4},{GPIOA,5},{GPIOA,6},{GPIOA,7}};
/*���I/O����Ϣ*/
GIO_Info Outnput_IO_list[OUTPUT_IO_NUM]={{GPIOB,0},{GPIOB,1},{GPIOB,2},{GPIOB,3},{GPIOB,4},{GPIOB,5},{GPIOB,6},{GPIOB,7}};
/******************
 * ������void GPO_Input_init(GIO_Info* pch)
 * ���ܣ�I/O�� �����ʼ��
 * ���룺pch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 * �������
 * *******************/
void GPO_Input_init(GIO_Info* pch)
{
    uint8 i=0;
        gioREG->GCR0   = 1U;
        gioREG->ENACLR = 0xFFU;
        gioREG->LVLCLR = 0xFFU;

        /** @b initialize @b Port @b A */
       for(i=0;i<16;i++)
       {
           if(pch[i].port==GPIOA &&(  pch[i].pin<=7) )
           {
               gioPORTA->DIR  &=~(uint32)(1<<pch[i].pin);      /** - Port A direction 0--INPUT  1--OUT*/
               gioPORTA->DOUT &=~(uint32)(1<<pch[i].pin);     /** - Port A output values */
               gioPORTA->PDR  &= ~(uint32)(1<<pch[i].pin);    /** - Port A open drain enable  0--push/pull  1--open drain*/
               gioPORTA->PSL  &= ~(uint32)(1<<pch[i].pin);    /** - Port A pullup / pulldown selection   0--down  1--up*/
               gioPORTA->PULDIS &= ~(uint32)(1<<pch[i].pin);  /** - Port A pullup / pulldown enable  0--ENABLE*/
           }
           else if(pch[i].port==GPIOB &&( pch[i].pin<=7) )
           {
               gioPORTB->DIR  &=~(uint32)(1<<pch[i].pin);
               gioPORTB->DOUT &=~(uint32)(1<<pch[i].pin);
               gioPORTB->PDR  &= ~(uint32)(1<<pch[i].pin);
               gioPORTB->PSL  &= ~(uint32)(1<<pch[i].pin);
               gioPORTB->PULDIS &= ~(uint32)(1<<pch[i].pin);
           }
       }
}
/******************
 * ������void GPO_Output_init(GIO_Info* pch)
 * ���ܣ�I/O�� �����ʼ��
 * ���룺pch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 * �������
 * *******************/
void GPO_Output_init(GIO_Info* pch)
{
        uint8 i=0;
        gioREG->GCR0   = 1U;
        gioREG->ENACLR = 0xFFU;
        gioREG->LVLCLR = 0xFFU;

        /** @b initialize @b Port @b A */
       for(i=0;i<16;i++)
       {
           if(pch[i].port==GPIOA &&( pch[i].pin<=7) )
           {
               gioPORTA->DIR  |=(uint32)(1<<pch[i].pin);      /** - Port A direction 0--INPUT  1--OUT*/
               gioPORTA->DOUT &=~(uint32)(1<<pch[i].pin);     /** - Port A output values */
               gioPORTA->PDR  |= (uint32)(1<<pch[i].pin);    /** - Port A open drain enable  0--push/pull  1--open drain*/
               gioPORTA->PSL  &= ~(uint32)(1<<pch[i].pin);    /** - Port A pullup / pulldown selection   0--down  1--up*/
               gioPORTA->PULDIS &= ~(uint32)(1<<pch[i].pin);  /** - Port A pullup / pulldown enable  0--ENABLE*/
           }
           else if(pch[i].port==GPIOB &&( pch[i].pin<=7) )
           {
               gioPORTB->DIR  |=(uint32)(1<<pch[i].pin);
               gioPORTB->DOUT &=~(uint32)(1<<pch[i].pin);
               gioPORTB->PDR  |= (uint32)(1<<pch[i].pin);
               gioPORTB->PSL  &= ~(uint32)(1<<pch[i].pin);
               gioPORTB->PULDIS &= ~(uint32)(1<<pch[i].pin);
           }
       }
}
/******************
 * ������IO_STATE GPO_get(GIO_Info input_ch)
 * ���ܣ���ȡI/O�� ״̬
 * ���룺input_ch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 * �����I/O��״̬
 * *******************/
IO_STATE GPO_get(GIO_Info input_ch)
{
    IO_STATE sate=IO_LOW;
    if(input_ch.port==GPIOA &&( input_ch.pin<=7) )
    {
        sate= ((gioPORTA->DIN >> input_ch.pin) & 1U)>0? IO_HIG:IO_LOW;
    }
    else if(input_ch.port==GPIOB &&( input_ch.pin<=7) )
    {
        sate= ((gioPORTB->DIN >> input_ch.pin) & 1U)>0? IO_HIG:IO_LOW;
    }
    return sate;
}
/******************
 * ������void GPO_set(GIO_Info output_ch, IO_STATE target)
 * ���ܣ�����I/O�� ״̬
 * ���룺input_ch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 *      target��I/O��״̬����ѡIO_LOW��IO_LOW=0
 * �������
 * *******************/
void GPO_set(GIO_Info output_ch, IO_STATE target)
{
    if(output_ch.port==GPIOA &&(  output_ch.pin<=7) )
    {
         gioPORTA->DOUT &= ~(1>> output_ch.pin);
         gioPORTA->DOUT |= (target>> output_ch.pin);
    }
    else if(output_ch.port==GPIOB &&( output_ch.pin<=7) )
    {
         gioPORTB->DOUT &= ~(1>> output_ch.pin);
         gioPORTB->DOUT |= (target>> output_ch.pin);
    }
}

/******************
 * ������void N2HET_As_Gio_Input_init(GIO_Info* pch)
 * ���ܣ�N2HET �� �����ʼ��
 * ���룺pch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 * �������
 * *******************/
void N2HET_As_Gio_Input_init(GIO_Info* pch)
{
   uint8 i=0;
   for(i=0;i<62;i++)
   {
       if(pch[i].port==N2HET1 &&( pch[i].pin<=31) )
       {
           hetPORT1->DIR  &=~(uint32)(1<<pch[i].pin);      /** - Port A direction 0--INPUT  1--OUT*/
           hetPORT1->DOUT &=~(uint32)(1<<pch[i].pin);     /** - Port A output values */
           hetPORT1->PDR  &= ~(uint32)(1<<pch[i].pin);    /** - Port A open drain enable  0--push/pull  1--open drain*/
           hetPORT1->PSL  &= ~(uint32)(1<<pch[i].pin);    /** - Port A pullup / pulldown selection   0--down  1--up*/
           hetPORT1->PULDIS &= ~(uint32)(1<<pch[i].pin);  /** - Port A pullup / pulldown enable  0--ENABLE*/
       }
       else if(pch[i].port==N2HET2 &&( pch[i].pin<=31) )
       {
           hetPORT2->DIR  &=~(uint32)(1<<pch[i].pin);
           hetPORT2->DOUT &=~(uint32)(1<<pch[i].pin);
           hetPORT2->PDR  &= ~(uint32)(1<<pch[i].pin);
           hetPORT2->PSL  &= ~(uint32)(1<<pch[i].pin);
           hetPORT2->PULDIS &= ~(uint32)(1<<pch[i].pin);
       }
   }
}
/******************
 * ������void GPO_Output_init(GIO_Info* pch)
 * ���ܣ�N2HET �� �����ʼ��
 * ���룺pch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 * �������
 * *******************/
void N2HET_As_Gio_Output_init(GIO_Info* pch)
{
    uint8 i=0;
   for(i=0;i<62;i++)
   {
       if(pch[i].port==N2HET1 &&(pch[i].pin<=31) )
       {
           hetPORT1->DIR  |=(uint32)(1<<pch[i].pin);      /** - Port A direction 0--INPUT  1--OUT*/
           hetPORT1->DOUT &=~(uint32)(1<<pch[i].pin);     /** - Port A output values */
           hetPORT1->PDR  |= (uint32)(1<<pch[i].pin);    /** - Port A open drain enable  0--push/pull  1--open drain*/
           hetPORT1->PSL  &= ~(uint32)(1<<pch[i].pin);    /** - Port A pullup / pulldown selection   0--down  1--up*/
           hetPORT1->PULDIS &= ~(uint32)(1<<pch[i].pin);  /** - Port A pullup / pulldown enable  0--ENABLE*/
       }
       else if(pch[i].port==N2HET2 &&( pch[i].pin<=31) )
       {
           hetPORT2->DIR  |=(uint32)(1<<pch[i].pin);
           hetPORT2->DOUT &=~(uint32)(1<<pch[i].pin);
           hetPORT2->PDR  |= (uint32)(1<<pch[i].pin);
           hetPORT2->PSL  &= ~(uint32)(1<<pch[i].pin);
           hetPORT2->PULDIS &= ~(uint32)(1<<pch[i].pin);
       }
   }
}
/******************
 * ������IO_STATE N2HET_As_Gio_get(GIO_Info input_ch)
 * ���ܣ���ȡN2HET�� ״̬
 * ���룺input_ch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 * �����I/O��״̬
 * *******************/
IO_STATE N2HET_As_Gio_get(GIO_Info input_ch)
{
    IO_STATE sate=IO_LOW;
    if(input_ch.port==N2HET1 &&( input_ch.pin<=7) )
    {
        sate= ((hetPORT1->DIN >> input_ch.pin) & 1U)>0? IO_HIG:IO_LOW;
    }
    else if(input_ch.port==N2HET2 &&( input_ch.pin<=7) )
    {
        sate= ((hetPORT2->DOUT >> input_ch.pin) & 1U)>0? IO_HIG:IO_LOW;
    }
    return sate;
}
/******************
 * ������void N2HET_As_Gio_set(GIO_Info output_ch, IO_STATE target)
 * ���ܣ�����N2HET�� ״̬
 * ���룺input_ch�����ò��������->port��GIO�ı�ţ���ѡ��GPIOA��GPIOB��
 *                    ->pin:���ź�
 *      target��I/O��״̬����ѡIO_LOW��IO_LOW=0
 * �������
 * *******************/
void N2HET_As_Gio_set(GIO_Info output_ch, IO_STATE target)
{
    if(output_ch.port==N2HET1 &&(output_ch.pin<=7) )
    {
        hetPORT1->DIN |= (target>> output_ch.pin);
    }
    else if(output_ch.port==N2HET2 &&( output_ch.pin<=7) )
    {
        hetPORT2->DOUT |= (target>> output_ch.pin);
    }
}
