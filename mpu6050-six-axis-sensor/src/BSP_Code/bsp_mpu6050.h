/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#ifndef BSP_CODE_BSP_MPU6050_H_
#define BSP_CODE_BSP_MPU6050_H_

#include "hal_data.h"
#include <stdio.h>
#include "inv_mpu.h"                   // 操作MPU
#include "inv_mpu_dmp_motion_driver.h" // 操作DMP
#include "math.h"

#ifndef u8
#define u8 uint8_t
#endif
#ifndef u16
#define u16 uint16_t
#endif
#ifndef u32
#define u32 uint32_t
#endif

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


#define Module_SCL_PIN   BSP_IO_PORT_04_PIN_08 // SCL
#define Module_SDA_PIN   BSP_IO_PORT_04_PIN_09 // SDA

//SDA输入模式
#define SDA_IN() { \
    fsp_err_t err = R_IOPORT_PinCfg(&g_ioport_ctrl, Module_SDA_PIN, \
                    (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT); \
    if(err != FSP_SUCCESS) { \
        printf("DHT11_GPIO_Mode_IN Failed!!\r\n"); \
    } \
}

//SDA输出模式
#define SDA_OUT() { \
    fsp_err_t err = R_IOPORT_PinCfg(&g_ioport_ctrl, Module_SDA_PIN, \
            ((uint32_t) IOPORT_CFG_DRIVE_HIGH \
		| (uint32_t) IOPORT_CFG_NMOS_ENABLE \
		| (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT \
		| (uint32_t) IOPORT_CFG_PORT_OUTPUT_HIGH)); \
    if(err != FSP_SUCCESS) { \
        printf("DHT11_GPIO_Mode_OUT Failed!!\r\n"); \
    } \
}

// SCL引脚和SDA引脚的输出
#define SCL(BIT)    R_IOPORT_PinWrite(&g_ioport_ctrl, Module_SCL_PIN, BIT)
#define SDA(BIT)    R_IOPORT_PinWrite(&g_ioport_ctrl, Module_SDA_PIN, BIT)

// 获取SDA引脚的电平状态
static inline bsp_io_level_t SDA_GET(void) {
    bsp_io_level_t p_pin_value;
    fsp_err_t err = R_IOPORT_PinRead(&g_ioport_ctrl, Module_SDA_PIN, &p_pin_value);
    if(err != FSP_SUCCESS) {
        printf("GPIO Input Read Failed!!\r\n");
    }
    return p_pin_value;
}


//MPU6050的AD0是IIC地址引脚，接地则IIC地址为0x68,接VCC则IIC地址为0x69
#define MPU6050_RA_SMPLRT_DIV       0x19        //陀螺仪采样率 地址
#define MPU6050_RA_CONFIG           0x1A        //设置数字低通滤波器 地址
#define MPU6050_RA_GYRO_CONFIG      0x1B        //陀螺仪配置寄存器
#define MPU6050_RA_ACCEL_CONFIG     0x1C        //加速度传感器配置寄存器
#define MPU_INT_EN_REG              0X38        //中断使能寄存器
#define MPU_USER_CTRL_REG           0X6A        //用户控制寄存器
#define MPU_FIFO_EN_REG             0X23        //FIFO使能寄存器
#define MPU_PWR_MGMT2_REG           0X6C        //电源管理寄存器2
#define MPU_GYRO_CFG_REG            0X1B        //陀螺仪配置寄存器
#define MPU_ACCEL_CFG_REG           0X1C        //加速度计配置寄存器
#define MPU_CFG_REG                 0X1A        //配置寄存器
#define MPU_SAMPLE_RATE_REG         0X19        //采样频率分频器
#define MPU_INTBP_CFG_REG           0X37        //中断/旁路设置寄存器

#define MPU6050_RA_PWR_MGMT_1       0x6B
#define MPU6050_RA_PWR_MGMT_2       0x6C

#define MPU6050_WHO_AM_I            0x75
#define MPU6050_SMPLRT_DIV          0            //8000Hz
#define MPU6050_DLPF_CFG            0
#define MPU6050_GYRO_OUT            0x43         //MPU6050陀螺仪数据寄存器地址
#define MPU6050_ACC_OUT             0x3B         //MPU6050加速度数据寄存器地址

#define MPU6050_RA_TEMP_OUT_H       0x41        //温度高位
#define MPU6050_RA_TEMP_OUT_L       0x42        //温度低位

#define MPU_ACCEL_XOUTH_REG         0X3B        //加速度值,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG         0X3C        //加速度值,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG         0X3D        //加速度值,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG         0X3E        //加速度值,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG         0X3F        //加速度值,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG         0X40        //加速度值,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG           0X41        //温度值高八位寄存器
#define MPU_TEMP_OUTL_REG           0X42        //温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG          0X43        //陀螺仪值,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG          0X44        //陀螺仪值,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG          0X45        //陀螺仪值,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG          0X46        //陀螺仪值,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG          0X47        //陀螺仪值,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG          0X48        //陀螺仪值,Z轴低8位寄存器

char MPU6050_Init(void);
void MPU6050ReadGyro(short *gyroData);
void MPU6050ReadAcc(short *accData);
float MPU6050_GetTemp(void);
uint8_t MPU6050ReadID(void);

#endif /* BSP_CODE_BSP_MPU6050_H_ */

