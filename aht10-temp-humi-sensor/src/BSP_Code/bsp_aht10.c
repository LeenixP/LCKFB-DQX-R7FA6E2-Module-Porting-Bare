/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_aht10.h"
#include "stdio.h"


/******************************************************************
 * 函 数 名 称：IIC_Start
 * 函 数 说 明：IIC起始时序
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void IIC_Start(void)
{
    SDA_OUT();

    SDA(1);
    SCL(1);
    delay_us(4);

    SDA(0);
    delay_us(4);
    SCL(0);

}
/******************************************************************
 * 函 数 名 称：IIC_Stop
 * 函 数 说 明：IIC停止信号
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void IIC_Stop(void)
{
        SDA_OUT();

        SCL(0);
        SDA(0);
        delay_us(4);

        SCL(1);
        SDA(1);
        delay_us(4);

}

/******************************************************************
 * 函 数 名 称：IIC_Send_Ack
 * 函 数 说 明：主机发送应答或者非应答信号
 * 函 数 形 参：0发送应答  1发送非应答
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void IIC_Send_Ack(unsigned char ack)
{
        SDA_OUT();
        SCL(0);
        SDA(0);
        delay_us(2);
        if(!ack) SDA(0);
        else     SDA(1);
        SCL(1);
        delay_us(2);
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
static unsigned char I2C_WaitAck(void)
{
        char ack = 0;
        unsigned char ack_flag = 10;

        SDA(1);
        delay_us(1);
        SCL(1);
        delay_us(1);

        SDA_IN();
        delay_us(2);

        while( (SDA_GET()==1) && ( ack_flag ) )
        {
                ack_flag--;
                delay_us(3);
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
                SDA(0);
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
static void Send_Byte(uint8_t dat)
{
        int i = 0;
        SDA_OUT();
        SCL(0);//拉低时钟开始数据传输

        for( i = 0; i < 8; i++ )
        {
                SDA( (dat & 0x80) >> 7 );
                delay_us(1);
                SCL(1);
                delay_us(2);
                SCL(0);
                delay_us(2);
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
static uint8_t Read_Byte(void)
{
        unsigned char i,receive=0;
        SDA_IN();//SDA设置为输入
        for(i=0;i<8;i++ )
        {
                SCL(0);
                delay_us(2);
                SCL(1);
                delay_us(2);
                receive<<=1;
                if( SDA_GET() )
                {
                      receive|=1;
                }
                delay_us(1);
        }
        SCL(0);
        return receive;
}

/******************************************************************
 * 函 数 名 称：AHT10Reset
 * 函 数 说 明：软件复位
 * 函 数 形 参：
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
static void AHT10Reset(void)
{
    IIC_Start();
    Send_Byte(0x70);
    I2C_WaitAck();
    Send_Byte(0xba);
    I2C_WaitAck();
    IIC_Stop();
}


/**********************************************************
 * 函 数 名 称：AHT10_Read
 * 函 数 功 能：读取AHT10的温湿度数据
 * 传 入 参 数：温度地址，湿度地址
 * 函 数 返 回：0读取成功
 * 作       者：LC
 * 备       注：无
**********************************************************/
static uint8_t AHT10_Read(float *Temperature, float *Humidity)
{
    char timeout = 0;
    unsigned char buff[6] = {0};

    IIC_Start();//起始信号
    Send_Byte(0X38<<1 | 0);     //器件地址+写命令
    I2C_WaitAck();              //等待传感器应答
    Send_Byte(0XAC);            //寄存器地址：触发测量命令
    I2C_WaitAck();//等待传感器应答

    Send_Byte(0X33);//发送数据位
    I2C_WaitAck();//等待传感器应答

    Send_Byte(0X00);//发送数据位
    I2C_WaitAck();//等待传感器应答

    IIC_Stop();        //停止信号

    do{
            delay_ms(1);//超时判断，等待传感器采集数据
            timeout++;
            IIC_Start();//重新发送起始信号
            Send_Byte(0X38<<1  | 1);//器件地址+写命令
    }while( I2C_WaitAck() == 1 && timeout < 5 );

    if(timeout > 5)
        return 1;


    buff[0] = Read_Byte();//读取状态字
    IIC_Send_Ack(0);//主机发送应答

    buff[1] = Read_Byte();//湿度高8位数据
    IIC_Send_Ack(0);//主机发送应答

    buff[2] = Read_Byte();//湿度低8位数据
    IIC_Send_Ack(0);//主机发送应答

    buff[3] = Read_Byte();
    IIC_Send_Ack(0);//主机发送应答

    buff[4] = Read_Byte();
    IIC_Send_Ack(0);//主机发送应答

    buff[5] = Read_Byte();
    IIC_Send_Ack(1);//主机发送应答

    IIC_Stop();//停止信号

    delay_ms(20);

    //高位在前
    uint32_t humidity_raw = ( (uint32_t)buff[1] << 12 ) |
                            ( (uint32_t)buff[2] << 4  ) |
                            ( buff[3] >> 4 );
    *Humidity = humidity_raw / 1048576.0f * 100.0f;

    uint32_t temp_raw = ( (uint32_t)(buff[3] & 0x0F) << 16 ) |
                        ( (uint32_t)buff[4] << 8 ) |
                        buff[5];
    *Temperature = (temp_raw / 1048576.0f) * 200.0f - 50.0f;

    AHT10Reset();        // AHT10复位
    AHT10_Init();        // GPIO重新初始化

    return 0;
}


/******************************************************************
 * 函 数 名 称：AHT10_Init
 * 函 数 说 明：AHT10的初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void AHT10_Init(void)
{
    SCL(1);
    SDA(1);

    delay_ms(100);//延时让传感器稳定

    IIC_Start();
    Send_Byte(0x70);  //获取状态
    //初始化校准
    Send_Byte(0xe1);
    Send_Byte(0x08);
    Send_Byte(0x00);
    IIC_Stop();
    delay_ms(50);//延时50ms让传感器稳定
}



/**********************************************************
 * 函 数 名 称：AHT10_Get_TempHumi
 * 函 数 功 能：获取AHT10采集后的温度湿度数据
 * 传 入 参 数：Temperature - 指向存储温度的浮点数指针
 *             Humidity    - 指向存储湿度的浮点数指针
 * 函 数 返 回：0 - 成功，非0 - 失败（具体错误码）
 * 作       者：LCKFB
 * 备       注：调用前必须确保已经执行了数据采集函数
**********************************************************/
uint8_t AHT10_Get_TempHumi(float *Temperature, float *Humidity)
{
    float Temp_Temperature = 0;
    float Temp_Humidity    = 0;

    if(AHT10_Read(&Temp_Temperature, &Temp_Humidity) != 0)
    {
        printf("AHT10 Read Error!!\r\n");
        return 1;
    }

    if(Temperature != NULL)
    {
        *Temperature = Temp_Temperature;
    }
    else
    {
        printf("Temperature pointer is NULL.\r\n");
    }

    if(Humidity != NULL)
    {
        *Humidity = Temp_Humidity;
    }
    else
    {
        printf("Humidity pointer is NULL.\r\n");
    }

    return 0;
}

