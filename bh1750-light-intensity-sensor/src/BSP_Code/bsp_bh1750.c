/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_bh1750.h"
#include "stdio.h"

// 定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
// ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0xB8
#define   SlaveAddress   0x46


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
        SDA( dat & 0x80 );
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
 * 函 数 名 称：Single_Write
 * 函 数 说 明：向BH1750写入命令
 * 函 数 形 参：REG_Address=写入的命令
 * 函 数 返 回：0写入成功  1=器件地址错误（识别不到模块） 2=命令错误
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
char Single_Write_BH1750(uint8_t REG_Address)
{
      IIC_Start();                        //起始信号
      Send_Byte(SlaveAddress);            //发送设备地址+写信号
      if( I2C_WaitAck() != 0 )return 1;
      Send_Byte(REG_Address);             //内部寄存器地址
      if( I2C_WaitAck() != 0 )return 2;
      IIC_Stop();                         //发送停止信号
      return 0;
}

/******************************************************************
 * 函 数 名 称：Multiple_read_BH1750
 * 函 数 说 明：读取BH1750内部数据
 * 函 数 形 参：bh1750_value: 光照度(单位:lx)
 * 函 数 返 回：0成功
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
char Multiple_read_BH1750(float *bh1750_value)
{
    uint16_t dis_data=0;
    uint8_t dat_buff[2];

    Single_Write_BH1750(0x10); // 连续高分辨率模式测量
    delay_ms(180);             // 测量一般需要120ms

    IIC_Start();                            //起始信号
    Send_Byte(SlaveAddress+1);         //发送设备地址+读信号
    I2C_WaitAck();

    dat_buff[0] = Read_Byte();              //读取高8位
    IIC_Send_Ack(0);                   //回应ACK
    dat_buff[1] = Read_Byte();              //读取低8位
    IIC_Send_Ack(1);                   //回应NOACK
    IIC_Stop();                             //停止信号

    //合成数据，即光照数据
    dis_data=( (uint16_t)dat_buff[0] << 8 ) + dat_buff[1];

    *bh1750_value = (float)dis_data/1.2f;
    return 0;
}

/******************************************************************
 * 函 数 名 称：BH1750_Init
 * 函 数 说 明：初始化BH1750
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void BH1750_Init(void)
{
    SCL(1);
    SDA(1);
    delay_ms(100); // 等待传感器稳定

    Single_Write_BH1750(0x01);//上电
    delay_ms(100); // 等待传感器稳定
}

