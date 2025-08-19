/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#ifndef BSP_CODE_BSP_TOUCHKEY_H_
#define BSP_CODE_BSP_TOUCHKEY_H_

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

/* TTP224触摸按键引脚定义 */
#define TTP224_KEY_IN1_GPIO_PIN     BSP_IO_PORT_04_PIN_03
#define TTP224_KEY_IN2_GPIO_PIN     BSP_IO_PORT_04_PIN_07
#define TTP224_KEY_IN3_GPIO_PIN     BSP_IO_PORT_04_PIN_08
#define TTP224_KEY_IN4_GPIO_PIN     BSP_IO_PORT_04_PIN_09

/* GPIO状态读取 */
static inline bsp_io_level_t TTP224_KEY_IN1_READ() {
    bsp_io_level_t p_pin_value;
    fsp_err_t err = R_IOPORT_PinRead(&g_ioport_ctrl, TTP224_KEY_IN1_GPIO_PIN, &p_pin_value);
    if(err != FSP_SUCCESS)
    {
        printf("GPIO Input Read Failed!!\r\n");
    }
    return p_pin_value;
}
static inline bsp_io_level_t TTP224_KEY_IN2_READ() {
    bsp_io_level_t p_pin_value;
    fsp_err_t err = R_IOPORT_PinRead(&g_ioport_ctrl, TTP224_KEY_IN2_GPIO_PIN, &p_pin_value);
    if(err != FSP_SUCCESS)
    {
        printf("GPIO Input Read Failed!!\r\n");
    }
    return p_pin_value;
}
static inline bsp_io_level_t TTP224_KEY_IN3_READ() {
    bsp_io_level_t p_pin_value;
    fsp_err_t err = R_IOPORT_PinRead(&g_ioport_ctrl, TTP224_KEY_IN3_GPIO_PIN, &p_pin_value);
    if(err != FSP_SUCCESS)
    {
        printf("GPIO Input Read Failed!!\r\n");
    }
    return p_pin_value;
}
static inline bsp_io_level_t TTP224_KEY_IN4_READ() {
    bsp_io_level_t p_pin_value;
    fsp_err_t err = R_IOPORT_PinRead(&g_ioport_ctrl, TTP224_KEY_IN4_GPIO_PIN, &p_pin_value);
    if(err != FSP_SUCCESS)
    {
        printf("GPIO Input Read Failed!!\r\n");
    }
    return p_pin_value;
}

/* 函数声明 */
char Key_IN1_Scan(void);//触摸按键1的输入状态
char Key_IN2_Scan(void);//触摸按键2的输入状态
char Key_IN3_Scan(void);//触摸按键3的输入状态
char Key_IN4_Scan(void);//触摸按键4的输入状态

#endif /* BSP_CODE_BSP_TOUCHKEY_H_ */

