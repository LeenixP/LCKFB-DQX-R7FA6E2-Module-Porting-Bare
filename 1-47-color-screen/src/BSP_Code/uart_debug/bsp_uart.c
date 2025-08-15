/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 文档网站：wiki.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 嘉立创社区问答：https://www.jlc-bbs.com/lckfb
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 */

#include "bsp_uart.h"

// 发送完成标志位
volatile bool Uart0_Send_Flag = false;
// 接收完成标志位
volatile bool Uart0_Receive_Flag = false;
// 写入的字节
uint32_t bytes = 1;

//调试串口 UART0 初始化
void UART0_Debug_Init(void)
{
    fsp_err_t err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);

    if (FSP_SUCCESS != err) {
           printf("串口初始化失败! \n");
           return;
       }
}

/* 串口中断回调 */
void uart0_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_RX_CHAR: //收到数据
        {
            R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t*) &(p_args->data), bytes);
            break;
        }
        case UART_EVENT_RX_COMPLETE:  //接收完整的事件
        {
            Uart0_Receive_Flag = true;
            break;
        }
        case UART_EVENT_TX_COMPLETE://发送完整的事件
        {
            Uart0_Send_Flag = true;
            break;
        }
        default:
        break;
    }
}


// 串口重定义
// 函数前置声明（仅需声明一次）
int __io_putchar(int ch);
int _write(int fd, char *pBuffer, int size);
int _close(int fd);
int _lseek(int fd, off_t ptr, int dir);
int _read(int fd, char *pBuffer, int size);
int _fstat(int fd, struct stat *pStat);
int _isatty(int fd);

// 弱符号实现（仅需实现一次）
__attribute__((weak)) int _close(int fd) {
    (void)fd;  // 忽略未使用参数
    return -1;
}

__attribute__((weak)) int _lseek(int fd, off_t ptr, int dir) {
    (void)fd; (void)ptr; (void)dir;
    return -1;
}

__attribute__((weak)) int _read(int fd, char *pBuffer, int size) {
    (void)fd; (void)pBuffer; (void)size;
    return 0;
}

__attribute__((weak)) int _fstat(int fd, struct stat *pStat) {
    (void)fd; (void)pStat;
    return -1;
}

__attribute__((weak)) int _isatty(int fd) {
    (void)fd;
    return 1;
}

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE {
    fsp_err_t err;
    err = R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)&ch, 1);
    if (FSP_SUCCESS != err) __BKPT();
    while (Uart0_Send_Flag == false);
    Uart0_Send_Flag = false;
    return ch;
}

int _write(int fd, char *pBuffer, int size) {
    (void)fd;  // 忽略未使用参数
    R_SCI_UART_Write (&g_uart0_ctrl, (uint8_t*) pBuffer, (uint32_t) size);
    while (Uart0_Send_Flag == false);
    Uart0_Send_Flag = false;
    return size;
}

