#include "def.h"
#include "lcdlib.h"
#include <string.h>
#include "2410addr.h"
#include "2410lib.h"
#include "PIC1.h"
#include "PIC2.h"
int const Time_1s= 1000;
int const Time_half_s= 100;
void Lcd2410_Init_On(void);
extern U16 (*frameBuffer16BitTft800480)[SCR_XSIZE_TFT_800480];


short int flag;
int volatile Time1ms;
char pre_key;
int Time_count;
int change_flag;

void show1();
void show2();

//��ʼ����ʱ����ص�
void var_Init()
{
	 flag=1;//�л�ͼƬ��־
     Time1ms = 0 ;//��ʱ����ʱ
	 pre_key='1';
	 Time_count=Time_1s;//��ʱ����ʱ����1000
	 change_flag=0;
}


void lcdTest()
{
 	U16 col = 0x1234;
 
	Lcd2410_Init_On(); 	//LCD��ʼ����ں���
	show1();
	while(1)
	{
		if(Time1ms>=Time_count)//��ʱ����ʱ
		{
			//Timer4 start/stop,[20]��������ʱ��4�Ŀ��أ�0=�أ�1=��
			rTCON &= ~(1 << 20) ; //Timer4 end
			
			Time1ms=0;//��ʱ����ʱ����
			
			if(flag==1)//�л�ͼƬ
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
			//��ͼƬ�е����ص�ֵ����֡�ڴ�
			frameBuffer16BitTft800480[i][j] = FF1[i][j];
		}
	}
	//++flag;//�л�ͼƬ
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
	//--flag;//�л�ͼƬ
	rTCON |= (1 << 20) ; //Timer4 start
}


//ʱ���жϷ������
//ÿ1ms����һ�Σ�Time1ms�����Լ�1
void __irq Timer4Int()
{
    rSRCPND = BIT_TIMER4; //����ж�Դ����Ĵ���
    rINTPND = BIT_TIMER4; //����жϷ���Ĵ���
    Time1ms ++ ;
	
}


//�����жϷ�����
//�������ݵ���ʱ����
void __irq RxInt()
{
 	char r;
    rSUBSRCPND=BIT_SUB_RXD0;//0x4a000018 ���RXD1,ERR1�жϹ����־λ
 	rSRCPND=BIT_UART0;  //0x4a000000
    rINTPND=BIT_UART0;	//)0x4a000010
	r = Uart_GetKey();
	if(r!='1'&&r!='0')
	{
		Uart_SendByte(r);
		//change_flag=0;
		return;
	}
	else if(r!=pre_key)
	{
		if(pre_key=='1')
		{
			Time_count=Time_half_s;
			pre_key=r;
		}
		else if(pre_key=='0')
		{
			Time_count=Time_1s;
			pre_key=r;
		}
		
		
	}
	else
		// change_flag=0;
	
	Uart_SendByte(r);
}






