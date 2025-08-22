/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_irtracking.h"
#include "stdio.h"


// ADC Scan complete flag
volatile bool adc_flag = false;

/* ADC callback */
void adc_callback(adc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    adc_flag = true;
}

void Module_BSP_Init(void)
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


/******************************************************************
 * 函 数 名 称：Get_ADC_Value
 * 函 数 说 明：对ADC值进行平均值计算后输出
 * 函 数 形 参：
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
int Get_ADC_TRACK_Value(void)
{
    char i = 0;
    int AdcValue = 0;

    /* 因为采集 SAMPLES 次，故循环 SAMPLES 次 */
    for(i = 0; i < SAMPLES; i++)
    {
        /*    累加    */
        AdcValue += ADC_GET();
    }
    /* 求平均值 */
    AdcValue = AdcValue / SAMPLES;

    return AdcValue;
}
/******************************************************************
 * 函 数 名 称：Get_TRACK_DO
 * 函 数 说 明：读取传感器识别状态
 * 函 数 形 参：无
 * 函 数 返 回：1=识别为黑色   0=识别的不是黑色
 * 作       者：LCKFB
 * 备       注：可以通过模块上的可调电阻调整识别黑色的阈值
******************************************************************/
char Get_TRACK_DO(void)
{
    if( IR_DO() == 1 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

