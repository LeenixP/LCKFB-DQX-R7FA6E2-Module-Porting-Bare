/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_ws2812.h"

#define WS2812_0_CODE           0xC0    // 0码
#define WS2812_1_CODE           0xFC    // 1码
#define WS2812_RESET_CODE       0x00    // 复位码


static uint8_t   LedsArray[WS2812_MAX * 3];      // 定义颜色数据存储数组
static uint32_t  ledsCount   = WS2812_NUMBERS;   // 定义实际彩灯默认个数
static uint32_t  nbLedsBytes = WS2812_NUMBERS*3; // 定义实际彩灯颜色数据个数

static volatile bool g_transfer_complete = false;
void spi0_callback(spi_callback_args_t *arg)
{
	/* 如果是发送完成触发的中断则将发送完成标志位置1 */
    if(SPI_EVENT_TRANSFER_COMPLETE == arg->event)
        g_transfer_complete = true;
}

static uint8_t spi_read_write_byte(uint8_t dat)
{
	/* 数据发送 */
    fsp_err_t err = R_SPI_WriteRead(&g_spi0_ctrl,
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
	while (false == g_transfer_complete);
	g_transfer_complete = false; // 重置标志位

    return dat;
}


/******************************************************************
 * 函 数 名 称：WS2812_WriteData
 * 函 数 说 明：向WS2812写入len长度的字节
 * 函 数 形 参：send_buff数据地址   len字节长度
 * 函 数 返 回：0成功  -1失败
 * 作       者：LCKFB
 * 备       注：1码的时序 = 高电平580ns~1us    再低电平220ns~420ns
 *              0码的时序 = 高电平220ns~380ns  再低电平580ns~1us
******************************************************************/
static int WS2812_WriteData(uint8_t *send_buff, uint32_t len)
{
    int i, bit;

    // 熄灭信号
    spi_read_write_byte(0x00);

    for (i = 0; i < len; i++)
    {
        for (bit = 0; bit < 8; bit++)
        {
            if (send_buff[i] & (0x80 >> bit)) // 当前位为1
            {
                //发送1码
                spi_read_write_byte(WS2812_1_CODE);
            }
            else // 当前位为0
            {
                //发送0码
                spi_read_write_byte(WS2812_0_CODE);
            }
        }
    }
    return 0;
}


/******************************************************************
 * 函 数 名 称：WS2812_RESET
 * 函 数 说 明：复位ws2812
 * 函 数 形 参：无
 * 函 数 返 回：0成功  -1失败
 * 作       者：LCKFB
 * 备       注：低电平280us以上
******************************************************************/
int WS2812_RESET(void)
{
    int i;

    for(i = 0; i < 20; i++)
    {
        //发送复位码
        spi_read_write_byte(WS2812_RESET_CODE);
    }

    return 0;
}


/******************************************************************
 * 函 数 名 称：WS2812_Set_Color
 * 函 数 说 明：设置彩灯颜色
 * 函 数 形 参：LED_NUM控制的第几个灯  Color颜色数据
 * 函 数 返 回：0成功  -1失败
 * 作       者：LCKFB
 * 备       注：
******************************************************************/
int WS2812_Set_Color(uint8_t LED_NUM, uint64_t Color)
{
    if( LED_NUM >= ledsCount )
    {
        return -1;    //to avoid overflow
    }

    LedsArray[LED_NUM * 3]     = 0;
    LedsArray[LED_NUM * 3 + 1] = 0;
    LedsArray[LED_NUM * 3 + 2] = 0;

    LedsArray[LED_NUM * 3]     = (Color>>8)  & 0xff;
    LedsArray[LED_NUM * 3 + 1] = (Color>>16) & 0xff;
    LedsArray[LED_NUM * 3 + 2] = (Color>>0)  & 0xff;

    return 0;
}

/******************************************************************
 * 函 数 名 称：WS2812_Send_Array
 * 函 数 说 明：发送彩灯数据
 * 函 数 形 参：无
 * 函 数 返 回：0成功  -1失败
 * 作       者：LCKFB
 * 备       注：
******************************************************************/
int WS2812_Send_Array(void)
{
    WS2812_RESET();

    delay_ms(10);

    int ret = WS2812_WriteData(LedsArray, nbLedsBytes);
    if(ret != 0)
    {
        return -1;
    }

    return 0;
}

/******************************************************************
 * 函 数 名 称：WS2812_Init
 * 函 数 说 明：初始化WS2812
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：
******************************************************************/
void WS2812_Init(void)
{
    fsp_err_t err = R_SPI_Open(&g_spi0_ctrl, g_spi0.p_cfg);
    if (err != FSP_SUCCESS)
    {
        printf("\r\nSPI启动错误\r\n");
    }

    delay_1ms(100); // 等待SPI稳定
}

