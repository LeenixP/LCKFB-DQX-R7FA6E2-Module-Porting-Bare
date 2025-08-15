#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "hal_data.h"
#include <stdio.h>

#define USE_HORIZONTAL 0  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 280

#else
#define LCD_W 280
#define LCD_H 240
#endif


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

//-----------------LCD端口定义----------------
#define LCD_RES_PIN     BSP_IO_PORT_04_PIN_03
#define LCD_DC_PIN      BSP_IO_PORT_04_PIN_07
#define LCD_CS_PIN      BSP_IO_PORT_04_PIN_08
#define LCD_BLK_PIN     BSP_IO_PORT_04_PIN_09

/* LCD信号控制宏定义 */
#define LCD_RES_Clr()  R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_RES_PIN, 0)//RES
#define LCD_RES_Set()  R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_RES_PIN, 1)

#define LCD_DC_Clr()   R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_DC_PIN, 0)//DC
#define LCD_DC_Set()   R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_DC_PIN, 1)

#define LCD_CS_Clr()   R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_CS_PIN, 0)//CS
#define LCD_CS_Set()   R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_CS_PIN, 1)

#define LCD_BLK_Clr()  R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_BLK_PIN, 0)//BLK
#define LCD_BLK_Set()  R_IOPORT_PinWrite(&g_ioport_ctrl,LCD_BLK_PIN, 1)

void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif
