/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_bmp180.h"
#include "stdio.h"
#include "math.h"

typedef struct _BMP180_STRUCT{
    short AC1;
    short AC2;
    short AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    short B1;
    short B2;
    short MB;
    short MC;
    short MD;
}_BMP180_PARAM_;

_BMP180_PARAM_ param={0};

long B5 = 0;

/******************************************************************
 * 函 数 名 称：IIC_Start
 * 函 数 说 明：IIC起始时序
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
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
 * 作       者：LCKFB
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
 * 作       者：LCKFB
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
 * 作       者：LCKFB
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
 * 作       者：LCKFB
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
 * 作       者：LCKFB
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

/******************************************************************
 * 函 数 名 称：BMP180_Write_Cmd
 * 函 数 说 明：向BMP180写入一个字节数据
 * 函 数 形 参：regaddr寄存器地址  cmd写入的数据
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：regaddr=0xf4, cmd=0X2E
******************************************************************/
void BMP180_Write_Cmd(uint8_t regaddr,uint8_t cmd)
{
        IIC_Start();//起始信号
        Send_Byte(0XEE);//器件地址+写
        if( I2C_WaitAck() == 1 ) printf("Write_Cmd NACK -1\r\n");

        Send_Byte(regaddr);
        if( I2C_WaitAck() == 1 ) printf("Write_Cmd NACK -2\r\n");

        Send_Byte(cmd);
        if( I2C_WaitAck() == 1 ) printf("Write_Cmd NACK -3\r\n");

        IIC_Stop();
}

/******************************************************************
 * 函 数 名 称：BMP180_Read16
 * 函 数 说 明：读取BMP180数据
 * 函 数 形 参：regaddr读取的地址 len读取的长度
 * 函 数 返 回：读取到的数据
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
uint16_t BMP180_Read16(uint16_t regaddr,uint8_t len)
{
      int timeout = 0;
      uint16_t dat[3] = {0};
      int i =0;
      for( i = 0; i < len; i++ )
      {
            IIC_Start();//起始信号
            Send_Byte(0XEE);//器件地址+写
            if( I2C_WaitAck() == 1 ) printf("Read_Reg NACK -1\r\n");
            Send_Byte(regaddr+i);
            if( I2C_WaitAck() == 1 ) printf("Read_Reg NACK -2\r\n");

            do{
                  timeout++;
                  delay_ms(1);
                  IIC_Start();//起始信号
                  Send_Byte(0XEF);//器件地址+读
            }while(I2C_WaitAck() == 1 && (timeout < 5) );

            dat[i] = Read_Byte();
            IIC_Send_Ack(1);
            IIC_Stop();
            delay_ms(1);
      }
      if( len == 2 ) return ( ((uint32_t)dat[0]<<8) | dat[1] );
      if( len == 3 ) return (( ((uint32_t)dat[0]<<16) | ((uint32_t)dat[1]<<8) | (dat[2]) ) >> 8);
      return 0;
}

/******************************************************************
 * 函 数 名 称：BMP180_Get_Temperature
 * 函 数 说 明：读取温度单位℃
 * 函 数 形 参：无
 * 函 数 返 回：温度
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
float BMP180_Get_Temperature(void)
{
        long UT = 0;
        long X1 = 0, X2 = 0;

        BMP180_Write_Cmd(0XF4, 0X2E);
        delay_ms(6);
        UT = BMP180_Read16(0xf6,2);

        X1 = ((long)UT - param.AC6) * param.AC5 / 32768.0;
        X2 = ((long)param.MC * 2048.0) / ( X1 + param.MD );
        B5 = X1 + X2;
        return ((B5+8)/16.0)*0.1f;
}

/******************************************************************
 * 函 数 名 称：BMP180_Get_Pressure
 * 函 数 说 明：读取气压,单位Pa
 * 函 数 形 参：无
 * 函 数 返 回：当前气压，单位Pa
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
float BMP180_Get_Pressure(void)
{
    long UP = 0;
    uint8_t oss = 0;
    long X1 = 0, X2 = 0;

    BMP180_Get_Temperature();

    BMP180_Write_Cmd(0XF4, (0X34+(oss<<6)));
    delay_ms(10);
    UP = BMP180_Read16(0xf6,3);

    int32_t B6 = B5 - 4000;

    X1 = (B6 * B6 >> 12) * param.B2 >> 11;

    X2 = param.AC2 * B6 >> 11;

    int32_t X3 = X1 + X2;

    int32_t B3 = (((param.AC1 << 2) + X3) + 2) >> 2;

    X1 = param.AC3 * B6 >> 13;

    X2 = (B6 * B6 >> 12) * param.B1 >> 16;

    X3 = (X1 + X2 + 2) >> 2;

    uint32_t B4 = param.AC4 * (uint32_t)(X3 + 32768) >> 15;

    uint32_t B7 = ((uint32_t)UP - B3) * 50000;

    int32_t p;
    if(B7 < 0x80000000)
    {
        p = (B7 << 1) / B4;
    }
    else
    {
        p = B7/B4 << 1;
    }

    X1 = (p >> 8) * (p >> 8);

    X1 = (X1 * 3038) >> 16;

    X2 = (-7375 * p) >> 16;

    p = p + ((X1 + X2 + 3791) >> 4);
     return p;

}

/******************************************************************
 * 函 数 名 称：BMP180_Get_Altitude
 * 函 数 说 明：计算海拔高度
 * 函 数 形 参：p=当前气压
 * 函 数 返 回：海拔高度
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
float BMP180_Get_Altitude(float p)
{
    // #define PRESSURE_OF_SEA        101325.0f // 参考海平面压强
    float altitude = 0;
    altitude = 44330*(1 - pow((p)/ 101325.0f, 1.0f / 5.255f));
    return altitude;
}

/******************************************************************
 * 函 数 名 称：BMP180_Get_param
 * 函 数 说 明：获取出厂校准值
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void BMP180_Get_param(void)
{
    param.AC1 =  BMP180_Read16(0xaa,2);
    param.AC2 =  BMP180_Read16(0xac,2);
    param.AC3 =  BMP180_Read16(0xae,2);
    param.AC4 =  BMP180_Read16(0xb0,2);
    param.AC5 =  BMP180_Read16(0xb2,2);
    param.AC6 =  BMP180_Read16(0xb4,2);
    param.B1  =  BMP180_Read16(0xb6,2);
    param.B2  =  BMP180_Read16(0xb8,2);
    param.MB  =  BMP180_Read16(0xba,2);
    param.MC  =  BMP180_Read16(0xbc,2);
    param.MD  =  BMP180_Read16(0xbe,2);
}


/******************************************************************
 * 函 数 名 称：BMP180_Init
 * 函 数 说 明：BMP180传感器初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void BMP180_Init(void)
{
    SCL(1);
    SDA(1);
    delay_ms(100); // 等待传感器稳定

    BMP180_Get_param(); // 校准
    delay_ms(500); // 等待传感器稳定
}

