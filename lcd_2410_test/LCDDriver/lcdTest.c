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


//��ʼ����ʱ����ص�
void var_Init()
{
	 flag=1;//�л�ͼƬ��־
     Time1ms = 0 ;//��ʱ����ʱ
	 pre_key='1';//��һ�������ַ�
	 Time_count=Time_1s;//��ʱ����ʱ����1000
	 change_flag=0;
}


void lcdTest()
{
 	U16 col = 0x1234;
 
	Lcd2410_Init_On();//LCD��ʼ����ں���
	Lcd_ClearScr(0xFFFF);//����
	while(1)
	{
		Lcd_Print_ZW(100,60,(54<<8)+48,0);
		Lcd_Print_ZW(100,80,(27<<8)+10,0);
	}
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


/*********************************************************
TFT LCD�������ص���ʾ�������
*********************************************************/
void PutPixel(U32 x,U32 y, U16 c )
{
	if ( (x < 800) && (y < 480) )
		frameBuffer16BitTft800480[(y)][(x)] = c;
}


/**********************************************************
TFT LCDȫ������ض���ɫ��Ԫ������
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
TFT LCD��ʾ����
**********************************************************/
void Lcd_Print_ZW(U32 x,U32 y,U16 QW,U32 c)
{
       U16 i,j;
       U8 *pZK,mask,buf;

       pZK = &__CHS[ (  ( (QW >> 8) - 1 )*94 + (QW & 0x00FF)- 1 )*32 ];
       for( i = 0 ; i < 16 ; i++ )
       {
              //��
              mask = 0x80;
              buf = pZK[i*2];
              for( j = 0 ; j < 8 ; j++ )
              {
                     if( buf & mask )
                     {
                            PutPixel(x+j,y+i,c);
                     }
                     mask = mask >> 1;
              } 
        
              //��
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
	
	//Uart_Getch()��Uart_GetKey()������
	//ǰ��Uart_Getch()�ǵȴ��û��ڳ����ն��������ݺ�ż����������У�����һֱѭ���ȴ���ֱ������������
	//������Uart_GetKey()���������û����������򷵻���������ݣ���û���򷵻�0�����ȴ�
	r = Uart_GetKey();
	if(r!='1'&&r!='0')
	{
		//�������r�����͵����ͻ���Ĵ�����Ȼ����ʾ�ڳ����ն�
		Uart_SendByte(r);
		Uart_Printf("\n����ģʽ��Ч��\n");
		//change_flag=0;
		return;
	}
	//��ͬ��ģʽ�Ͳ������л�
	else if(r!=pre_key)
	{
		if(pre_key=='1')
		{
			Time_count=Time_half_s;
			Uart_SendByte(r);
			Uart_Printf("\n����10s�л�һ��ͼƬģʽ��\n");
			pre_key=r;
		}
		else if(pre_key=='0')
		{
			Time_count=Time_1s;
			Uart_SendByte(r);
			Uart_Printf("\n����1s�л�һ��ͼƬģʽ��\n");
			pre_key=r;
		}
		
		
	}
	else
	{
		// change_flag=0;
		Uart_SendByte(r);
		Uart_Printf("\n�л�ͼƬģʽ���䡣\n");
	}
}






