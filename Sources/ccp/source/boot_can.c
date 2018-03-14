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
#include "mscan.h"
#include <mc9s12xep100.h>
#include <hidef.h>
//#include "derivative.h" /* include peripheral declarations */

int ccpBootTransmitCrm (unsigned char *array_msg)
{
    struct can_msg mg;
    unsigned char tt=100;
    unsigned char i = 0;
	
	if(mg.len > 8){
		return(FALSE);
	}

	// �������ʱ��
	if(CAN0CTL0_SYNCH==0){
		return(FALSE);
	}
	
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    mg.id= CCP_DTO_ID;

	for(i=0;i<8;i++){
		mg.data[i] = *(array_msg+i);
	}

    while((!MSCAN0SendMsg(mg))&&(tt>0)){
        tt--;
    }
	
	return TRUE;
}

int ccpBootReceiveCro (unsigned char *array_msg)
{
    struct can_msg msg;
	unsigned char i = 0;
	unsigned long pp=0;
	unsigned long pp1=0;
	
	// �����ձ�־
	if(!(CAN0RFLG_RXF)){
		return(FALSE);
	}
	
	// ��� CANЭ�鱨��ģʽ ��һ��/��չ�� ��ʶ��
	if(CAN0RXIDR1_IDE){
		// IDE = Recessive (Extended Mode)
		return(FALSE);
	}
	
	pp = (unsigned long)CAN0RXIDR0<<15;  
	pp1 = (unsigned long)((CAN0RXIDR1>>5)&0x07)<<15;							  
	
	msg.id = ((unsigned long)pp<<6) | //expanded
			   (pp1<<3) |
			   ((unsigned long)(CAN0RXIDR1&0x07)<<15)|
			   ((unsigned long)CAN0RXIDR2<<7) | 
			   ((unsigned long)CAN0RXIDR3>>1);
	
	if (msg.id != CCP_DTO_ID){ 
		return(FALSE);	// if received not correctly, return 0
	}
	
	if(CAN0RXIDR1&0x10){
		msg.RTR = TRUE;
	}
	else{
		msg.RTR = FALSE;
	}
	
	// ��ȡ���ݳ��� 
	msg.len = CAN0RXDLR;

    for(i=0;i<8;i++){ 
        *(array_msg+i) = *((&CAN0RXDSR0)+i);
    }
	
	// �� RXF ��־λ (������׼������)
	CAN0RFLG = 0x01;
	
	return TRUE;	// if received correctly, return 1
}

/*************************************************************/
/*                       CAN0����                            */
/*************************************************************/
//int ccpBootTransmitCrm (unsigned char *array_msg)
//{
//  msg.id = g_ccp_dto_id;

//  if(msg.len > 8)
//    return(FALSE);

//  // �������ʱ��
//  if(CAN0CTL0_SYNCH==0)
//    return(FALSE);

//  send_buf = 0;
//  do
//  {
//    // Ѱ�ҿ��еĻ�����
//    CAN0TBSEL=CAN0TFLG;
//    send_buf=CAN0TBSEL;
//  }
//  
//  while(!send_buf); 
// 
//  // д���ʶ��
//  CAN0TXIDR0 = (unsigned char)(msg.id>>3);
//  CAN0TXIDR1 = (unsigned char)(msg.id<<5);
//  
//    if(msg.RTR)
//    // RTR = ����
//    CAN0TXIDR1 |= 0x10;
//    
//  // д������
//  CAN0TXDSR0 = *array_msg;
//  CAN0TXDSR1 = *(array_msg+1);
//  CAN0TXDSR2 = *(array_msg+2);
//  CAN0TXDSR3 = *(array_msg+3);
//  CAN0TXDSR4 = *(array_msg+4);
//  CAN0TXDSR5 = *(array_msg+5);
//  CAN0TXDSR6 = *(array_msg+6);
//  CAN0TXDSR7 = *(array_msg+7);
//     
//  // д�����ݳ���
//  CAN0TXDLR = msg.len;

//  
//  // д�����ȼ�
//  CAN0TXTBPR = msg.prty;
//  
//  // �� TXx ��־ (������׼������)
//  CAN0TFLG = send_buf;
//  
//  return(TRUE);  
//}

/*************************************************************/
/*                       CAN0����                            */
/*************************************************************/
//int ccpBootReceiveCro (unsigned char *array_msg)
//{

//  // �����ձ�־
//  if(!(CAN0RFLG_RXF))
//    return(FALSE);
//  
//  // ��� CANЭ�鱨��ģʽ ��һ��/��չ�� ��ʶ��
//  if(CAN0RXIDR1_IDE)
//    // IDE = Recessive (Extended Mode)
//    return(FALSE);

//  // ����ʶ��
//  msg.id = (unsigned int)(CAN0RXIDR0<<3) | 
//            (unsigned char)(CAN0RXIDR1>>5);
//  
//  if(CAN0RXIDR1&0x10)
//    msg.RTR = TRUE;
//  else
//    msg.RTR = FALSE;
// 
//  // ��ȡ���ݳ��� 
//  msg.len = CAN0RXDLR;
//  
// *array_msg = CAN0RXDSR0;
// *(array_msg+1) = CAN0RXDSR1;
// *(array_msg+2) = CAN0RXDSR2;
// *(array_msg+3) = CAN0RXDSR3;
// *(array_msg+4) = CAN0RXDSR4;
// *(array_msg+5) = CAN0RXDSR5;
// *(array_msg+6) = CAN0RXDSR6;
// *(array_msg+7) = CAN0RXDSR7;
//   

//  // �� RXF ��־λ (������׼������)
//  CAN0RFLG = 0x01;
//  if (msg.id == g_ccp_cro_id) 
//    return(TRUE);            // if correctly received, return 1
//    //return(TRUE);     
//}


