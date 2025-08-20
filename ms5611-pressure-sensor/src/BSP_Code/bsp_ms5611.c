/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_ms5611.h"
#include "stdio.h"

//出厂校准值
//Cal_C1_6[0]                                 = 厂家信息
//Cal_C1_6[1] ~ Cal_C1_6[6] = 校准值
//Cal_C1_6[7]                                 = 校准值的CRC校验
uint16_t Cal_C1_6[8];

/******************************************************************
 * 函 数 名 称：MS5611_Init
 * 函 数 说 明：MS5611初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void MS5611_Init(void)
{
    SCL(1);
    SDA(1);

    delay_ms(100); // 等待传感器稳定
}



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

/**********************************************************
 * 函 数 名 称：MS5611_Reset
 * 函 数 功 能：MS5611的复位
 * 传 入 参 数：无
 * 函 数 返 回：0=复位成功  1=器件地址错误  2=命令无应答
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
char MS5611_Reset(void)
{
    IIC_Start();//起始信号
    Send_Byte(0xee|0); //器件地址+写
    if( I2C_WaitAck() == 1 )return 1;
    Send_Byte(0x1e); //复位命令
    if( I2C_WaitAck() == 1 )return 2;
    IIC_Stop();

    return 0;
}

//C1-C6 16位  6个地址  每一个地址16位
/**********************************************************
 * 函 数 名 称：MS5611_Read_PROM
 * 函 数 功 能：读取出厂校准值
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
void MS5611_Read_PROM(void)
{
    uint8_t data_H=0,data_L=0;
    uint8_t i = 0;

    for( i = 0; i < 8; i++ )
    {
        IIC_Start();//起始信号
        Send_Byte(0xee|0); //器件地址+写
        I2C_WaitAck();
        Send_Byte( 0xA0 + i * 2 ); //寄存器地址
        I2C_WaitAck();
        IIC_Stop();

        delay_us(200);

        IIC_Start();//起始信号
        Send_Byte(0xee|1); //器件地址+读
        I2C_WaitAck();

        data_H = Read_Byte();//读取的数据高8位
        IIC_Send_Ack(0);
        data_L = Read_Byte();//读取的数据低8位
        IIC_Send_Ack(1);

        IIC_Stop();
        //保存出厂校准数据
        Cal_C1_6[i] = (data_H<<8) | data_L;
    }
}

/**********************************************************
 * 函 数 名 称：MS5611_Read_D1_D2
 * 函 数 功 能：读取气压D1和温度D2的初始数据
 * 传 入 参 数：regaddr=0x48或0x58
 * 函 数 返 回：返回读取后整合为24位的数据
 * 作       者：LCKFB
 * 备       注：
 *              regaddr= 0x48         读取D1数据(OSR=4096)
 *              regaddr= 0x58         读取D2数据(OSR=4096)
**********************************************************/
uint32_t MS5611_Read_D1_D2(uint8_t regaddr)
{
        uint32_t dat = 0;
        uint8_t buff[3] ={0};
        IIC_Start();//起始信号
        Send_Byte(0xee|0); //器件地址+写
        if( I2C_WaitAck() == 1 )printf("D1 NACK -1\r\n");
        Send_Byte(regaddr); //OSR = 4096
        if( I2C_WaitAck() == 1 )printf("D1 NACK -2\r\n");
        IIC_Stop();

        delay_ms(10);

        IIC_Start();//起始信号
        Send_Byte(0xee|0); //器件地址+写
        if( I2C_WaitAck() == 1 )printf("D1 NACK -3\r\n");
        Send_Byte(0X00);
        if( I2C_WaitAck() == 1 )printf("D1 NACK -4\r\n");
        IIC_Stop();

        delay_ms(10);

        IIC_Start();//起始信号
        Send_Byte(0xee|1); //器件地址+读
        if( I2C_WaitAck() == 1 )printf("D1 NACK -5\r\n");

        buff[0] = Read_Byte();
        IIC_Send_Ack(0);
        buff[1] = Read_Byte();
        IIC_Send_Ack(0);
        buff[2] = Read_Byte();
        IIC_Send_Ack(1);
        IIC_Stop();

        dat = ((((uint32_t)buff[0]<<16) | ( (uint32_t)buff[1]<<8)) | buff[2]);
        return dat;
}


/**********************************************************
 * 函 数 名 称：MS5611_Read_TempPress
 * 函 数 功 能：获取读到的温度和气压数据
 * 传 入 参 数：数据存放地址
 * 函 数 返 回：0成功，其他失败
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
uint8_t MS5611_Read_TempPress(float *Temperature, float *Pressure)
{
    uint32_t D1 = 0;
    uint32_t D2 = 0;
    uint32_t DT = 0;

    long long TEMP = 0;

    long long SENS = 0;
    long long P =0;
    long long OFF = 0;

    /* ======= 温度 ======== */

    D1 = MS5611_Read_D1_D2(0x48);
    delay_ms(10);

    D2 = MS5611_Read_D1_D2(0x58);
    delay_ms(10);

    DT = D2 - (Cal_C1_6[5] * 256.0);

    TEMP = 2000 + ((DT * Cal_C1_6[6]) / 8388608.0);

    *Temperature =  (float)(TEMP/100.0f);

    /* ======= 气压 ======== */

    OFF = Cal_C1_6[2] * 65536.0 + Cal_C1_6[4] * DT / 128;
    SENS = (Cal_C1_6[1] * 32768.0) + ((Cal_C1_6[3] * DT ) / 256.0);

    P = (D1 * SENS / 2097152.0 - OFF) / 32768.0;

    *Pressure = P/100.0;

    return 0;
}

