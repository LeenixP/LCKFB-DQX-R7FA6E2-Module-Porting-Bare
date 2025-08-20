/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_sgp30.h"
#include "stdio.h"


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

        SCL(0);
        delay_us(1);

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

/******************************************************************
 * 函 数 名 称：SGP30_Write
 * 函 数 说 明：SGP30写命令
 * 函 数 形 参：regaddr_H命令高8位   regaddr_L命令低8位
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void SGP30_Write_cmd(uint8_t regaddr_H, uint8_t regaddr_L)
{
	  IIC_Start();
	  Send_Byte(0XB0);         //发送器件地址+写指令
	  I2C_WaitAck();
	  Send_Byte(regaddr_H);    //发送控制地址
	  I2C_WaitAck();
	  Send_Byte(regaddr_L);    //发送数据
	  I2C_WaitAck();
	  IIC_Stop();
	  delay_ms(100);
}




/******************************************************************
 * 函 数 名 称：SGP30_Read
 * 函 数 说 明：读取数据并进行CRC校验
 * 函 数 形 参：
 *              - co2_dat: 输出CO₂浓度值
 *              - tvoc_dat: 输出TVOC浓度值
 * 函 数 返 回：0: 读取成功且CRC校验通过 其他:失败
 * 作       者：LC
 * 备       注：SGP30获取的数据格式为：2位CO2数据+1位CO2的CRC校验+2位TVOC数据+1位TVOC的CRC校验。
                模块上电需要15s左右初始化，在初始化阶段读取的CO2浓度为400ppm，TVOC为0ppd且恒定不变。
                因此上电后一直读，直到TVOC不为0并且CO2不为400，SGP30模块才初始化完成。
                初始化完成后刚开始读出数据会波动比较大，属于正常现象，一段时间后会逐渐趋于稳定。
                气体类传感器比较容易受环境影响，测量数据出现波动是正常的，可以添加滤波函数进行滤波。
******************************************************************/
char SGP30_Read(uint16_t *co2_dat, uint16_t *tvoc_dat)
{
    uint8_t buffer[6] = {0};  // 存储原始数据（6字节）
    int i;

    SGP30_Write_cmd(0x20,0x08);

    /* --- 1. 发起读取请求 --- */
    IIC_Start();
    Send_Byte(0XB1); //发送器件地址+读指令
    if(I2C_WaitAck())
        return -1;

    /* --- 2. 读取6字节数据 --- */
    for (i = 0; i < 5; i++)
    {
        buffer[i] = Read_Byte();
        IIC_Send_Ack(0);  // 发送ACK（非结束位）
    }
    buffer[5] = Read_Byte();
    IIC_Send_Ack(1);      // 最后字节发送NACK
    IIC_Stop();

    /* --- 4. 数据 --- */
    *co2_dat  = (buffer[0] << 8) | buffer[1];
    *tvoc_dat = (buffer[3] << 8) | buffer[4];

    return 0;  // 成功
}

/******************************************************************
 * 函 数 名 称：SGP30_Init
 * 函 数 说 明：SGP30初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void SGP30_Init(void)
{
    SCL(1);
    SDA(1);
    delay_ms(100); // 等待传感器稳定

    SGP30_Write_cmd(0x20, 0x03);
    delay_ms(500); // 等待传感器稳定
}

