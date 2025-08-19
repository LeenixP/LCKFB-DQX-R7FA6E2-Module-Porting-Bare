/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_raindrop.h"
#include "stdio.h"

// ADC Scan complete flag
volatile bool adc_flag = false;

/* ADC callback */
void adc_callback(adc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    adc_flag = true;
}

/**********************************************************
 * 函 数 名 称：Raindrop_Init
 * 函 数 功 能：初始化
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：LP
**********************************************************/
void Raindrop_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    // 打开ADC设备
    err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    if (FSP_SUCCESS != err) {
        printf("ADC Open Failed! \n");
        return;
    }

    // 配置ADC扫描
    err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    if (FSP_SUCCESS != err) {
        printf("ADC Scan Config Failed! \n");
        R_ADC_Close(&g_adc0_ctrl);  // 关闭已打开的ADC
        return;
    }
}

/**********************************************************
 * 函 数 名 称：ADC_GET
 * 函 数 功 能：读取一次ADC数据
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：LP
**********************************************************/
static int ADC_GET(void)
{
    int gAdcResult = 0;
    int timeOut = 1000;

    fsp_err_t err = FSP_SUCCESS;

    // 启动ADC扫描
    err = R_ADC_ScanStart(&g_adc0_ctrl);
    if (FSP_SUCCESS != err) {
        printf("ADC扫描启动失败! \n");
        return 9999;  // 返回错误值
    }

    // 等待ADC总线处理完成
    while(!adc_flag && timeOut--)
    {
        delay_us(1);
    }

    // 清除ADC采样完成标志位
    adc_flag = false;

    if(!timeOut)
    {
        printf("ADC_GET Failed!!!\r\n");
        return 0;
    }

    // 读取ADC数据
    err = R_ADC_Read(&g_adc0_ctrl, BSP_ADC_CHANNEL, (uint16_t *)&gAdcResult);
    if (FSP_SUCCESS != err) {
        printf("ADC Read Failed!\n");
        return 9999;  // 返回错误值
    }

    return gAdcResult;
}


/**********************************************************
 * 函 数 名 称：get_raindrop_value
 * 函 数 功 能：读取ADC值
 * 传 入 参 数：
 * 函 数 返 回：测量到的值
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
unsigned int get_raindrop_value(void)
{
    uint32_t Data = 0;
    int i;

    for(i = 0; i < SAMPLES; i++)
    {
        Data += ADC_GET();

        delay_ms(5);
    }

    Data = Data / SAMPLES;

    return Data;
}

/******************************************************************
 * 函 数 名 称：get_raindrop_percentage_value
 * 函 数 说 明：读取雨滴AO值，并且返回百分比
 * 函 数 形 参：无
 * 函 数 返 回：返回百分比
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
unsigned int get_raindrop_percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;

    adc_new = get_raindrop_value();

    Percentage_value = ((float)adc_new / (float)adc_max) * 100.f;
    return Percentage_value;
}

/******************************************************************
 * 函 数 名 称：get_raindrop_do_value
 * 函 数 说 明：读取雨滴DO值，返回0或者1
 * 函 数 形 参：无
 * 函 数 返 回：
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
unsigned char get_raindrop_do_value(void)
{
    return GET_DO();
}


