/*描述:将AD采集到的电压通过串口调试助手打印出来*/

#include "iostm8s105s4.h"
#include "UART.h"

#define uchar unsigned char
#define uint unsigned int

uchar Watch_Flag=0;
uint Current_AD_Value;
float Current_AD_Conversion_Value;

void delay_ms(uint ms)  //1ms延时函数
{
  uint i,j;
  for(i=ms;i>0;i--)
    for(j=1000;j>0;j--);
}

void CLK_Init(void)
{
  CLK_CKDIVR=0x08;  //16M内部RC经2分频后系统时钟为8M
}


void ADC_Init(void)
{
  //ADC输入引脚：PE6 AIN9
  PE_DDR&=0XBF;    //浮空输入  pe6
  PE_CR1&=0XBF;    //都是pe6
  PE_CR2&=0XBF;

  ADC_CSR=0X19;    //ADC输入引脚AIN9，使能模拟看门狗中断  这里决定了单片机的引脚输入 接入到pe6上去
  ADC_CR1|=0x02;   //2分频4M，连续转换
  ADC_CR2=0x00;    // 数据左对齐  先读ADC_DRH,再读ADCZ_DRL  自配置
  ADC_TDRH=0x02;   //禁止施密特触发，降低功耗

  ADC_HTRH=921/256;//模拟看门狗电压上限4.5V
  ADC_HTRL=921%256;
  ADC_LTRH=0;      //模拟看门狗电压下限0.5V
  ADC_LTRL=102;

  ADC_CR1|=0X01;   //将单片机从低功耗中唤醒   引脚寄存器处理。
  delay_ms(1);    //等待ADC模块上电完成
  ADC_CR1|=0X01;  //开启AD转换，数据从pe6输入
}

float Get_AD_Value(void)
{
  uint AD_Value;
  while((ADC_CSR&0X80)==0);  //等待转换完成   引脚寄存器处理
  ADC_CSR&=0X7F;             //清除标志位EOC
  AD_Value=ADC_DRH<<2;
  AD_Value|=ADC_DRL;
  return ((float)(AD_Value*5)/1024);
}




void main()        //开始了主函数
{

  float AD_Conversion_Value;
  asm("rim");
  UART2_Init();
  CLK_Init();
  ADC_Init();
  while(1)
  {
    AD_Conversion_Value=Get_AD_Value();
    printf("\n\r当前的电压为:%5.4fV\n\r",AD_Conversion_Value);
    delay_ms(4000);

    if(Watch_Flag==1)
    {
      Watch_Flag=0;
      printf("\n\r当前的电压大于4.5V: %5.4fV\n\r",Current_AD_Conversion_Value);
      delay_ms(4000);
    }
    else if(Watch_Flag==2)
    {
      Watch_Flag=0;
      printf("\n\r当前的电压小于0.5V: %5.4fV\n\r",Current_AD_Conversion_Value);
      delay_ms(4000);
    }
    else
      ;;;
  }
}



#pragma vector=0x18

__interrupt void ADC2_IRQHandler(void)    //AD需要中断  接收中断
{

  if(ADC_CSR&0x40)
  {
     ADC_CSR&=0xbf;//清除标志位AWD
     Current_AD_Value=ADC_DRH<<2;
     Current_AD_Value|=ADC_DRL;
     Current_AD_Conversion_Value=(float)(Current_AD_Value*5)/1024;

     if(Current_AD_Conversion_Value>4.4)
       Watch_Flag=1;
     else if(Current_AD_Conversion_Value<0.4)
       Watch_Flag=2;
  }

}
