#include "def.h"
#include "lcdlib.h"
#include <string.h>
#include "2410addr.h"
#include "2410lib.h"
#include "PIC1.h"
#include "PIC2.h"
int const Time_1s= 1000;
int const Time_half_s= 10000;
void Lcd2410_Init_On(void);
extern U16 (*frameBuffer16BitTft800480)[SCR_XSIZE_TFT_800480];


short int flag;
int volatile Time1ms;
char pre_key;
int Time_count;
int change_flag;

void show1();
void show2();

//初始化定时器相关的
void var_Init()
{
	 flag=1;//切换图片标志
     Time1ms = 0 ;//定时器计时
	 pre_key='1';//上一个输入字符
	 Time_count=Time_1s;//定时器计时上限1000
	 change_flag=0;
}


void lcdTest()
{
 	U16 col = 0x1234;
 
	Lcd2410_Init_On(); 	//LCD初始化入口函数
	show1();
	while(1)
	{
		if(Time1ms>=Time_count)//定时器超时
		{
			//Timer4 start/stop,[20]，决定定时器4的开关，0=关，1=开
			rTCON &= ~(1 << 20) ; //Timer4 end
			
			Time1ms=0;//定时器计时清零
			
			if(flag==1)//切换图片
			{
				show1();
				flag = 2;
			}
			else
			{
				show2();
				flag = 1;
			}	
		}
	}
}


void show1()
{
	int i,j;
	for(i = 0;i < 480;i++)
	{
		for(j = 0;j < 800;j++)
		{
			//将图片中的像素点值赋给帧内存
			frameBuffer16BitTft800480[i][j] = FF1[i][j];
		}
	}
	//++flag;//切换图片
	rTCON |= (1 << 20) ; //Timer4 start
}


void show2()
{
	int i,j;
	for(i = 0;i < 480;i++)
	{
		for(j = 0;j < 800;j++)
		{
			frameBuffer16BitTft800480[i][j] = FF2[i][j];
		}
	}
	//--flag;//切换图片
	rTCON |= (1 << 20) ; //Timer4 start
}


//时钟中断服务程序
//每1ms进来一次，Time1ms变量自加1
void __irq Timer4Int()
{
    rSRCPND = BIT_TIMER4; //清除中断源挂起寄存器
    rINTPND = BIT_TIMER4; //清除中断服务寄存器
    Time1ms ++ ;
	
}


//串口中断服务函数
//当有数据到来时触发
void __irq RxInt()
{
 	char r;
    rSUBSRCPND=BIT_SUB_RXD0;//0x4a000018 清除RXD1,ERR1中断挂起标志位
 	rSRCPND=BIT_UART0;  //0x4a000000
    rINTPND=BIT_UART0;	//)0x4a000010
	
	//Uart_Getch()和Uart_GetKey()的区别：
	//前者Uart_Getch()是等待用户在超级终端输入数据后才继续往下运行，否则一直循环等待，直到有数据输入
	//而后者Uart_GetKey()则是若有用户输入数据则返回输入的数据，若没有则返回0，不等待
	r = Uart_GetKey();
	if(r!='1'&&r!='0')
	{
		//将输入的r，发送到发送缓存寄存器，然后显示在超级终端
		Uart_SendByte(r);
		Uart_Printf("\n输入模式无效。\n");
		//change_flag=0;
		return;
	}
	//相同的模式就不进行切换
	else if(r!=pre_key)
	{
		if(pre_key=='1')
		{
			Time_count=Time_half_s;
			Uart_SendByte(r);
			Uart_Printf("\n进入10s切换一张图片模式。\n");
			pre_key=r;
		}
		else if(pre_key=='0')
		{
			Time_count=Time_1s;
			Uart_SendByte(r);
			Uart_Printf("\n进入1s切换一张图片模式。\n");
			pre_key=r;
		}
		
		
	}
	else
	{
		// change_flag=0;
		Uart_SendByte(r);
		Uart_Printf("\n切换图片模式不变。\n");
	}
}






