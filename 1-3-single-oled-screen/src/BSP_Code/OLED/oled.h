#ifndef __OLED_H
#define __OLED_H 

#include "hal_data.h"
#include <stdio.h>


#ifndef delay_ms
#define delay_ms(x)   R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MILLISECONDS)
#endif
#ifndef delay_1ms
#define delay_1ms(x)  R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MILLISECONDS)
#endif
#ifndef delay_us
#define delay_us(x)   R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MICROSECONDS)
#endif
#ifndef delay_1us
#define delay_1us(x)  R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MICROSECONDS)
#endif

#ifndef u8
#define u8  uint8_t
#endif
#ifndef u16
#define u16 uint16_t
#endif
#ifndef u32
#define u32 uint32_t
#endif

//-----------------OLED端口定义---------------- 
#define OLED_RES_PIN     BSP_IO_PORT_04_PIN_03
#define OLED_DC_PIN      BSP_IO_PORT_04_PIN_07
#define OLED_CS_PIN      BSP_IO_PORT_04_PIN_08

#define OLED_RES_Clr() R_IOPORT_PinWrite(&g_ioport_ctrl,OLED_RES_PIN,0)//RES
#define OLED_RES_Set() R_IOPORT_PinWrite(&g_ioport_ctrl,OLED_RES_PIN,1)

#define OLED_DC_Clr()  R_IOPORT_PinWrite(&g_ioport_ctrl,OLED_DC_PIN,0)//DC
#define OLED_DC_Set()  R_IOPORT_PinWrite(&g_ioport_ctrl,OLED_DC_PIN,1)

#define OLED_CS_Clr()  R_IOPORT_PinWrite(&g_ioport_ctrl,OLED_CS_PIN,0)//CS
#define OLED_CS_Set()  R_IOPORT_PinWrite(&g_ioport_ctrl,OLED_CS_PIN,1)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
void OLED_Init(void);

#endif

