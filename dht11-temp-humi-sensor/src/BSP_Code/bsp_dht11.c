/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_dht11.h"

#define DHT11_DEBUG     0       // 调试信息 1：开启 0：关闭
#define DHT11_TIMEOUT   1000    // 超时阈值

/******************************************************************
 * 函 数 名 称：DHT11_Init
 * 函 数 说 明：配置DHT11的初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void DHT11_Init(void)
{
    //调用 R_IOPORT_Open 函数来初始化 IOPORT 模块
    R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);

    /* 设置DHT11数据引脚输出高电平 */
    DATA_GPIO_OUT(1);

    delay_1ms(100); // 等待DHT11稳定
}



/******************************************************************
 * 函 数 名 称：DHT11_GPIO_Mode_OUT
 * 函 数 说 明：配置DHT11的数据引脚为输出模式
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void DHT11_GPIO_Mode_OUT(void)
{
    // 在运行过程中配置DHT11数据引脚为输出模式
    // 这里使用R_IOPORT_PinCfg函数配置引脚
    fsp_err_t err = R_IOPORT_PinCfg(&g_ioport_ctrl, DHT11_DAT_GPIO_PIN,
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH
		| (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT
		| (uint32_t) IOPORT_CFG_PORT_OUTPUT_HIGH));
    if(err != FSP_SUCCESS)
    {
        printf("DHT11_GPIO_Mode_OUT Failed!!\r\n");
    }
}

/******************************************************************
 * 函 数 名 称：DHT11_GPIO_Mode_IN
 * 函 数 说 明：配置DHT11的数据引脚为输入模式
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void DHT11_GPIO_Mode_IN(void)
{
    // 在运行过程中配置DHT11数据引脚为输入模式
    // 这里使用R_IOPORT_PinCfg函数配置引脚
    fsp_err_t err = R_IOPORT_PinCfg(&g_ioport_ctrl, DHT11_DAT_GPIO_PIN,
                    (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT);
    if(err != FSP_SUCCESS)
    {
        printf("DHT11_GPIO_Mode_IN Failed!!\r\n");
    }
}


/******************************************************************
 * 函 数 名 称：DHT11_Start
 * 函 数 说 明：开始DHT11测量
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void DHT11_Start(void)
{
    DHT11_GPIO_Mode_OUT(); // 输出模式
    DATA_GPIO_OUT(1);
    DATA_GPIO_OUT(0);
    delay_ms(25);        // 保持18+ms低电平
    DATA_GPIO_OUT(1);
    delay_us(25);        // 主机释放总线

    DHT11_GPIO_Mode_IN(); // 切换输入模式
}

/******************************************************************
 * 函 数 名 称：DHT11_CheckResponse
 * 函 数 说 明：检查DHT11从机的相应是否正确
 * 函 数 形 参：无
 * 函 数 返 回：1：正确 0：错误
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint8_t DHT11_CheckResponse(void)
{
    uint32_t timeout = 0;

    // 等待从机响应发送电平响应（低电平）（80µs）
    timeout = DHT11_TIMEOUT;
    while(DATA_GPIO_IN() && timeout--)
    {
        if(timeout == 0)
        {
            printf("DHT11_CheckResponse Failed[1]!!\r\n");
            return 0; // 失败
        }
    }

    // 等待从机响应发送电平响应（高电平）（80µs）
    timeout = DHT11_TIMEOUT;
    while((!DATA_GPIO_IN()) && timeout--)
    {
        if(timeout == 0)
        {
            printf("DHT11_CheckResponse Failed[2]!!\r\n");
            return 0; // 失败
        }
    }

    // 进入前导低电平
    timeout = DHT11_TIMEOUT;
    while(DATA_GPIO_IN() && timeout--)
    {
        if(timeout == 0)
        {
            printf("DHT11_CheckResponse Failed[3]!!\r\n");
            return 0; // 失败
        }
    }

    return 1;
}


/******************************************************************
 * 函 数 名 称：DHT11_ReadBit
 * 函 数 说 明：读取一位数据位
 * 函 数 形 参：无
 * 函 数 返 回：0 或 1
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint8_t DHT11_ReadBit(void)
{
    uint8_t bit = 0;
    uint8_t timeCount = 0;
    uint32_t timeout;

    // 等待前导低电平过去
    // 等待信号线由低变高
    timeout = DHT11_TIMEOUT;
    while((!DATA_GPIO_IN()) && timeout--)
    {
        if(timeout == 0)
        {
            printf("DHT11_ReadBit Failed[1]!!\r\n");
            return 0;
        }
    }

    // 判断是0还是1?
    timeout = DHT11_TIMEOUT;
    while(DATA_GPIO_IN() && timeout--)
    {
        timeCount++;
        delay_us(1); // 等待1us
        if(timeout == 0)
        {
            printf("DHT11_ReadBit Failed[2]!!\r\n");
            return 0; // 失败
        }
    }

    // 只要大于30us的高电平即可判断为数据1
    if(timeCount >= 30)
        bit = 1; // 数据1
    else
        bit = 0; // 数据0

    return bit;

}
/******************************************************************
 * 函 数 名 称：DHT11_Read_Data
 * 函 数 说 明：根据时序读取温湿度数据
 * 函 数 形 参：
 * 函 数 返 回：0=数据校验失败  其他=错误
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint8_t DHT11_Read_Data(float *temperature, float *humidity)
{
    int i;
    uint8_t data[5] = {0};
    uint64_t val = 0;

    // 开始起始信号
    DHT11_Start();

    // 检查响应是否合规
    if(0 == DHT11_CheckResponse())
    {
        printf("DHT11_CheckResponse Failed!!\r\n");
        return 0;
    }

    // 数据读取核心（40-bit）
    for(i = 0; i < 40; i++)
    {
        val <<= 1;
        val |= DHT11_ReadBit();
    }

    // 校验与数据提取
    data[0] = (val >> 32) & 0xFF; // 湿度整数
    data[1] = (val >> 24) & 0xFF; // 湿度小数
    data[2] = (val >> 16) & 0xFF; // 温度整数
    data[3] = (val >> 8)  & 0xFF; // 温度小数
    data[4] = val & 0xFF;         // 校验和

#if DHT11_DEBUG
    printf("data[0] = %d\r\n",data[0]);
    printf("data[1] = %d\r\n",data[1]);
    printf("data[2] = %d\r\n",data[2]);
    printf("data[3] = %d\r\n",data[3]);
    printf("data[4] = %d\r\n",data[4]);
#endif

    // 校验计算：前4字节和 = 第5字节
    if ((data[4] != (data[0] + data[1] + data[2] + data[3])) ||
        (data[4] == 0)) {
        printf("CheckSum Failed!!!\r\n");
        return 0; // 校验失败
    }

    // 保存温湿度
    *humidity    = data[0] + (data[1] * 0.1f);  // 湿度（%）
    *temperature = data[2] + (data[3] * 0.1f);  // 温度（℃）

#if DHT11_DEBUG
    printf("T: %d\r\n",(int)*temperature);
    printf("H: %d\r\n",(int)*humidity);
#endif

    return 1;
}
