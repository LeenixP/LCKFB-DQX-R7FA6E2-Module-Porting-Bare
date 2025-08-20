/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_sht20.h"
#include "stdio.h"

/******************************************************************
 * 函 数 名 称：IIC_Start
 * 函 数 说 明：IIC起始信号
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Start(void)
{
        SDA_OUT();

        SCL(0);
        SDA(1);
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
 * 函 数 说 明：主机发送应答
 * 函 数 形 参：0应答  1非应答
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Send_Ack(uint8_t ack)
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
 * 函 数 名 称：IIC_Wait_Ack
 * 函 数 说 明：等待从机应答
 * 函 数 形 参：无
 * 函 数 返 回：1=无应答   0=有应答
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint8_t IIC_Wait_Ack(void)
{
        char ack = 0;
        unsigned char ack_flag = 10;
        SDA_IN();
        SDA(1);
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
 * 函 数 名 称：IIC_Write
 * 函 数 说 明：IIC写一个字节
 * 函 数 形 参：dat写入的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void IIC_Write(uint8_t data)
{
        int i = 0;
        SDA_OUT();
        SCL(0);//拉低时钟开始数据传输

        for( i = 0; i < 8; i++ )
        {
            SDA( (data & 0x80) >> 7 );
            delay_us(2);
            data<<=1;
            delay_us(6);
            SCL(1);
            delay_us(4);
            SCL(0);
            delay_us(4);

        }
}

/******************************************************************
 * 函 数 名 称：IIC_Read
 * 函 数 说 明：IIC读1个字节
 * 函 数 形 参：无
 * 函 数 返 回：读出的1个字节数据
 * 作       者：LC
 * 备       注：无
******************************************************************/
uint8_t IIC_Read(void)
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
    return receive;
}

/******************************************************************
 * 函 数 名 称：SHT20_Read
 * 函 数 说 明：测量温湿度
 * 函 数 形 参：regaddr寄存器地址 regaddr=0xF3测量温度 =0xF5测量湿度
 *              sht20_value数据地址
 * 函 数 返 回：0成功，其他失败
 * 作       者：LC
 * 备       注：无
******************************************************************/
char SHT20_Read(uint8_t regaddr, float *sht20_value)
{
    unsigned char data_H = 0;
    unsigned char data_L = 0;
    float temp = 0;
    int timeOut = 1000;

    IIC_Start();
    IIC_Write(0x80|0);
    if( IIC_Wait_Ack() == 1 ) printf("error -1\r\n");
    IIC_Write(regaddr);
    if( IIC_Wait_Ack() == 1 ) printf("error -2\r\n");

    do{
        delay_us(10);
        IIC_Start();
        IIC_Write(0x80|1);

    }while( IIC_Wait_Ack() == 1 && timeOut--);

    if(!timeOut)
    {
        printf("SHT20_Read Failed!!\r\n");
        return 1;
    }

    delay_us(20);

    data_H = IIC_Read();
    IIC_Send_Ack(0);
    data_L = IIC_Read();
    IIC_Send_Ack(1);
    IIC_Stop();

    if( regaddr == SHT20_TEMP_REG_ADDR ) // 温度
    {
        *sht20_value = (((uint16_t)data_H<<8)|data_L) / 65536.0 * 175.72 - 46.85;
    }
    if( regaddr == SHT20_HUMI_REG_ADDR ) // 湿度
    {
        *sht20_value = (((uint16_t)data_H<<8)|data_L) / 65536.0 * 125.0 - 6;
    }

    return 0;
}


/******************************************************************
 * 函 数 名 称：SHT20_Init
 * 函 数 说 明：SHT20 Sensor 初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void SHT20_Init(void)
{
    SCL(1);
    SDA(1);

    delay_ms(100); // 等待传感器稳定
}

