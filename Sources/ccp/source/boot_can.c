/*******************************************************************************

  File Name   : boot_can.c
  Date        : 14.08.2001
  Version     : 1.0
  Desciption  : free CAN driver for CCP without using interrupts

*******************************************************************************/


// -----------------------------------------------------------------------------
// INCLUDE FILES
// -----------------------------------------------------------------------------
//#include <iod60.h>               // standard HC12 IO
#include "ccppar.h"              // CPP config file
#include "boot_can.h"            // CAN driver

#include <mc9s12xep100.h>
#include <hidef.h>
//#include "derivative.h" /* include peripheral declarations */

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// VARIABLES and Structs
// -----------------------------------------------------------------------------
unsigned int g_ccp_dto_id;              // global DTO-ID
unsigned int g_ccp_cro_id;              // global CRO-ID
unsigned char send_buf = 0;



struct can_msg     //���ͱ��ĵĽṹ��
{
    unsigned int id;
    Bool RTR;
    unsigned char tx_data[8];
    unsigned char rx_data[8];
    unsigned char len;
    unsigned char prty;
};

struct can_msg msg; 
/*************************************************************/
/*                        ��ʼ��CAN0                         */
/*************************************************************/
void ccpBootInit (int cro_id, int dto_id)
{
  g_ccp_dto_id = dto_id;       // handover IDs (CRO, DTO)
  g_ccp_cro_id = cro_id;
   
  if(CAN0CTL0_INITRQ==0)       // ��ѯ�Ƿ�����ʼ��״̬   
    CAN0CTL0_INITRQ =1;        // �����ʼ��״̬

  while (CAN0CTL1_INITAK==0);  //�ȴ������ʼ��״̬

  CAN0BTR0_SJW = 0;            //����ͬ��
  CAN0BTR0_BRP = 7;            //���ò�����  
  CAN0BTR1 = 0x1c;             //����ʱ��1��ʱ��2��Tq���� ,����Ƶ��Ϊ250kb/s

// �ر��˲���                                  
  CAN0IDMR0 = 0xFF;
  CAN0IDMR1 = 0xFF;
  CAN0IDMR2 = 0xFF;
  CAN0IDMR3 = 0xFF;
  CAN0IDMR4 = 0xFF;
  CAN0IDMR5 = 0xFF;
  CAN0IDMR6 = 0xFF;
  CAN0IDMR7 = 0xFF; 

  CAN0CTL1 = 0xC0;             //ʹ��MSCANģ��,����Ϊһ������ģʽ��ʹ������ʱ��Դ 

  CAN0CTL0 = 0x00;             //����һ��ģʽ����

  while(CAN0CTL1_INITAK);      //�ȴ��ص�һ������ģʽ

  while(CAN0CTL0_SYNCH==0);    //�ȴ�����ʱ��ͬ��

  CAN0RIER_RXFIE = 0;          //��ֹ�����ж�    
}
int ccpBootTransmitCrmPossible( void ) {
//  return ((msCAN12.CTFLG & TXE0) == 1);        // return 1 if so

  do
  {
    // Ѱ�ҿ��еĻ�����
    CAN0TBSEL=CAN0TFLG;
    send_buf=CAN0TBSEL;
  }  
  while(!send_buf);
  return(TRUE);
}

/*************************************************************/
/*                       CAN0����                            */
/*************************************************************/
int ccpBootTransmitCrm (unsigned char *array_msg)
{
  msg.id = g_ccp_dto_id;

  if(msg.len > 8)
    return(FALSE);

  // �������ʱ��
  if(CAN0CTL0_SYNCH==0)
    return(FALSE);

  send_buf = 0;
  do
  {
    // Ѱ�ҿ��еĻ�����
    CAN0TBSEL=CAN0TFLG;
    send_buf=CAN0TBSEL;
  }
  
  while(!send_buf); 
 
  // д���ʶ��
  CAN0TXIDR0 = (unsigned char)(msg.id>>3);
  CAN0TXIDR1 = (unsigned char)(msg.id<<5);
  
    if(msg.RTR)
    // RTR = ����
    CAN0TXIDR1 |= 0x10;
    
  // д������
  CAN0TXDSR0 = *array_msg;
  CAN0TXDSR1 = *(array_msg+1);
  CAN0TXDSR2 = *(array_msg+2);
  CAN0TXDSR3 = *(array_msg+3);
  CAN0TXDSR4 = *(array_msg+4);
  CAN0TXDSR5 = *(array_msg+5);
  CAN0TXDSR6 = *(array_msg+6);
  CAN0TXDSR7 = *(array_msg+7);
     
  // д�����ݳ���
  CAN0TXDLR = msg.len;

  
  // д�����ȼ�
  CAN0TXTBPR = msg.prty;
  
  // �� TXx ��־ (������׼������)
  CAN0TFLG = send_buf;
  
  return(TRUE);  
}
/*************************************************************/
/*                       CAN0����                            */
/*************************************************************/
int ccpBootReceiveCro (unsigned char *array_msg)
{

  // �����ձ�־
  if(!(CAN0RFLG_RXF))
    return(FALSE);
  
  // ��� CANЭ�鱨��ģʽ ��һ��/��չ�� ��ʶ��
  if(CAN0RXIDR1_IDE)
    // IDE = Recessive (Extended Mode)
    return(FALSE);

  // ����ʶ��
  msg.id = (unsigned int)(CAN0RXIDR0<<3) | 
            (unsigned char)(CAN0RXIDR1>>5);
  
  if(CAN0RXIDR1&0x10)
    msg.RTR = TRUE;
  else
    msg.RTR = FALSE;
 
  // ��ȡ���ݳ��� 
  msg.len = CAN0RXDLR;
  
 *array_msg = CAN0RXDSR0;
 *(array_msg+1) = CAN0RXDSR1;
 *(array_msg+2) = CAN0RXDSR2;
 *(array_msg+3) = CAN0RXDSR3;
 *(array_msg+4) = CAN0RXDSR4;
 *(array_msg+5) = CAN0RXDSR5;
 *(array_msg+6) = CAN0RXDSR6;
 *(array_msg+7) = CAN0RXDSR7;
   

  // �� RXF ��־λ (������׼������)
  CAN0RFLG = 0x01;
  if (msg.id == g_ccp_cro_id) 
    return(TRUE);            // if correctly received, return 1
    //return(TRUE);     
}


