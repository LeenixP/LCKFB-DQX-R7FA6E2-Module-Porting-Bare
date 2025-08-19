/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#ifndef BSP_CODE_BSP_FLAME_H_
#define BSP_CODE_BSP_FLAME_H_

#include "hal_data.h"
#include <stdio.h>

#ifndef delay_ms
#define delay_ms(x)   R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MILLISECONDS)
#endif
#ifndef delay_1ms
#define delay_1ms(x)  R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MILLISECONDS)
#endif
#ifndef delay_us
#define delay_us(x)   R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MICROSECONDS)
#endif
#ifndef delay_1us
#define delay_1us(x)  R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MICROSECONDS)
#endif

#ifndef u8
#define u8  uint8_t
#endif
#ifndef u16
#define u16 uint16_t
#endif
#ifndef u32
#define u32 uint32_t
#endif

#define BSP_ADC_CHANNEL     ADC_CHANNEL_0

/* DO 引脚 */
#define BSP_DO_GPIO_PIN     BSP_IO_PORT_04_PIN_07


static inline bsp_io_level_t GET_DO(void) {
    bsp_io_level_t p_pin_value;
    fsp_err_t err = R_IOPORT_PinRead(&g_ioport_ctrl, BSP_DO_GPIO_PIN, &p_pin_value);
    if(err != FSP_SUCCESS)
    {
        printf("GPIO Input Read Failed!!\r\n");
    }
    return p_pin_value;
}

// 采样次数
#define SAMPLES     5

void Flame_Module_Init(void);
int Get_Adc_Flame_Value(void);
unsigned int Get_Flame_Percentage_value(void);
char Get_Flame_Do_value(void);

#endif /* BSP_CODE_BSP_FLAME_H_ */

