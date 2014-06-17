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

//二维数组的指针，指向存放图片的二维数组
U16 (*frameBuffer16BitTft800480)[SCR_XSIZE_TFT_800480];

void Lcd2410_Init();
void Lcd2410_PowerEnable(int invpwren,int pwren);
void Lcd2410_EnvidOnOff(int onoff);

//LCD初始化入口函数
void Lcd2410_Init_On(void)
{
	Lcd2410_Port_Init();//LCD数据和控制端口初始化
	Lcd2410_Init();	//LCD功能模块初始化
	Lcd2410_PowerEnable(0, 0); //LCD电源控制引脚使能 //0-开背光灯，1-关
    Lcd2410_EnvidOnOff(1);	//LCD视频和控制信号输出或者停止，1开启视频输出
}


//320×240 16Bpp TFT LCD数据和控制端口初始化
void Lcd2410_Port_Init(void)
{

    rGPCUP=0xffffffff; // Disable Pull-up register
    rGPCCON=0xaaaaaaaa; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
    rGPDUP=0xffffffff; // Disable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[23:8]
    //Uart_Printf("Initializing GPIO ports..........\n");
}



//LCD功能模块初始化
void Lcd2410_Init()
{
    frameBuffer16BitTft800480=(U16 (*)[SCR_XSIZE_TFT_800480])LCDFRAMEBUFFER;
	rLCDCON1=(CLKVAL_TFT_800480<<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;
        // TFT LCD panel,16bpp TFT,ENVID=off
	rLCDCON2=(VBPD_800480<<24)|(LINEVAL_TFT_800480<<14)|(VFPD_800480<<6)|(VSPW_800480);
	rLCDCON3=(HBPD_800480<<19)|(HOZVAL_TFT_800480<<8)|(HFPD_800480);
	rLCDCON4=(MVAL<<8)|(HSPW_800480);
	rLCDCON5=(1<<11)|(1<<9)|(1<<8);	//FRM5:6:5,HSYNC and VSYNC are inverted
	rLCDSADDR1=(((U32)frameBuffer16BitTft800480>>22)<<21)|M5D((U32)frameBuffer16BitTft800480>>1);
	rLCDSADDR2=M5D( ((U32)frameBuffer16BitTft800480+(SCR_XSIZE_TFT_800480*LCD_YSIZE_TFT_800480*2))>>1 );
	rLCDSADDR3=(((SCR_XSIZE_TFT_800480-LCD_XSIZE_TFT_800480)/1)<<11)|(LCD_XSIZE_TFT_800480/1);
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rLPCSEL&=(~7); // Disable LPC3600
	rTPAL=0; // Disable Temp Palette

}



//320×240 8Bpp TFT LCD 电源控制引脚使能
void Lcd2410_PowerEnable(int invpwren,int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP=rGPGUP&(~(1<<4))|(1<<4); // Pull-up disable
    rGPGCON=rGPGCON&(~(3<<8))|(3<<8); //GPG4=LCD_PWREN
    //Enable LCD POWER ENABLE Function
    rLCDCON5=rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
    rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}    


//LCD视频和控制信号输出或者停止，1开启视频输出
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



    
