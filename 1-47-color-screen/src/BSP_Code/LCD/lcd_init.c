#include "lcd_init.h"

static volatile bool g_transfer_complete = false;
void spi0_callback(spi_callback_args_t *arg)
{
	/* 如果是发送完成触发的中断则将发送完成标志位置1 */
    if(SPI_EVENT_TRANSFER_COMPLETE == arg->event)
        g_transfer_complete = true;
}


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
    uint8_t recv_data = 0;
	fsp_err_t err = FSP_SUCCESS;
	volatile uint32_t wTimeout = 10000; // 10ms

	LCD_CS_Clr();

	/* 数据发送 */
    err = R_SPI_WriteRead(&g_spi0_ctrl,
							&dat, 		 // 数据发送
						  	NULL,        // 数据接收
							1,           // 发送长度
							SPI_BIT_WIDTH_8_BITS);
	if (err != FSP_SUCCESS)
	{
		printf("\r\nSPI发送错误\r\n");
		return;
	}

	/* 等待发送成功 */
	while (false == g_transfer_complete)
	{
		if (wTimeout-- == 0)
		{
			g_transfer_complete = false; // 重置标志位
			printf("\r\nSPI发送超时\r\n");
			return;
		}
		delay_us(1);
	}
	g_transfer_complete = false; // 重置标志位

    LCD_CS_Set();

	return recv_data;
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+34);
		LCD_WR_DATA(x2+34);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+34);
		LCD_WR_DATA(x2+34);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+34);
		LCD_WR_DATA(y2+34);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+34);
		LCD_WR_DATA(y2+34);
		LCD_WR_REG(0x2c);//储存器写
	}
}
void LCD_Init(void)
{
    fsp_err_t err = R_SPI_Open(&g_spi0_ctrl, g_spi0.p_cfg);
    if (err != FSP_SUCCESS)
    {
        printf("\r\nSPI启动错误\r\n");
    }
	
	LCD_RES_Clr();//复位
	delay_ms(30);
	LCD_RES_Set();
	delay_ms(100);
	LCD_BLK_Set();//打开背光
  delay_ms(100);
//	
	LCD_WR_REG(0x11); 
	// delay_ms(120); 
	LCD_WR_REG(0x36); 
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33); 

	LCD_WR_REG(0xB7); 
	LCD_WR_DATA8(0x35);  

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x35);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x13);   

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20);  

	LCD_WR_REG(0xC6); 
	LCD_WR_DATA8(0x0F);    

	LCD_WR_REG(0xD0); 
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xD6); 
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3E);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x30);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x28);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3E);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0x32);
	
// 	LCD_WR_REG(0x2A);
//	LCD_WR_DATA8(0x00);
//	LCD_WR_DATA8(0x22);
//	LCD_WR_DATA8(0x00);
//	LCD_WR_DATA8(0xCD);
//	LCD_WR_DATA8(0x2B);
//	LCD_WR_DATA8(0x00);
//	LCD_WR_DATA8(0x00);
//	LCD_WR_DATA8(0x01);
//	LCD_WR_DATA8(0x3F);
//	LCD_WR_REG(0x2C);
	LCD_WR_REG(0x21); 
  
  LCD_WR_REG(0x11);
  delay_ms(120);	
	LCD_WR_REG(0x29); 
}








