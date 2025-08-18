/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_irdistance.h"
#include "stdio.h"
#include "math.h"

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
    err = R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &gAdcResult);
    if (FSP_SUCCESS != err) {
        printf("ADC Read Failed!\n");
        return 9999;  // 返回错误值
    }

    return gAdcResult;
}

/**********************************************************
 * 函 数 名 称：Get_Adc_Value
 * 函 数 功 能：获得某个通道的值
 * 传 入 参 数：Count：采集次数
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：LP
**********************************************************/
int Get_Adc_IRdistance_Value(uint8_t Count)
{
    int gAdcResult = 0;
    uint8_t i = 0;
    for(i = 0; i < Count; i++)
    {
        //获取数据
        gAdcResult += ADC_GET();
    }

    return (gAdcResult / Count);
}

/******************************************************************
 * 函 数 名 称：Get_illume_Percentage_value
 * 函 数 说 明：计算红外测距的测量距离
 * 函 数 形 参：无
 * 函 数 返 回：返回测量距离
 * 作       者：LC
 * 备       注：无
******************************************************************/
double Get_IRdistance_Distance(void)
{
    double adc_new = 0;
    double Distance = 0;

    adc_new = (((double)Get_Adc_IRdistance_Value(10) / 4095.f) * 3.3f);

    // 根据官方代码库链接：https://github.com/zoubworldArduino/ZSharpIR
    // 得到距离换算公式：
    //【GP2Y0A02YK0F：Using MS Excel, we can calculate function (For distance > 15cm) :
    // Distance = 60.374 X POW(Volt , -1.16)】

    Distance = 60.374f * pow(adc_new,-1.16);

    return Distance;
}

