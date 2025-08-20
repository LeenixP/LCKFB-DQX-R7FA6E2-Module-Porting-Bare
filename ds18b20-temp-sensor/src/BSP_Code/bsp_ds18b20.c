
/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_ds18b20.h"
#include "stdio.h"


/******************************************************************
 * 函 数 名 称：bsp_ds18b20_Init
 * 函 数 说 明：MLX90614的初始化
 * 函 数 形 参：无
 * 函 数 返 回：1未检测到器件   0检测到器件
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
char DS18B20_Init(void)
{
    char ret;
    DQ_OUT();//设置为输出模式

    DQ(1);
    delay_ms(100); // 等待传感器稳定

    ret = DS18B20_Check();//检测器件是否存在
    if (ret) {
        printf("\r\nDS18B20 Check Failed!!\r\n");
    } else {
        printf("\r\nDS18B20 Check Successful!!\r\n");
    }
    return ret;
}



/******************************************************************
 * 函 数 名 称：DS18B20_Read_Byte
 * 函 数 说 明：从DS18B20读取一个字节
 * 函 数 形 参：无
 * 函 数 返 回：读取到的字节数据
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i=0,dat=0;

    for (i=0;i<8;i++)
    {
        DQ_OUT();//设置为输入模式
        DQ(0); //拉低
        delay_us(2);
        DQ(1); //释放总线
        DQ_IN();//设置为输入模式
        delay_us(12);
        dat>>=1;
        if( DQ_GET() )
        {
            dat=dat|0x80;
        }
        delay_us(50);
     }
     return dat;
}

/******************************************************************
 * 函 数 名 称：DS18B20_Write_Byte
 * 函 数 说 明：写一个字节到DS18B20
 * 函 数 形 参：dat：要写入的字节
 * 函 数 返 回：无
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
void DS18B20_Write_Byte(uint8_t dat)
{
        uint8_t i;
        DQ_OUT();//设置输出模式
        for (i=0;i<8;i++)
        {
            if ( (dat&0x01) ) //写1
            {
                DQ(0);
                delay_us(10);
                DQ(1);
                delay_us(60);
            }
            else //写0
            {
                DQ(0);//拉低60us
                delay_us(60);
                DQ(1);//释放总线
                delay_us(10);
            }
            dat=dat>>1;//传输下一位
        }
}



/******************************************************************
 * 函 数 名 称：DS18B20_Check
 * 函 数 说 明：检测DS18B20是否存在
 * 函 数 形 参：无
 * 函 数 返 回：1:未检测到DS18B20的存在  0:存在
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
uint8_t DS18B20_Check(void)
{
      uint8_t timeout=0;
      //复位DS18B20
      DQ_OUT(); //设置为输出模式
      DQ(0); //拉低DQ
      delay_us(750); //拉低750us
      DQ(1); //拉高DQ
      delay_us(15);  //15us

      //设置为输入模式
      DQ_IN();
      //等待拉低，拉低说明有应答
      while ( DQ_GET() &&timeout<200)
      {
              timeout++;//超时判断
              delay_us(1);
      };
      //设备未应答
      if(timeout>=200)
              return 1;
      else
              timeout=0;

      //等待18B20释放总线
      while ( !DQ_GET() &&timeout<240)
      {
              timeout++;//超时判断
              delay_us(1);
      };
      //释放总线失败
      if(timeout>=240)
              return 1;

      return 0;
}


/******************************************************************
 * 函 数 名 称：DS18B20_GetTemperture
 * 函 数 说 明：从ds18b20得到温度值
 * 函 数 形 参：无
 * 函 数 返 回：温度数据
 * 作       者：LCKFB
 * 备       注：无
******************************************************************/
float DS18B20_GetTemperture(void)
{
    uint16_t temp;
    uint8_t dataL=0,dataH=0;
    float value;

    if(DS18B20_Check())           //查询是否有设备应答
        printf("\r\nDS18B20_Check ERROR!\r\n");

    delay_ms(5); // 天猛星80MHz必须加这个延时

    DS18B20_Write_Byte(0xcc);   //对总线上所有设备进行寻址
    DS18B20_Write_Byte(0x44);   //启动温度转换

    if(DS18B20_Check())
        printf("\r\nDS18B20_Check ERROR!\r\n");

    delay_ms(5); // 天猛星80MHz必须加这个延时

    DS18B20_Write_Byte(0xcc);   //对总线上所有设备进行寻址
    DS18B20_Write_Byte(0xbe);   //读取数据命令

    dataL=DS18B20_Read_Byte();  //LSB
    dataH=DS18B20_Read_Byte();  //MSB
    temp=(dataH<<8)+dataL;//整合数据


    if(dataH&0X80)//高位为1，说明是负温度
    {
        temp=(~temp)+1;
        value=temp*(-0.0625);
    }
    else
    {
        value=temp*0.0625;
    }
    return value;
}

