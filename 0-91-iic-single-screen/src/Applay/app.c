/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "app.h"
#include "stdio.h"

#include "bsp_uart.h"
#include "oled.h"
#include "bmp.h"

/******************************************************************
 * 函 数 名 称：led_blink
 * 函 数 说 明：该函数用于控制LED灯的闪烁效果
 *             运行时，LED灯每隔500毫秒闪烁一次
 *             完整运行此函数需要1s时间
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void led_blink(void)
{
    /* Set the pin to low */
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_02, BSP_IO_LEVEL_LOW);
    
    /* Delay for 500 milliseconds */
    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
    
    /* Set the pin to high */
    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_02, BSP_IO_LEVEL_HIGH);

    /* Delay for another 500 milliseconds */
    R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
}


/******************************************************************
 * 函 数 名 称：Run
 * 函 数 说 明：该函数是用户自定义的入口函数，等效于 main_app() 函数。
 *             在此函数中可以编写用户的应用逻辑代码。
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void Run(void)
{
    /* 初始化调试串口 */
    /* | RX:P100 | TX:P101 | */
    UART0_Debug_Init();

    printf("\r\n= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\r\n");
    printf("\r\n=== Welcome to use the DQX-R7FA6E2BB3CNE development board ====\r\n");
    printf("\r\n======================= www.lckfb.com =========================\r\n");
    printf("\r\n======================= wiki.lckfb.com ========================\r\n");
    printf("\r\n======================= [Debug Uart0] =========================\r\n");
    printf("\r\n=================== | RX:P100 | TX:P101 | =====================\r\n");
    printf("\r\n= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\r\n");

    u8 t=' ';

    OLED_Init();
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    while(1)
    {
        OLED_ShowPicture(0,0,128,64,BMP1,1);
        OLED_Refresh();
        delay_ms(500);
        OLED_Clear();
        OLED_ShowChinese(0,0,0,16,1);//中
        OLED_ShowChinese(18,0,1,16,1);//景
        OLED_ShowChinese(36,0,2,16,1);//园
        OLED_ShowChinese(54,0,3,16,1);//电
        OLED_ShowChinese(72,0,4,16,1);//子
        OLED_ShowChinese(90,0,5,16,1);//技
        OLED_ShowChinese(108,0,6,16,1);//术
        OLED_ShowString(8,16,"ZHONGJINGYUAN",16,1);
        OLED_ShowString(20,32,"2014/05/01",16,1);
        OLED_ShowString(0,48,"ASCII:",16,1);
        OLED_ShowString(63,48,"CODE:",16,1);
        OLED_ShowChar(48,48,t,16,1);//显示ASCII字符
        t++;
        if(t>'~')t=' ';
        OLED_ShowNum(103,48,t,3,16,1);
        OLED_Refresh();
        delay_ms(500);
        OLED_Clear();
        OLED_ShowChinese(0,0,0,16,1);  //16*16 中
        OLED_ShowChinese(16,0,0,24,1); //24*24 中
        OLED_ShowChinese(24,20,0,32,1);//32*32 中
        OLED_ShowChinese(64,0,0,64,1); //64*64 中
        OLED_Refresh();
        delay_ms(500);
        OLED_Clear();
        OLED_ShowString(0,0,"ABC",8,1);//6*8 “ABC”
        OLED_ShowString(0,8,"ABC",12,1);//6*12 “ABC”
        OLED_ShowString(0,20,"ABC",16,1);//8*16 “ABC”
        OLED_ShowString(0,36,"ABC",24,1);//12*24 “ABC”
        OLED_Refresh();
        delay_ms(500);
        OLED_ScrollDisplay(11,4,1);
    }
}


