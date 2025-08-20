/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#ifndef BSP_CODE_UART_DEBUG_BSP_UART_H_
#define BSP_CODE_UART_DEBUG_BSP_UART_H_

#include "hal_data.h"
#include "stdio.h"
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>  // 添加这个头文件以获得struct stat定义
#include <stdint.h>

void UART0_Debug_Init(void);
void uart0_callback(uart_callback_args_t *p_args);

#endif /* BSP_CODE_UART_DEBUG_BSP_UART_H_ */
