/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_tcs34725.h"
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

/*******************************************************************************
 * @brief Writes data to a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes to write.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_I2C_Write(uint8_t slaveAddress, uint8_t* dataBuffer,uint8_t bytesNumber, uint8_t stopBit)
{
        uint8_t i = 0;

        IIC_Start();
        Send_Byte((slaveAddress << 1) | 0x00);           //发送从机地址写命令
        I2C_WaitAck();
        for(i = 0; i < bytesNumber; i++)
        {
                Send_Byte(*(dataBuffer + i));
                I2C_WaitAck();
        }
        if(stopBit == 1) IIC_Stop();
}
/*******************************************************************************
 * @brief Reads data from a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes to read.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
*******************************************************************************/
void TCS34725_I2C_Read(uint8_t slaveAddress, uint8_t* dataBuffer, uint8_t bytesNumber, uint8_t stopBit)
{
      uint8_t i = 0;

      IIC_Start();
      Send_Byte((slaveAddress << 1) | 0x01);           //发送从机地址读命令
      I2C_WaitAck();
      for(i = 0; i < bytesNumber; i++)
      {
            if(i == bytesNumber - 1)
            {
                    *(dataBuffer + i) = Read_Byte();//读取的最后一个字节发送NACK
                    IIC_Send_Ack(1);
            }
            else
            {
                    *(dataBuffer + i) = Read_Byte();
                    IIC_Send_Ack(0);
            }
      }
      if(stopBit == 1) IIC_Stop();
}
/*******************************************************************************
 * @brief Writes data into TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes that will be sent.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Write(uint8_t subAddr, uint8_t* dataBuffer, uint8_t bytesNumber)
{
    uint8_t sendBuffer[10] = {0, };
    uint8_t byte = 0;

    sendBuffer[0] = subAddr | TCS34725_COMMAND_BIT;
    for(byte = 1; byte <= bytesNumber; byte++)
    {
        sendBuffer[byte] = dataBuffer[byte - 1];
    }
        TCS34725_I2C_Write(TCS34725_ADDRESS, sendBuffer, bytesNumber + 1, 1);
}
/*******************************************************************************
 * @brief Reads data from TCS34725 registers, starting from the selected
 *        register address pointer.
 *
 * @param subAddr - The selected register address pointer.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes that will be read.
 *
 * @return None.
*******************************************************************************/
void TCS34725_Read(uint8_t subAddr, uint8_t* dataBuffer, uint8_t bytesNumber)
{
        subAddr |= TCS34725_COMMAND_BIT;

        TCS34725_I2C_Write(TCS34725_ADDRESS, (uint8_t*)&subAddr, 1, 0);
        TCS34725_I2C_Read(TCS34725_ADDRESS, dataBuffer, bytesNumber, 1);
}
/*******************************************************************************
 * @brief TCS34725设置积分时间
 *
 * @return None
*******************************************************************************/
void TCS34725_SetIntegrationTime(uint8_t time)
{
        TCS34725_Write(TCS34725_ATIME, &time, 1);
}
/*******************************************************************************
 * @brief TCS34725设置增益
 *
 * @return None
*******************************************************************************/
void TCS34725_SetGain(uint8_t gain)
{
        TCS34725_Write(TCS34725_CONTROL, &gain, 1);
}
/*******************************************************************************
 * @brief TCS34725使能
 *
 * @return None
*******************************************************************************/
void TCS34725_Enable(void)
{
        uint8_t cmd = TCS34725_ENABLE_PON;

        TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
        cmd = TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN;
        TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
        //delay_s(600000);//delay_ms(3);//延时应该放在设置AEN之后
}
/*******************************************************************************
 * @brief TCS34725失能
 *
 * @return None
*******************************************************************************/
void TCS34725_Disable(void)
{
        uint8_t cmd = 0;

        TCS34725_Read(TCS34725_ENABLE, &cmd, 1);
        cmd = cmd & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
        TCS34725_Write(TCS34725_ENABLE, &cmd, 1);
}
/*******************************************************************************
 * @brief TCS34725初始化
 *
 * @return ID - ID寄存器中的值
*******************************************************************************/
uint8_t TCS34725_Init(void)
{
    uint8_t id=0;
    
    SCL(1);
    SDA(1);

    delay_ms(100); // 等待传感器稳定

    TCS34725_Read(TCS34725_ID, &id, 1);  //TCS34725 的 ID 是 0x44 可以根据这个来判断是否成功连接,0x4D是TCS34727;
    if(id==0x4D | id==0x44)
    {
        TCS34725_SetIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
        TCS34725_SetGain(TCS34725_GAIN_1X);
        TCS34725_Enable();
        return 0;
    }
    else
    {
        return 1;
    }
}
/*******************************************************************************
 * @brief TCS34725获取单个通道数据
 *
 * @return data - 该通道的转换值
*******************************************************************************/
uint16_t TCS34725_GetChannelData(uint8_t reg)
{
    uint8_t tmp[2] = {0,0};
    uint16_t data;

    TCS34725_Read(reg, tmp, 2);
    data = (tmp[1] << 8) | tmp[0];

    return data;
}
/*******************************************************************************
 * @brief TCS34725获取各个通道数据
 *
 * @return 0 - 转换完成，数据可用
 *         1 - 转换未完成，数据不可用
*******************************************************************************/
uint8_t TCS34725_GetRawData(COLOR_RGBC *rgbc)
{
    uint8_t status = TCS34725_STATUS_AVALID;

    TCS34725_Read(TCS34725_STATUS, &status, 1);

    if(status & TCS34725_STATUS_AVALID)
    {
        rgbc->c = TCS34725_GetChannelData(TCS34725_CDATAL);
        rgbc->r = TCS34725_GetChannelData(TCS34725_RDATAL);
        rgbc->g = TCS34725_GetChannelData(TCS34725_GDATAL);
        rgbc->b = TCS34725_GetChannelData(TCS34725_BDATAL);
        return 0;
    }
    return 1;
}
/******************************************************************************/
//RGB转HSL
void RGBtoHSL(COLOR_RGBC *Rgb, COLOR_HSL *Hsl)
{
        uint8_t maxVal,minVal,difVal;
        uint8_t r = Rgb->r*100/Rgb->c;   //[0-100]
        uint8_t g = Rgb->g*100/Rgb->c;
        uint8_t b = Rgb->b*100/Rgb->c;

        maxVal = max3v(r,g,b);
        minVal = min3v(r,g,b);
        difVal = maxVal-minVal;

        //计算亮度
        Hsl->l = (maxVal+minVal)/2;   //[0-100]

        if(maxVal == minVal)//若r=g=b,灰度
        {
            Hsl->h = 0;
            Hsl->s = 0;
        }
        else
        {
            //计算色调
            if(maxVal==r)
            {
                if(g>=b)
                    Hsl->h = 60*(g-b)/difVal;
                else
                    Hsl->h = 60*(g-b)/difVal+360;
            }
            else
            {
                if(maxVal==g)Hsl->h = 60*(b-r)/difVal+120;
                else
                    if(maxVal==b)Hsl->h = 60*(r-g)/difVal+240;
            }

            //计算饱和度
            if(Hsl->l<=50)Hsl->s=difVal*100/(maxVal+minVal);  //[0-100]
            else
                Hsl->s=difVal*100/(200-(maxVal+minVal));
        }
}
/******************************************************************************/

