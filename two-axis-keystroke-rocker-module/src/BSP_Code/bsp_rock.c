/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_rock.h"
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
 * 函 数 名 称：ADC_GET
 * 函 数 功 能：读取一次ADC数据
 * 传 入 参 数：CHx:读取通道是X还是Y
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：LP
**********************************************************/
static int ADC_GET(uint8_t CHx)
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
    while(!adc_flag && timeOut--) { delay_us(1); }

    // 清除ADC采样完成标志位
    adc_flag = false;

    if(!timeOut) {
        printf("ADC_GET Failed!!!\r\n");
        return 0;
    }

    if(CHx == 0){ // x通道

        // 读取ADC数据
        err = R_ADC_Read(&g_adc0_ctrl, BSP_ADC_X_CHANNEL, (uint16_t *)&gAdcResult);

    }else if(CHx == 1){ // y通道

        // 读取ADC数据
        err = R_ADC_Read(&g_adc0_ctrl, BSP_ADC_Y_CHANNEL, (uint16_t *)&gAdcResult);

    }else{

        printf("\r\nCHx Error!!\r\n");
        return 1;
    }

    if (FSP_SUCCESS != err) {
        printf("ADC Read Failed!\n");
        return 9999;  // 返回错误值
    }

    // printf("gAdcResult = %d\r\n",gAdcResult);

    return gAdcResult;
}


/******************************************************************
 * 函 数 名 称：Get_Adc_Joystick_Value
 * 函 数 说 明：对保存的数据进行平均值计算后输出
 * 函 数 形 参：CHx 那个通道值
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
int Get_Adc_Joystick_Value(uint8_t CHx)
{
    int Data = 0;
    int i;

    for(i = 0; i < SAMPLES; i++)
    {
        Data += ADC_GET(CHx);

        delay_ms(10);
    }

    Data = Data / SAMPLES;

    return Data;
}

/******************************************************************
 * 函 数 名 称：Get_MQ2_Percentage_value
 * 函 数 说 明：读取摇杆值，并且返回百分比
 * 函 数 形 参：0=读取摇杆左右值，1=读取摇杆上下值
 * 函 数 返 回：返回百分比
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
int Get_Joystick_Percentage_value(uint8_t dir)
{
    int adc_new = 0;
    int Percentage_value = 0;

    if( dir == 0 )
    {
        adc_new = Get_Adc_Joystick_Value(0); // 通道0：X的值
    }
    else if( dir == 1 )
    {
        adc_new = Get_Adc_Joystick_Value(1); // 通道1：Y的值
    }
    else
    {
        printf("\nCH Error!!\r\n");
    }

    Percentage_value = (uint16_t)(((float)adc_new/4095.0f) * 100.f);
    return Percentage_value;
}

/******************************************************************
 * 函 数 名 称：Get_SW_state
 * 函 数 说 明：读取摇杆是否有按下
 * 函 数 形 参：无
 * 函 数 返 回：0摇杆被按下   1摇杆没有按下
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
uint8_t Get_SW_state(void)
{
    //如果被按下
    if( GET_SW() == 0 )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/******************************************************************
 * 函 数 名 称：Joystick_Init
 * 函 数 说 明：初始化摇杆
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void Joystick_Init(void)
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

    delay_1ms(100); // 等待ADC稳定
}

