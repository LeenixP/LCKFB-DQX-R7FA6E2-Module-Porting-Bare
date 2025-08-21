/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_relay.h"
#include "stdio.h"


/******************************************************************
 * 函 数 名 称：Set_Relay_Switch
 * 函 数 说 明：设置继电器状态
 * 函 数 形 参：0继电器吸合   1继电器断开
 * 函 数 返 回：
 * 作       者：LCKFB
 * 备       注：
******************************************************************/
void Set_Relay_Switch(unsigned char state)
{
    RELAY_OUT(state);
}

