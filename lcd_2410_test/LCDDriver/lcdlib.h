/************************************************ 
  NAME    : LCDLIB.H
  DESC    :
  Revision History
  2002.03.12 : draft ver 0.0
 ************************************************/

#ifndef __LCDLIB_H__
#define __LCDLIB_H__

#define MVAL		(13)
#define MVAL_USED 	(0)
#define M5D(n) ((n) & 0x1fffff)	// To get lower 21bits


//TFT LCD Panel(800*480)
#define MODE_TFT_1BIT_800480     (0x4301)
#define MODE_TFT_8BIT_800480     (0x4302)
#define MODE_TFT_16BIT_800480    (0x4304)
#define MODE_TFT_24BIT_800480    (0x4308)


//TFT 800480
#define LCD_XSIZE_TFT_800480 	(800)	
#define LCD_YSIZE_TFT_800480 	(480)



//TFT 800480
#define SCR_XSIZE_TFT_800480 	(LCD_XSIZE_TFT_800480*2)
#define SCR_YSIZE_TFT_800480 	(LCD_YSIZE_TFT_800480*2)



//TFT800480
#define HOZVAL_TFT_800480	(LCD_XSIZE_TFT_800480-1)
#define LINEVAL_TFT_800480	(LCD_YSIZE_TFT_800480-1)


#define VBPD_800480		((32-1)&0xff)
#define VFPD_800480		((11-1)&0xff)
#define HBPD_800480		((88-1)&0x7f)
#define HFPD_800480		((40-1)&0xff)
#define HSPW_800480		((128-1)&0xff)
#define VSPW_800480		((2-1) &0x3f)



#define CLKVAL_TFT_800480 	(0)
    //53.5hz @90Mhz
    //VSYNC,HSYNC should be inverted
    //HBPD=47VCLK,HFPD=15VCLK,HSPW=95VCLK
    //VBPD=32HSYNC,VFPD=9HSYNC,VSPW=1HSYNC
#define CLKVAL_TFT_800600 	(0)

#define LCDFRAMEBUFFER 0x33800000 //_NONCACHE_STARTADDRESS

void Lcd2410_Port_Init(void);
void Lcd2410_Port_Return(void);
void Lcd2410_Init();
void Lcd2410_Init_On(void);

#endif /*__LCDLIB_H__*/
