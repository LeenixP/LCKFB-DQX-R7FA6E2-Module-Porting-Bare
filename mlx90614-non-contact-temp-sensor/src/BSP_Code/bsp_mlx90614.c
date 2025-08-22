/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_mlx90614.h"
#include "stdio.h"


/******************************************************************
 * 函 数 名 称：MLX90614_Init
 * 函 数 说 明：MLX90614初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void MLX90614_Init(void)
{
    SDA_OUT();

    SCL(1);
    SDA(1);

    delay_ms(100); // 等待传感器稳定
}



/******************************************************************
 * 函 数 名 称：IIC_Start
 * 函 数 说 明：IIC起始时序
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Start(void)
{
    SDA_OUT();

    SDA(1);
    delay_us(5);
    SCL(1);
    delay_us(5);

    SDA(0);
    delay_us(5);
    SCL(0);
    delay_us(5);

}
/******************************************************************
 * 函 数 名 称：IIC_Stop
 * 函 数 说 明：IIC停止信号
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Stop(void)
{
    SDA_OUT();
    SCL(0);
    SDA(0);

    SCL(1);
    delay_us(5);
    SDA(1);
    delay_us(5);

}

/******************************************************************
 * 函 数 名 称：IIC_Send_Ack
 * 函 数 说 明：主机发送应答或者非应答信号
 * 函 数 形 参：0发送应答  1发送非应答
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Send_Ack(unsigned char ack)
{
    SDA_OUT();
    SCL(0);
    SDA(0);
    delay_us(5);
    if(!ack) SDA(0);
    else     SDA(1);
    SCL(1);
    delay_us(5);
    SCL(0);
    SDA(1);
}


/******************************************************************
 * 函 数 名 称：I2C_WaitAck
 * 函 数 说 明：等待从机应答
 * 函 数 形 参：无
 * 函 数 返 回：0有应答  1超时无应答
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned char I2C_WaitAck(void)
{
    char ack = 0;
    unsigned char ack_flag = 10;
    SCL(0);
    SDA(1);
    SDA_IN();
    delay_us(5);
    SCL(1);
    delay_us(5);

    while( (SDA_GET()==1) && ( ack_flag ) )
    {
        ack_flag--;
        delay_us(5);
    }

    if( ack_flag <= 0 )
    {
        IIC_Stop();
        return 1;
    }
    else
    {
        SCL(0);
        SDA_OUT();
    }
    return ack;
}

/******************************************************************
 * 函 数 名 称：Send_Byte
 * 函 数 说 明：写入一个字节
 * 函 数 形 参：dat要写人的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void Send_Byte(uint8_t dat)
{
    int i = 0;
    SDA_OUT();
    SCL(0);//拉低时钟开始数据传输

    for( i = 0; i < 8; i++ )
    {
        SDA( (dat & 0x80) >> 7 );
        delay_us(1);
        SCL(1);
        delay_us(5);
        SCL(0);
        delay_us(5);
        dat<<=1;
    }
}

/******************************************************************
 * 函 数 名 称：Read_Byte
 * 函 数 说 明：IIC读时序
 * 函 数 形 参：无
 * 函 数 返 回：读到的数据
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned char Read_Byte(void)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
    {
        SCL(0);
        delay_us(5);
        SCL(1);
        delay_us(5);
        receive<<=1;
        if( SDA_GET() )
        {
            receive|=1;
        }
        delay_us(5);
    }
    SCL(0);
    return receive;
}

/************************************************************
 * 函数名称：MLX90615_Read
 * 函数说明：读取MLX90615的温度
 * 型    参：SlaveAddr = 器件地址  RegAddr = 要操作的寄存器地址
 * 返 回 值：温度值
 * 备    注：   SlaveAddr = 0X5A默认器件地址
 *              RegAddr   = 0X07读取被测量物体温度
 *              RegAddr   = 0X06读取环境温度
*************************************************************/
float MLX90614_Read(unsigned char SlaveAddr, unsigned char RegAddr)
{
    IIC_Start();
    Send_Byte((SlaveAddr << 1) | 0); // 写命令
    if(I2C_WaitAck()) {
        IIC_Stop();
        return 0.0;
    }

    Send_Byte(RegAddr); // 寄存器地址
    if(I2C_WaitAck()) {
        IIC_Stop();
        return 0.0;
    }

    // delay_ms(1);

    IIC_Start();
    Send_Byte((SlaveAddr << 1) | 1); // 读命令
    if(I2C_WaitAck()) {
        IIC_Stop();
        return 0.0;
    }

    uint8_t lowByte = Read_Byte(); // 低8位
    IIC_Send_Ack(0);
    uint8_t highByte = Read_Byte(); // 高8位
    IIC_Send_Ack(1);
    IIC_Stop();

    uint16_t tempRaw = (highByte << 8) | lowByte;
    float temperature = (tempRaw * 0.02) - 273.15;

    return temperature;
}

