#include "UART.h"


/*******************************************************************************
 * 名称: fputc
 * 功能: 重定向c库函数printf到UART1
 * 形参: 无
 * 返回: 要打印的字符
 * 说明: 由printf调用 
 ******************************************************************************/

int fputc(int ch, FILE *f)
{  
     // 将Printf内容发往串口 

     while(!UART2_SR_TXE);//等待发送空闲
     UART2_DR=ch;
	
     return (ch);
}


//初始化UART子函数
void UART2_Init(void)
{
  //串口引脚初始化
  //PD5:TX 推挽   
  PD_DDR|=0X20;
  PD_CR1|=0X20;
  PD_CR2|=0X20;
  
  //PD6:RX 不带中断的上拉输入
  PD_DDR&=0Xbf;
  PD_CR1|=0X40;
  PD_CR2&=0Xbf;
  
  CLK_CKDIVR=0X08;  //16M内部RC经2分频后系统时钟为8M
  UART2_BRR2=0X01;  //设置波特率9600
  UART2_BRR1=0x34;  //8M/9600 = 0x0341，必须先装入UART2_BRR2，后装入UART2_BRR1（中间两位，BRR1不能为0）
  UART2_CR1=0x00;   //8位数据
  UART2_CR3=0x00;   //1位停止位
  UART2_CR2=0x2c;   //b3=1,允许发送  b2=1,允许接受  
                    //b5=1,允许产生接收中断  
}





