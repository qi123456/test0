/*����:��AD�ɼ����ĵ�ѹͨ�����ڵ������ִ�ӡ����*/

#include "iostm8s105s4.h"
#include "UART.h"

#define uchar unsigned char
#define uint unsigned int

uchar Watch_Flag=0;
uint Current_AD_Value;
float Current_AD_Conversion_Value;

void delay_ms(uint ms)  //1ms��ʱ����
{
  uint i,j;
  for(i=ms;i>0;i--)
    for(j=1000;j>0;j--);
}

void CLK_Init(void)
{
  CLK_CKDIVR=0x08;  //16M�ڲ�RC��2��Ƶ��ϵͳʱ��Ϊ8M
}


void ADC_Init(void)
{
  //ADC�������ţ�PE6 AIN9
  PE_DDR&=0XBF;    //��������  pe6
  PE_CR1&=0XBF;    //����pe6
  PE_CR2&=0XBF;

  ADC_CSR=0X19;    //ADC��������AIN9��ʹ��ģ�⿴�Ź��ж�  ��������˵�Ƭ������������ ���뵽pe6��ȥ
  ADC_CR1|=0x02;   //2��Ƶ4M������ת��
  ADC_CR2=0x00;    // ���������  �ȶ�ADC_DRH,�ٶ�ADCZ_DRL  ������
  ADC_TDRH=0x02;   //��ֹʩ���ش��������͹���

  ADC_HTRH=921/256;//ģ�⿴�Ź���ѹ����4.5V
  ADC_HTRL=921%256;
  ADC_LTRH=0;      //ģ�⿴�Ź���ѹ����0.5V
  ADC_LTRL=102;

  ADC_CR1|=0X01;   //����Ƭ���ӵ͹����л���   ���żĴ�������
  delay_ms(1);    //�ȴ�ADCģ���ϵ����
  ADC_CR1|=0X01;  //����ADת�������ݴ�pe6����
}

float Get_AD_Value(void)
{
  uint AD_Value;
  while((ADC_CSR&0X80)==0);  //�ȴ�ת�����   ���żĴ�������
  ADC_CSR&=0X7F;             //�����־λEOC
  AD_Value=ADC_DRH<<2;
  AD_Value|=ADC_DRL;
  return ((float)(AD_Value*5)/1024);
}




void main()        //��ʼ��������
{

  float AD_Conversion_Value;
  asm("rim");
  UART2_Init();
  CLK_Init();
  ADC_Init();
  while(1)
  {
    AD_Conversion_Value=Get_AD_Value();
    printf("\n\r��ǰ�ĵ�ѹΪ:%5.4fV\n\r",AD_Conversion_Value);
    delay_ms(4000);

    if(Watch_Flag==1)
    {
      Watch_Flag=0;
      printf("\n\r��ǰ�ĵ�ѹ����4.5V: %5.4fV\n\r",Current_AD_Conversion_Value);
      delay_ms(4000);
    }
    else if(Watch_Flag==2)
    {
      Watch_Flag=0;
      printf("\n\r��ǰ�ĵ�ѹС��0.5V: %5.4fV\n\r",Current_AD_Conversion_Value);
      delay_ms(4000);
    }
    else
      ;;;
  }
}



#pragma vector=0x18

__interrupt void ADC2_IRQHandler(void)    //AD��Ҫ�ж�  �����ж�
{

  if(ADC_CSR&0x40)
  {
     ADC_CSR&=0xbf;//�����־λAWD
     Current_AD_Value=ADC_DRH<<2;
     Current_AD_Value|=ADC_DRL;
     Current_AD_Conversion_Value=(float)(Current_AD_Value*5)/1024;

     if(Current_AD_Conversion_Value>4.4)
       Watch_Flag=1;
     else if(Current_AD_Conversion_Value<0.4)
       Watch_Flag=2;
  }

}
