/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_ads1115.h"

/******************************************************************
 * 函 数 名 称：ADS1115_Init
 * 函 数 说 明：对ADS1115初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：1100_0010_1000_0011  WriteADS1115(0x01,0xc2,0x83);
******************************************************************/
void ADS1115_Init(void)
{
    SCL(1);
    SDA(1);

    delay_ms(100); //等待ADS1115上电稳定

	//写入配置参数
	WriteADS1115(0x01,0xC2,0x83);
}

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
 * 函 数 说 明：主机发送应答
 * 函 数 形 参：0应答  1非应答
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
 * 函 数 名 称：IIC_Wait_Ack
 * 函 数 说 明：等待从机应答
 * 函 数 形 参：无
 * 函 数 返 回：1=无应答   0=有应答
 * 作       者：LC
 * 备       注：无
******************************************************************/

unsigned char IIC_Wait_Ack(void)
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
void IIC_Write(unsigned char dat)
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
 * 函 数 名 称：IIC_Read
 * 函 数 说 明：IIC读1个字节
 * 函 数 形 参：无
 * 函 数 返 回：读出的1个字节数据
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned char IIC_Read(void)
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
 * 函 数 名 称：WriteADS1115
 * 函 数 说 明：向ADS1115的add地址写入dat数据
 * 函 数 形 参：add写入寄存器地址 dat_H写入的高8位数据  dat_L写入的低8位数据
 * 函 数 返 回：0写入成功 1写入器件地址无应答  2写入寄存器地址无应答
 *              3写入高8位数据无应答  4写入低8位数据无应答
 * 作       者：LC
 * 备       注：器件地址=0X90
******************************************************************/
uint8_t WriteADS1115(uint8_t add,uint8_t dat_H,uint8_t dat_L)
{
	IIC_Start();
	IIC_Write(0x90);

	if( IIC_Wait_Ack() == 1 )
	{
		printf("error 1\r\n");
		return 1;
	}

	IIC_Write(add);

	if( IIC_Wait_Ack() == 1 )
	{
		printf("error 2\r\n");
		return 2;
	}

	IIC_Write(dat_H);
	IIC_Wait_Ack();
	IIC_Write(dat_L);
	IIC_Wait_Ack();
	IIC_Stop();

	return (0);
}



/******************************************************************
 * 函 数 名 称：ReadADS1115
 * 函 数 说 明：读取ADS1115的数据
 * 函 数 形 参：add读取的寄存器地址
 * 函 数 返 回：-1-读取失败  其他-读取成功
 * 作       者：LC
 * 备       注：无
******************************************************************/
float ReadADS1115(unsigned char add)
{
	int i =0;
	unsigned char dat[2]={0};
	unsigned int num = 0;
	float ret=0;

	IIC_Start();//起始信号
	IIC_Write(0x90);//器件地址+写

	if( IIC_Wait_Ack() == 1 )
		return -1;

	IIC_Write(add);//寄存器地址

	if( IIC_Wait_Ack() == 1 )
		return -1;
	do{
		//超时判断
		i++;
		if( i > 20 ) return -1;
		delay_ms(1);
		IIC_Start();//重新发送起始信号
		IIC_Write(0x91);//器件地址+读
	}while(IIC_Wait_Ack() == 1);

	dat[0]=IIC_Read();//读高8位数据
	IIC_Send_Ack(0);//应答
	dat[1]=IIC_Read();//读低8位数据
	IIC_Send_Ack(1);//非应答
	IIC_Stop();//发送停止信号
    //数据整合
	num =  ((dat[0]<<8) | (dat[1]));

    // 数值计算取决于PGA配置
    // 2的15次方=32768
    // 设置的最大量程4.096
    //  if(num>32768)
    //      ret=((float)(65535-num)/32768.0)*4.096;
    //  else
    //      ret=((float)num/32768.0)*4.096;

    // 分辨率计算：测量电压范围/(2^AD位数-1)
    // 分辨率= 4.096/2^15=0.000125
    // 电压= 采集到的ADC值 * 分辨率
	if(num>32768)
		ret=(65535-num)*0.000125;
	else
		ret=num*0.000125;

	return ret;
}


