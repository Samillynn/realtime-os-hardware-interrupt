#ifndef RPI_H
#define RPI_H

#include "common.h"

#define CHANNEL_GTK_TERM 0
#define CHANNEL_MARKLIN  1

void init_gpio();
void init_spi(u32 channel);
void init_uart(u32 spi_channel);
char uart_getc(usize spi_channel, usize uart_channel);
void uart_putc(usize spi_channel, usize uart_channel, char c);
void uart_puts(usize spi_channel, usize uart_channel, const cstring buffer, usize buffer_len);
void uart_printf(usize spi_channel, usize uart_channel, cstring fmt, ...);

#endif
