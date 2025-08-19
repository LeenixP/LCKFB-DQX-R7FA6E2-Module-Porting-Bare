/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_sht30.h"

/* 调试开关 1开启 0关闭 */
#define SHT30_DEBUG     0

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
	SDA_IN();
	SDA(1);
	delay_us(5);
	SCL(1);
	delay_us(5);
	while( (GET_SDA()==1) && ( ack_flag ) )
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
		SDA_IN();
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
void Send_Byte(u8 dat)
{
	int i = 0;
	SDA_OUT();
	SCL(0);//拉低时钟开始数据传输

	for( i = 0; i < 8; i++ )
	{
		SDA( (dat & 0x80) >> 7 );
		delay_us(2);
		dat<<=1;
		delay_us(6);
		SCL(1);
		delay_us(4);
		SCL(0);
		delay_us(4);
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
		if( GET_SDA() )
		{
			receive|=1;
		}

		delay_us(5);
	}
	return receive;
}

/******************************************************************
 * 函 数 名 称：SHT30_Write_mode
 * 函 数 说 明：在周期模式下设置测量周期与可重复性命令
 * 函 数 形 参：dat设置命令
                常用的有：每一秒采集0.5次  0x2024
                         每一秒采集1次    0x2126
                         每一秒采集2次    0x2220
                         每一秒采集4次    0x2334
                         每一秒采集10次   0x2721
 * 函 数 返 回：
 * 作       者：LC
 * 备       注：
******************************************************************/
char SHT30_Write_mode(uint16_t dat)
{
      IIC_Start();

      // << 1 是将最后一位置0，设置为写命令
      Send_Byte((0X44 << 1) | 0 );
      //返回0为产生了应答，返回1说明通信失败
      if( I2C_WaitAck() == 1 )return 1;
      //发送命令的高8位
      Send_Byte((dat >> 8 ) );
      //返回0为产生了应答，返回1说明通信失败
      if( I2C_WaitAck() == 1 )return 2;
      //发送命令的低8位
      Send_Byte(dat & 0xff );
      //返回0为产生了应答，返回1说明通信失败
      if( I2C_WaitAck() == 1 )return 3;
      // IIC_Stop();
      return 0;
}

/******************************************************************
 * 函 数 名 称：SHT30_Read
 * 函 数 说 明：读取温湿度值
 * 函 数 形 参：- cmd 读取的命令
                 周期模式：0xe000
                 单次模式：0x2c06 or 0x2400
               - temp: 温度数据存储地址
               - humi: 湿度存储地址
 * 函 数 返 回：0读取成功  其他失败
 * 作       者：LC
 * 备       注：当前为周期模式读取，如使用单次模式，则将
 *              【设置周期模式命令】下的命令注释即可。
******************************************************************/
uint8_t SHT30_Read(uint16_t cmd, float *temp, float *humi)
{
    uint16_t i = 0;
    unsigned char buff[6] = {0};
    uint16_t data_16 = 0;

    IIC_Start();
    Send_Byte( (0x44<<1) | 0);
    if( I2C_WaitAck() == 1 )return 1;
    Send_Byte((cmd >> 8 ));
    if( I2C_WaitAck() == 1 )return 2;
    Send_Byte( cmd & 0xff );
    if( I2C_WaitAck() == 1 )return 3;

    //如不使用超时判断，很容易数据错乱
    do
    {
        //超时判断
        i++;
        if( i > 20 ) return 4;
        delay_ms(2);

        IIC_Start();
        Send_Byte((0X44 << 1) | 1 );//读
        delay_us(5);

    }while(I2C_WaitAck() == 1);  // 收到应答则退出

    //温度高8位
    buff[0] = Read_Byte();
    IIC_Send_Ack(0);
    //温度低8位
    buff[1] = Read_Byte();
    IIC_Send_Ack(0);
    //温度CRC校验值
    buff[2] = Read_Byte();
    IIC_Send_Ack(0);
    //湿度高8位
    buff[3] = Read_Byte();
    IIC_Send_Ack(0);
    //湿度低8位
    buff[4] = Read_Byte();
    IIC_Send_Ack(0);
    //湿度CRC校验值
    buff[5] = Read_Byte();
    IIC_Send_Ack(1);

    IIC_Stop();

#if SHT30_DEBUG
    printf("buff[0] = %x\r\n",buff[0]);
    printf("buff[1] = %x\r\n",buff[1]);
    printf("buff[2] = %x\r\n",buff[2]);
    printf("buff[3] = %x\r\n",buff[3]);
    printf("buff[4] = %x\r\n",buff[4]);
    printf("buff[5] = %x\r\n",buff[5]);
#endif

    // 校验成功
    // 计算温度值
    data_16 =(buff[0]<<8) | buff[1];
    *temp = (data_16/65535.0)*175.0 - 45;
    // 计算湿度值
    data_16 = 0;
    data_16 =(buff[3]<<8) | buff[4];
    *humi = (data_16/65535.0) * 100.0;

    return 0;
}


/******************************************************************
 * 函 数 名 称：SHT30_Init
 * 函 数 说 明：SHT30初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void SHT30_Init(void)
{
    SCL(1);
    SDA(1);
    delay_ms(100); // 等待传感器稳定

    //设置周期模式命令
    if(SHT30_Write_mode(0x2130))//每1秒一次高重复测量(需要在周期模式下才有用)
        printf("SHT30_Write_mode Failed!!!\r\n");
    else
        printf("SHT30_Write_mode Succeed!!!\r\n");
}

