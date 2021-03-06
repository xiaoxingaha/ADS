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
extern unsigned char __CHS[];


short int flag;
int volatile Time1ms;
char pre_key;
int Time_count;
int change_flag;

void show1();
void show2();
void PutPixel(U32 x,U32 y, U16 c );
void Lcd_ClearScr(U32 c);
void Lcd_Print_ZW(U32 x,U32 y,U16 QW,U32 c);


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
 
	Lcd2410_Init_On();//LCD初始化入口函数
	Lcd_ClearScr(0xFFFF);//清屏
	while(1)
	{
		Lcd_Print_ZW(100,60,(54<<8)+48,0);
		Lcd_Print_ZW(100,80,(27<<8)+10,0);
	}
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


/*********************************************************

功能说明：TFT LCD单个像素的显示数据输出

参数说明：x,y分别为显示的横纵坐标，c为RGB颜色值

*********************************************************/
void PutPixel(U32 x,U32 y, U16 c )
{
	if ( (x < 800) && (y < 480) )
		frameBuffer16BitTft800480[(y)][(x)] = c;
}


/**********************************************************

功能说明：TFT LCD全屏填充特定颜色单元或清屏

参数说明：c为RGB颜色值

补充：清屏就是往 buffer中写入0xffff

**********************************************************/
void Lcd_ClearScr(U32 c)
{
	U32 x,y ;
		
    for( y = 0 ; y < 480 ; y++ )
    {
    	for( x = 0 ; x < 800 ; x++ )
    	{
			frameBuffer16BitTft800480[y][x] = c ;
    	}
    }
}


/**********************************************************

功能说明：TFT LCD显示汉字，基于16*16点阵模式的字库

参数说明：x,y分别为显示的横纵坐标，QW为16位的区位码，c为RGB颜色值

**********************************************************/
void Lcd_Print_ZW(U32 x,U32 y,U16 QW,U32 c)
{
	U16 i,j;
	//U8为char类型，一个字节为单位
	//pZK 存放汉字点阵首地址
	//mask 用于标志1个字符8位中具体某位的值
	//buf 存放当前字符的地址
    U8 *pZK,mask,buf;

	//点阵起始位置=((区码-1)*94+(位码-1))*汉子点阵字节数
	//这里采用的是16*16点阵模式的字库，也就是一个中文由16*16个像素点组成，32B
    //所以，下面这行是根据区位码查找所要显示汉字的点阵数据的首地址
    pZK = &__CHS[ (  ( (QW >> 8) - 1 )*94 + (QW & 0x00FF)- 1 )*32 ];
    
    //按奇偶字节显示，将汉字点阵数据和LCD屏幕显示像素挂钩
    for( i = 0 ; i < 16 ; i++ )
    {
		//偶数字节
		//从做高位到最低位依次判断
		mask = 0x80;
		buf = pZK[i*2];
		//一个字符一共八位，循环7次
		for( j = 0 ; j < 8 ; j++ )
		{
			//若该字节中当前位为1，则在屏幕像素点中显示
			if( buf & mask )
			{
				PutPixel(x+j,y+i,c);
			}
			//分别判断每一位的值
			mask = mask >> 1;
		} 

		//奇数字节
		mask = 0x80;
		buf = pZK[i*2 + 1];
		for( j = 0 ; j < 8 ; j++ )
		{
			if( buf & mask )
			{
				PutPixel(x+j + 8,y+i,c);
			}
			mask = mask >> 1;
		}                 
	}
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






