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
#include "bsp_ws2812.h"
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

    /* initialize WS2812 */
    WS2812_Init();
    printf("\r\nWS2812 initialized successfully!\r\n");

    int i;
    int while_count = 1;
    uint32_t buff[4] = {WS2812_RED, WS2812_GREEN, WS2812_BLUE, WS2812_WHITE};

    for (i = 0; i < 4; i++)
    {
        WS2812_Set_Color(0, buff[i]);
        WS2812_Set_Color(1, buff[i]);
        WS2812_Set_Color(2, buff[i]);
        WS2812_Set_Color(3, buff[i]);
        WS2812_Set_Color(4, buff[i]);
        WS2812_Set_Color(5, buff[i]);
        WS2812_Set_Color(6, buff[i]);
        WS2812_Set_Color(7, buff[i]);

        WS2812_Send_Array(); // 立即发送更新
        delay_1ms(1000);
    }

    delay_1ms(1000);

    i = 0;

    while(1)
    {
        WS2812_Set_Color((i + 0) % 8, buff[0]);
        WS2812_Set_Color((i + 1) % 8, buff[1]);
        WS2812_Set_Color((i + 2) % 8, buff[2]);
        WS2812_Set_Color((i + 3) % 8, buff[3]);
        WS2812_Send_Array(); // 发送更新

        // 清除其他LED
        WS2812_Set_Color((i + 4) % 8, WS2812_BLACK);
        WS2812_Set_Color((i + 5) % 8, WS2812_BLACK);
        WS2812_Set_Color((i + 6) % 8, WS2812_BLACK);
        WS2812_Set_Color((i + 7) % 8, WS2812_BLACK);
        WS2812_Send_Array(); // 发送更新

        i++;
        while_count++;

        delay_1ms(100);

        // 清除
        if(while_count >= 100)
        {
            while_count = 1;
            i = 0;
        }
    }
}


