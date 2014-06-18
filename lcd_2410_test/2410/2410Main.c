
#include "def.h"
#include "option.h"
#include "2410addr.h"
#include "2410lib.h"
#include "2410slib.h"
#include "locale.h"

void Isr_Init(void);
void Timer4_Init();//定时器4初始化
void HaltUndef(void);
void HaltSwi(void);
void HaltPabort(void);
void HaltDabort(void);

unsigned __rt_heap_extend(unsigned size, void ** block)
{
	return 0;
}
void RxInt();//串口中断服务函数初始化
void Timer4Int();//定时器4中断服务函数初始化

void Main(void)
{
	//硬件初始化
	var_Init();//初始化定时器相关的参数
	Port_Init();//IO端口初始化
	Isr_Init();//中断初始化
	Uart_Init(0,115200);//串口初始化
	Uart_Select(0);
	_init_alloc(0x32500000,0x333fffff);//初始化堆地址：15M，使能malloc等存储分配函数
	setlocale(LC_ALL,"C");//使能本地函数，如sprintf等
	Timer4_Init();
	lcdTest();
}	

//===================================================================
void Isr_Init(void)
{
    pISR_UNDEF  = (unsigned)HaltUndef;
    pISR_SWI    = (unsigned)HaltSwi;
    pISR_PABORT = (unsigned)HaltPabort;
    pISR_DABORT = (unsigned)HaltDabort;   
    rINTMOD     = 0x0;                     //All=IRQ mode  
    rINTMSK     = BIT_ALLMSK;              //All interrupt is masked.
    rINTSUBMSK  = BIT_SUB_ALLMSK;          //All sub-interrupt is masked. <- April 01, 2002 SOP
	//配置串口
	pISR_UART0=(unsigned)RxInt;            //pISR_FIQ,pISR_IRQ must be initialized

    rSRCPND = BIT_UART0; //清除中断源挂起寄存器
    rSUBSRCPND = BIT_SUB_RXD0;      
    
    rINTPND = BIT_UART0; //清除中断服务寄存器
          
    rINTMOD &= ~(BIT_UART0); //设置中断模式IRQ 
       
    rINTSUBMSK &=~((BIT_SUB_RXD0));   

    
    rINTMSK &= ~(BIT_UART0); //允许uart中断   
    //配置定时器4中断	
    pISR_TIMER4 =(unsigned)Timer4Int ;
    rSRCPND = BIT_TIMER4; //清除中断源挂起寄存器
    rINTPND = BIT_TIMER4; //清除中断服务寄存器
          
    rINTMOD &= ~(BIT_TIMER4); //设置中断模式IRQ 

    rINTMSK &= ~(BIT_TIMER4); //允许Timer4中断   

}

//===================================================================
void Timer4_Init()   
{
	//第1级分频器的分频系数：PCLK/(prescaler value +1)
	//定时器4对应的是预分频器1，即[15:8]位定义了预分频器的值
	rTCFG0 = (49 << 8) ; //Timer4 prescaler = 50 ; Tclk = 1M 
	
	//第2级分频器的分频系数：PCLK/(prescaler value +1)/(divider value)
	//定时器4对应的是MUX4，即[19:16]位，0000=1/2,0001=1/4,0010=1/8,0011=1/16
	rTCFG1 |= (3 << 16); //divider = 16
	
	//定时器控制寄存器
	//[22]，定时器4自动重装开关，0=单发，1=间歇模式(自动重装)
	//[21]，定时器4的手动更新，0=无操作，1=更新TCNTB4寄存器
	//【注】手动更新位在下次写的时候必须被清除
	rTCON |= (3 << 21) ; //Timer4 atuo reload,Manul update time4
	
	//定时器4计数缓冲寄存器
	rTCNTB4 = 30;		 //1ms定时
	//rTCNTB4 = 30*1000;   //1s
	
	//2410addr.h里没有，自己添加的，但是加了下面这句以后图片就不会自动切换
	//rTCMPB4 = 0;
	
	//[21]，定时器4的手动更新，0=无操作，1=更新TCNTB4寄存器
	rTCON &= ~(1 << 21);
	//rTCON |= (1 << 20) ; //Timer4 start
	
}

//===================================================================
void HaltUndef(void)
{
    while(1);
}

//===================================================================
void HaltSwi(void)
{
    while(1);
}

//===================================================================
void HaltPabort(void)
{
    while(1);
}

//===================================================================
void HaltDabort(void)
{
    while(1);
}

//===================================================================

