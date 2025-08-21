/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_sg90.h"

uint8_t Servo_Angle = 0;   //舵机角度

/******************************************************************
   配置占空比 范围 0 ~ (per-1)
   周期：20ms
   t = 0.5ms(500us) ------占空比2.5%-------------舵机会转动 0 °
   t = 1.0ms(1000us)------占空比5%---------------舵机会转动 45°
   t = 1.5ms(1500us) ------占空比7.5%-------------舵机会转动 90°
   t = 2.0ms(2000us) ------占空比10%--------------舵机会转动 135°
   t = 2.5ms(2500us) ------占空比12.5%------------舵机会转动180°
******************************************************************/
/******************************************************************
 * 函 数 名 称：Set_SG90_Servo_Angle
 * 函 数 说 明：设置角度
 * 函 数 形 参：angle=要设置的角度，范围0-180
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void Set_SG90_Servo_Angle(uint32_t angle)
{
    timer_info_t timer_info;
    fsp_err_t err;

    if (angle > 180) angle = 180; // 角度限幅
    Servo_Angle = angle;          // 记录角度（可选）

    // 获取定时器信息（关键：获取总周期计数值）
    err = R_GPT_InfoGet(&g_timer1_ctrl, &timer_info);
    if (err != FSP_SUCCESS) {
        printf("%s Error: Timer info failed (%d)\n", __func__, err);
        return;
    }

#if SG90_DEBUG
    printf("Set SG90 Angle: %d\n", angle);
    printf("Timer Period Counts: %d\n", timer_info.period_counts);
#endif

    // 计算20ms周期内高电平时间（µs）
    float high_time_us = 500.0 + ((float)angle * (2000.0 / 180.0));

    // 计算占空比所对应的计数值
    uint32_t duty_counts = (high_time_us / 20000.0) * timer_info.period_counts;

    // 设置PWM占空比
    err = R_GPT_DutyCycleSet(&g_timer1_ctrl, duty_counts, GPT_IO_PIN_GTIOCA);
    if (err != FSP_SUCCESS) {
        printf("%s Error: Duty set failed (%d)\n", __func__, err);
    }
}

/******************************************************************
 * 函 数 名 称：读取当前角度
 * 函 数 说 明：Get_SG90_Servo_Angle
 * 函 数 形 参：无
 * 函 数 返 回：当前角度
 * 作       者：LCKFB
 * 备       注：使用前必须确保之前使用过
                Set_SG90_Servo_Angle
                函数设置过角度
******************************************************************/
uint8_t Get_SG90_Servo_Angle(void)
{
    return Servo_Angle;
}

/******************************************************************
 * 函 数 名 称：SG90_Init
 * 函 数 说 明：SG90舵机初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void SG90_Init(void)
{
    R_GPT_Open(&g_timer1_ctrl, &g_timer1_cfg);
    R_GPT_Start(&g_timer1_ctrl);

    delay_1ms(100); // 等待100ms，确保舵机初始化完成
}

