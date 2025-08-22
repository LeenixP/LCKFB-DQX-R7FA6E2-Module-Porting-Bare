/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_dust.h"
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
 * 函 数 名 称：GP2Y1014AU_Init
 * 函 数 功 能：初始化GP2Y1014AU传感器
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：LP
**********************************************************/
void GP2Y1014AU_Init(void)
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

    delay_1ms(100); // 等待100ms，确保传感器稳定
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
 * 函 数 形 参：num采集次数
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
int Get_ADC_Value(void)
{
    int Data = 0;
    int i;

    for(i = 0; i < SAMPLES; i++)
    {
        Data += ADC_GET();

        delay_ms(2);
    }

    Data = Data / SAMPLES;

    return Data;
}

int Filter(int m)
{
    static int flag_first = 0, _buff[10], sum;
    const int _buff_max = 10;
    int i;

    if (flag_first == 0)
    {
        flag_first = 1;
        for (i = 0, sum = 0; i < _buff_max; i++)
        {
            _buff[i] = m;
            sum += _buff[i];
        }
        return m;
    }
    else
    {
        sum -= _buff[0];
        for (i = 0; i < (_buff_max - 1); i++)
        {
            _buff[i] = _buff[i + 1];
        }
        _buff[9] = m;
        sum += _buff[9];

        i = sum / 10.0;
        return i;
    }
}
/******************************************************************
 * 函 数 名 称：Read_dust_concentration
 * 函 数 说 明：读取粉尘浓度
 * 函 数 形 参：粉尘浓度
 * 函 数 返 回：0成功 其他失败
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
char Read_dust_concentration(float *d_dat)
{
    int value = 0;

    GP2Y1014AU_VO(0);

    delay_us(280);
    value = Get_ADC_Value();
    delay_us(40);

    GP2Y1014AU_VO(1);

    delay_us(9680);

    value = Filter(value);
    *d_dat = 0.17f * value - 0.1f; //转换公式

    return 0;
}

