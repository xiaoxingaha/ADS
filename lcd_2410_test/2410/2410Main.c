
#include "def.h"
#include "option.h"
#include "2410addr.h"
#include "2410lib.h"
#include "2410slib.h"
#include "locale.h"

void Isr_Init(void);
void Timer4_Init();//��ʱ��4��ʼ��
void HaltUndef(void);
void HaltSwi(void);
void HaltPabort(void);
void HaltDabort(void);

unsigned __rt_heap_extend(unsigned size, void ** block)
{
	return 0;
}
void RxInt();//�����жϷ�������ʼ��
void Timer4Int();//��ʱ��4�жϷ�������ʼ��

void Main(void)
{
	//Ӳ����ʼ��
	var_Init();//��ʼ����ʱ����صĲ���
	Port_Init();//IO�˿ڳ�ʼ��
	Isr_Init();//�жϳ�ʼ��
	Uart_Init(0,115200);//���ڳ�ʼ��
	Uart_Select(0);
	_init_alloc(0x32500000,0x333fffff);//��ʼ���ѵ�ַ��15M��ʹ��malloc�ȴ洢���亯��
	setlocale(LC_ALL,"C");//ʹ�ܱ��غ�������sprintf��
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
	//���ô���
	pISR_UART0=(unsigned)RxInt;            //pISR_FIQ,pISR_IRQ must be initialized

    rSRCPND = BIT_UART0; //����ж�Դ����Ĵ���
    rSUBSRCPND = BIT_SUB_RXD0;      
    
    rINTPND = BIT_UART0; //����жϷ���Ĵ���
          
    rINTMOD &= ~(BIT_UART0); //�����ж�ģʽIRQ 
       
    rINTSUBMSK &=~((BIT_SUB_RXD0));   

    
    rINTMSK &= ~(BIT_UART0); //����uart�ж�   
    //���ö�ʱ��4�ж�	
    pISR_TIMER4 =(unsigned)Timer4Int ;
    rSRCPND = BIT_TIMER4; //����ж�Դ����Ĵ���
    rINTPND = BIT_TIMER4; //����жϷ���Ĵ���
          
    rINTMOD &= ~(BIT_TIMER4); //�����ж�ģʽIRQ 

    rINTMSK &= ~(BIT_TIMER4); //����Timer4�ж�   

}

//===================================================================
void Timer4_Init()   
{
	//��1����Ƶ���ķ�Ƶϵ����PCLK/(prescaler value +1)
	//��ʱ��4��Ӧ����Ԥ��Ƶ��1����[15:8]λ������Ԥ��Ƶ����ֵ
	rTCFG0 = (49 << 8) ; //Timer4 prescaler = 50 ; Tclk = 1M 
	
	//��2����Ƶ���ķ�Ƶϵ����PCLK/(prescaler value +1)/(divider value)
	//��ʱ��4��Ӧ����MUX4����[19:16]λ��0000=1/2,0001=1/4,0010=1/8,0011=1/16
	rTCFG1 |= (3 << 16); //divider = 16
	
	//��ʱ�����ƼĴ���
	//[22]����ʱ��4�Զ���װ���أ�0=������1=��Ъģʽ(�Զ���װ)
	//[21]����ʱ��4���ֶ����£�0=�޲�����1=����TCNTB4�Ĵ���
	//��ע���ֶ�����λ���´�д��ʱ����뱻���
	rTCON |= (3 << 21) ; //Timer4 atuo reload,Manul update time4
	
	//��ʱ��4��������Ĵ���
	rTCNTB4 = 30;		 //1ms��ʱ
	//rTCNTB4 = 30*1000;   //1s
	
	//2410addr.h��û�У��Լ���ӵģ����Ǽ�����������Ժ�ͼƬ�Ͳ����Զ��л�
	//rTCMPB4 = 0;
	
	//[21]����ʱ��4���ֶ����£�0=�޲�����1=����TCNTB4�Ĵ���
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

