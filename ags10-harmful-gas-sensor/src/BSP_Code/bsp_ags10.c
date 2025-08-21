/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */
#include "bsp_ags10.h"

//起始信号
void AGS10_IIC_Start(void)
{
    SDA_OUT();
    SDA(1);
    SCL(1);
    delay_1us(5);
    SDA(0);
    delay_1us(5);
    SCL(0);
    delay_1us(5);
}

//停止信号
void AGS10_IIC_Stop(void)
{
    SDA_OUT();
    SCL(0);
    SDA(0);
    SCL(1);
    delay_1us(5);
    SDA(1);
    delay_1us(5);
}

//发送非应答
void AGS10_IIC_Send_Nack(void)
{
    SDA_OUT();
    SCL(0);
    SDA(0);
    delay_us(5);

    SDA(1);
    SCL(1);
    delay_1us(5);

    SCL(0);
    SDA(1);
}

//发送应答
void AGS10_IIC_Send_Ack(void)
{
    SDA_OUT();
    SCL(0);
    SDA(0);
    delay_us(5);

    SDA(0);
    SCL(1);
    delay_1us(5);

    SCL(0);
    SDA(1);
}

/**********************************************************
 * 函 数 名 称：I2C_WaitAck
 * 函 数 功 能：等待从机应答
 * 传 入 参 数：无
 * 函 数 返 回：1=非应答         0=应答
 * 作       者：LCKFB
 * 备       注：无
**********************************************************/
unsigned char AGS10_I2C_WaitAck(void)
{
        char ack = 0;
        unsigned char ack_flag = 100;
        SDA_IN();
        SDA(1);
        delay_us(5);
        SCL(1);
        delay_us(5);

        while( (SDA_GET()==1)  &&  ( ack_flag ) )
        {
                ack_flag--;
                delay_1us(5);
        }

        //非应答
        if( ack_flag <= 0 )
        {
                AGS10_IIC_Stop();
                return 1;
        }
        else//应答
        {
                SCL(0);
                SDA_IN();
        }
        return ack;
}

//发送一个字节
void AGS10_IIC_Send_Byte(uint8_t dat)
{
    int i = 0;
    SDA_OUT();
    SCL(0);

    for( i = 0; i < 8; i++ )
    {
        SDA( (dat & 0x80) >> 7 );
        delay_1us(5);
        SCL(1);
        delay_1us(5);
        SCL(0);
        delay_1us(5);
        dat<<=1;
    }
}

//接收一个字节
unsigned char AGS10_IIC_Read_Byte(void)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
    {
        SCL(0);
        delay_1us(5);
        SCL(1);
        delay_1us(5);
        receive<<=1;
        if( SDA_GET() )
        {
            receive |= 1;
        }
    }
    SCL(0);
    return receive;
}



/**********************************************************
 * 函 数 名 称：ags10_read
 * 函 数 功 能：读取AGS10的TVOC浓度数据
 * 传 入 参 数：无
 * 函 数 返 回：0: 成功
 *             1： 通信失败
 *             2：发送失败
 *             3：等待超时
 *             4: 校验失败
 * 作       者：LCKFB
 * 备       注：
**********************************************************/
char ags10_read(uint32_t *tvoc_dat)
{
    uint8_t timeout = 0;
    uint8_t data[5] = {0};

    AGS10_IIC_Start();
    AGS10_IIC_Send_Byte(0X34);
    if( AGS10_I2C_WaitAck() == 1 ) return 1;
    AGS10_IIC_Send_Byte(0X00);
    if( AGS10_I2C_WaitAck() == 1 ) return 2;
    AGS10_IIC_Stop();

    do{
        delay_1ms(1);
        timeout++;
        AGS10_IIC_Start();
        AGS10_IIC_Send_Byte(0X35);
    }while( (AGS10_I2C_WaitAck() == 1) && (timeout < 50) );

    //如果超时
    if( timeout >= 50 ){

        printf("\ntimeout\r\n");
        return 3;
    }

    data[0] = AGS10_IIC_Read_Byte();
    AGS10_IIC_Send_Ack();
    data[1] = AGS10_IIC_Read_Byte();
    AGS10_IIC_Send_Ack();
    data[2] = AGS10_IIC_Read_Byte();
    AGS10_IIC_Send_Ack();
    data[3] = AGS10_IIC_Read_Byte();
    AGS10_IIC_Send_Ack();
    data[4] = AGS10_IIC_Read_Byte();
    AGS10_IIC_Send_Nack();

    AGS10_IIC_Stop();

    *tvoc_dat = ((uint32_t)data[1]<<16) | ((uint16_t)data[2]<<8) | data[3];

    return 0;
}


/**********************************************************
 * 函 数 名 称：ags10_init
 * 函 数 功 能：ags10初始化
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：
**********************************************************/
void ags10_init(void)
{
    SCL(1);
    SDA(1);

    delay_ms(100); // 等待传感器稳定
}

