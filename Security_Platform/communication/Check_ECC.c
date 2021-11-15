/*
 * Check_ECC.c
 *
 *  Created on: 2021��10��29��
 *      Author: 15755
 */


#include "Hl_reg_l2ramw.h"
#include "Hl_reg_flash.h"
/******************
 * ������uint8_t RAM_ECC(void)
 * ���ܣ�RAM����ECC
 * ���룺��
 * ����������ECC�����0--��ECC������1--��ECC����
 * *******************/
uint8_t RAM_ECC(void)
{
    uint32_t ECC_Statues=0;
    /* 1-bit ECC error */

    l2ramwREG->DIAGDATAVECTOR_L = 0x00000000;
    l2ramwREG->DIAGDATAVECTOR_H = 0x00000000;
    l2ramwREG->DIAG_ECC = 0x0D;
    l2ramwREG->RAMTEST = (2 << 6) | (0xa << 0);
    l2ramwREG->RAMTEST = (1 << 8);
    ECC_Statues=l2ramwREG->RAMERRSTATUS & 0x00280000;

    if(ECC_Statues==0)
        return 0;
    else
        return 1;
}

/******************
 * ������uint8_t Flash_ECC(void)
 * ���ܣ�flash����ECC
 * ���룺��
 * ����������ECC�����0--��ECC������1--��ECC����
 * *******************/
uint8_t Flash_ECC(void)
{
    uint32_t flash_ecc_satues=0;
    flashWREG->FDIAGCTRL &=(~0x000f000f); /*DIAGMODE=0  DIAG_EN_KEY=0*/
    flashWREG->FDIAGCTRL |=0x00050005;   /*DIAGMODE=5  DIAG_EN_KEY=5*/
    flashWREG->FRAW_ADDR =0x00000000 | 0xA<<5; /*RAW_DATA=A*/
    flashWREG->FPRIM_ADD_TAG =0x00000000 | 0xA<<5; /*PRIM_ADD_TAG=A*/
    flashWREG->FDUP_ADD_TAG =0x00000000 ;  /* DUP_ADD_TAG=0*/
    flashWREG->FDIAGCTRL |=0x01000000; /*DIAG_TRIG=1*/
    flash_ecc_satues=(flashWREG->FEDAC_PASTATUS & 0x00000800) | (flashWREG->FEDAC_PBSTATUS & 0x00000800);
    if(flash_ecc_satues==0)
        return 0;
    else
        return 1;
}

