/*****************************************
  NAME: lcdlib.c
  DESC: lcd configuration low-level codes
  HISTORY:
  2002.03.13:draft ver 0.0
 *****************************************/
#include <string.h>
#include "def.h"
#include "2410addr.h"
#include "2410lib.h"
#include "2410slib.h"

#include "lcdlib.h"

//��ά�����ָ�룬ָ����ͼƬ�Ķ�ά����
U16 (*frameBuffer16BitTft800480)[SCR_XSIZE_TFT_800480];

void Lcd2410_Init();
void Lcd2410_PowerEnable(int invpwren,int pwren);
void Lcd2410_EnvidOnOff(int onoff);

//LCD��ʼ����ں���
void Lcd2410_Init_On(void)
{
	Lcd2410_Port_Init();//LCD���ݺͿ��ƶ˿ڳ�ʼ��
	Lcd2410_Init();	//LCD����ģ���ʼ��
	Lcd2410_PowerEnable(0, 0); //LCD��Դ��������ʹ�� //0-������ƣ�1-��
    Lcd2410_EnvidOnOff(1);	//LCD��Ƶ�Ϳ����ź��������ֹͣ��1������Ƶ���
}


/**************************************************************
800��480 16Bpp TFT LCD���ݺͿ��ƶ˿ڳ�ʼ��
**************************************************************/
void Lcd2410_Port_Init(void)
{

	//Port C
	//��������������
    rGPCUP=0xffffffff; // Disable Pull-up register
    //IO�˿�ȫ������ΪLCD��صĹ���
    rGPCCON=0xaaaaaaaa; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
    
	//Port D
	//��������������
    rGPDUP=0xffffffff; // Disable Pull-up register
    //IO�˿�ȫ������ΪLCD��صĹ���
    rGPDCON=0xaaaaaaaa; //Initialize VD[23:8]
    Uart_Printf("Initializing GPIO ports..........\n");
}


/**************************************************************
800��480 16Bpp TFT LCD����ģ���ʼ��
**************************************************************/
void Lcd2410_Init()
{
	//???
    frameBuffer16BitTft800480=(U16 (*)[SCR_XSIZE_TFT_800480])LCDFRAMEBUFFER;
	
	//CLKVAL [17:8]: ����ʱ��Ƶ�ʣ�TFT��VCLK=HCLK/((CLKVAL+1)*2)
	//MMODE [7]: ����VM�ı��ʣ�0=ÿ֡��1=��MVAL����ı��ʣ��˴�Ϊÿ֡����
	//PNRMODE [6:5]: ѡ����ʾģʽ��11=TFT LCD���
	//BPPMODE [4:1]: ѡ��BPP��ÿ���ص�λ����ģʽ��12=1100=TFT��16bpp
	//ENVID [0]: LCD��Ƶ������߼�����ʹ��/��ֹʹ�ܣ�0=��ֹ��1=����
	rLCDCON1=(CLKVAL_TFT_800480<<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;

	//VBPD [31:24]: TFT��ֱͬ���źŵĺ�磬Ϊ31
	//LINEVAL [23:14]: ��Щλ������LCD���Ĵ�ֱ����ĳߴ磬Ҳ����Yֵ��479
	//VFPD [13:6]: TFT��ֱͬ���źŵ�ǰ�磬Ϊ10
	//VSPW [5:0]: TFT��ֱͬ���źŵ�����Ϊ1
	rLCDCON2=(VBPD_800480<<24)|(LINEVAL_TFT_800480<<14)|(VFPD_800480<<6)|(VSPW_800480);
	
	//HBPD [25:19]: TFTˮƽͬ���źŵĺ�磬Ϊ87
	//HOZVAL [18:8]: ��Щλ������LCD����ˮƽ����ĳߴ磬Ҳ����Xֵ��799
	//HFPD [7:0]: TFTˮƽͬ���źŵ�ǰ�磬Ϊ39
	rLCDCON3=(HBPD_800480<<19)|(HOZVAL_TFT_800480<<8)|(HFPD_800480);
	
	//HSPW [7:0]: TFTˮƽͬ���źŵ�����Ϊ127	
	rLCDCON4=(MVAL<<8)|(HSPW_800480);
	
	//[11]: TFT��λѡ��16bpp�����Ƶ���ݵĸ�ʽ��1=5:6:5��ʽ
	//[9]: HSYNC���弫�ԣ�0=������1=����
	//[8]: VSYNC���弫�ԣ�0=������1=����		
	rLCDCON5=(1<<11)|(1<<9)|(1<<8);	//FRM5:6:5,HSYNC and VSYNC are inverted
	
	
	rLCDSADDR1=(((U32)frameBuffer16BitTft800480>>22)<<21)|M5D((U32)frameBuffer16BitTft800480>>1);
	rLCDSADDR2=M5D( ((U32)frameBuffer16BitTft800480+(SCR_XSIZE_TFT_800480*LCD_YSIZE_TFT_800480*2))>>1 );
	rLCDSADDR3=(((SCR_XSIZE_TFT_800480-LCD_XSIZE_TFT_800480)/1)<<11)|(LCD_XSIZE_TFT_800480/1);
	
	//[1]: ����LCD֡ͬ���жϣ�0=������1=����
	//[0]: ����LCD FIFO�жϣ�0=������1=����
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rLPCSEL&=(~7); // Disable LPC3600
	rTPAL=0; // Disable Temp Palette

}



/**************************************************************
800��480 8Bpp TFT LCD ��Դ��������ʹ��
**************************************************************/
void Lcd2410_PowerEnable(int invpwren,int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
    rGPGCON=rGPGCON&(~(3<<8))|(3<<8); //GPG4=LCD_PWREN
    //Enable LCD POWER ENABLE Function
    rLCDCON5=rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
    rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}    


/**************************************************************
LCD��Ƶ�Ϳ����ź��������ֹͣ��1������Ƶ���
**************************************************************/
void Lcd2410_EnvidOnOff(int onoff)
{
    if(onoff==1)
	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}    

/*

//LPC3600 is a timing control logic unit
void Lcd2410_Lpc3600Enable(void)
{
    rLPCSEL&=~(7);
    rLPCSEL|=(7); // 240320,Enable LPC3600
}    

*/



    
