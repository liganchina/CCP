//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : C50ES_FP
//* File Name          : ADC.C
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2011,05,26
//* Description        : ���ļ���������9S12XEP100��AD���ȡ��ֱ��ʡ�����ʱ�䡢����ͨ�����ж�ʹ�ܵȴ�
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "derivative.h" /* include peripheral declarations */
//**********************************************************************
//* Function name:   ADCInitial
//* Description:     ADCģ���ʼ����
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void ADCInitial(void) 
{ 
    ATD0CTL1=0x4f;//12λ����
    //ATD0CTL2=0x82; //ʹ��AD�����������־,��ʹ���ⲿ���� ,��˳��ת�������ж�
    ATD0CTL2=0x80;// ��ֹADC
  	ATD0CTL3=0xa0;//�Ҷ��룬
  	ATD0CTL4=0x0f; //����ʱ������Ϊ4��ʱ�����ڣ�16��Ƶ��������Ƶ��Ӧ����500k��2m֮��
  	ATD0CTL5=0xb0; //ɨ��ģʽ��ʹ��ͨ�� 0
  	
    //ATD0CTL2=0x82; //ʹ��AD�����������־,��ʹ���ⲿ���� ,��˳��ת�������ж�
    //ATD0CTL5=0xb0; 
}
//**********************************************************************
//* Function name:   turnOnADC
//* Description:     ����AD�ж�
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void turnOnADC(void)
{
    ATD0CTL2=0x82; //ʹ��AD�����������־,��ʹ���ⲿ���� ,��˳��ת�������ж�
    ATD0CTL5=0xb0; 
}
//**********************************************************************
//* Function name:   turnOffADC
//* Description:     �ر�AD�ж�
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void turnOffADC(void)
{
    ATD0CTL2=0x80;//
    ATD0CTL5=0xb0; 
}