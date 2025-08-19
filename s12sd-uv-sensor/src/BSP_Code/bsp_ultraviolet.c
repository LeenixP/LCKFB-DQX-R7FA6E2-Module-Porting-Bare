/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_ultraviolet.h"

// ADC Scan complete flag
volatile bool adc_flag = false;

/* ADC callback */
void adc_callback(adc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    adc_flag = true;
}


/**********************************************************
 * 函 数 名 称：S12SD_UV_GPIO_Init
 * 函 数 功 能：初始化紫外线传感器GPIO
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：LP
**********************************************************/
void S12SD_UV_GPIO_Init(void)
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
 * 函 数 形 参：无
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LCKFB
 * 备       注：
******************************************************************/
int Get_ADC_Value( void )
{
    int Data = 0;
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
 * 函 数 名 称：Get_Ultraviolet_Intensity
 * 函 数 说 明：判断当前紫外线强度等级
 * 函 数 形 参：value=ADC读取的值
 * 函 数 返 回：0~11 紫外线强度等级由低到高，11最高
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
char Get_Ultraviolet_Intensity(uint16_t value)
{
      char ret = 0;

      if( value < 227 )//紫外线强度0级
      {
            ret = 0;
      }
      if( value >= 227 && value < 318 )//紫外线强度1级
      {
            ret = 1;
      }
      if( value >= 318 && value < 408 )//紫外线强度2级
      {
            ret = 2;
      }
      if( value >= 408 && value < 503 )//紫外线强度3级
      {
            ret = 3;
      }
      if( value >= 503 && value < 606 )//紫外线强度4级
      {
            ret = 4;
      }
      if( value >= 606 && value < 696 )//紫外线强度5级
      {
            ret = 5;
      }
      if( value >= 696 && value < 795 )//紫外线强度6级
      {
            ret = 6;
      }

      if( value >= 795 && value < 881 )//紫外线强度7级
      {
            ret = 7;
      }
      if( value >= 881 && value < 976 )//紫外线强度8级
      {
            ret = 8;
      }
      if( value >= 976 && value < 1079 )//紫外线强度9级
      {
            ret = 9;
      }
      if( value >= 1079 && value < 1170 )//紫外线强度10级
      {
            ret = 10;
      }
      if( value >= 1170  )//紫外线强度11级
      {
            ret = 11;
      }

      return ret;
}


