#include <arch.h>

void bluetooth_init()
{
    uint32_t a = *READ_IO(BT_UART_BASE + UART_fcr);
    delay();
    *WRITE_IO(BT_UART_BASE + UART_fcr) = a | 0X00000001;
    delay();
    *WRITE_IO(BT_UART_BASE + UART_lcr) = 0X00000080; // LCR[7] is 1
    delay();
    *WRITE_IO(BT_UART_BASE + UART_dll) = 69;
    delay();
    *WRITE_IO(BT_UART_BASE + UART_dlm) = 0X00000001;
    delay();
    *WRITE_IO(BT_UART_BASE + UART_lcr) = 0X00000003;
    delay();
    *WRITE_IO(BT_UART_BASE + UART_ier) = 0X00000001;
    delay();
}

