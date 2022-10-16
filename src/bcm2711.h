#ifndef BCM2711_H
#define BCM2711_H

#include "common.h"

#define MMIO_BASE         0xFE000000
#define GPIO_BASE         (MMIO_BASE + 0x200000)
#define SYSTEM_TIMER_BASE (MMIO_BASE + 0x3000)
#define AUX_BASE          (GPIO_BASE + 0x15000)

// ********************************************************************************
// GPIO

#define GPIO_INPUT  0x00
#define GPIO_OUTPUT 0x01
#define GPIO_ALTFN0 0x04
#define GPIO_ALTFN1 0x05
#define GPIO_ALTFN2 0x06
#define GPIO_ALTFN3 0x07
#define GPIO_ALTFN4 0x03
#define GPIO_ALTFN5 0x02

#define GPIO_NONE 0x00
#define GPIO_PUP  0x01
#define GPIO_PDP  0x02

#define SPI_CNTL0_DOUT_HOLD_SHIFT 12
#define SPI_CNTL0_CS_SHIFT        17
#define SPI_CNTL0_SPEED_SHIFT     20

#define SPI_CNTL0_POSTINPUT   0x00010000
#define SPI_CNTL0_VAR_CS      0x00008000
#define SPI_CNTL0_VAR_WIDTH   0x00004000
#define SPI_CNTL0_Enable      0x00000800
#define SPI_CNTL0_In_Rising   0x00000400
#define SPI_CNTL0_Clear_FIFOs 0x00000200
#define SPI_CNTL0_Out_Rising  0x00000100
#define SPI_CNTL0_Invert_CLK  0x00000080
#define SPI_CNTL0_SO_MSB_FST  0x00000040
#define SPI_CNTL0_MAX_SHIFT   0x0000003F

#define SPI_CNTL1_CS_HIGH_SHIFT 8

#define SPI_CNTL1_Keep_Input  0x00000001
#define SPI_CNTL1_SI_MSB_FST  0x00000002
#define SPI_CNTL1_Done_IRQ    0x00000040
#define SPI_CNTL1_TX_EM_IRQ   0x00000080

#define SPI_STAT_TX_FIFO_MASK 0xFF000000
#define SPI_STAT_RX_FIFO_MASK 0x00FF0000
#define SPI_STAT_TX_FULL      0x00000400
#define SPI_STAT_TX_EMPTY     0x00000200
#define SPI_STAT_RX_FULL      0x00000100
#define SPI_STAT_RX_EMPTY     0x00000080
#define SPI_STAT_BUSY         0x00000040
#define SPI_STAT_BIT_CNT_MASK 0x0000003F

// ********************************************************************************
// SPI

#define UART_RHR       0x00 // R
#define UART_THR       0x00 // W
#define UART_IER       0x01 // R/W
#define UART_IIR       0x02 // R
#define UART_FCR       0x02 // W
#define UART_LCR       0x03 // R/W
#define UART_MCR       0x04 // R/W
#define UART_LSR       0x05 // R
#define UART_MSR       0x06 // R
#define UART_SPR       0x07 // R/W
#define UART_TXLVL     0x08 // R
#define UART_RXLVL     0x09 // R
#define UART_IODir     0x0A // R/W
#define UART_IOState   0x0B // R/W
#define UART_IOIntEna  0x0C // R/W
#define UART_reserved  0x0D
#define UART_IOControl 0x0E // R/W
#define UART_EFCR      0x0F // R/W

#define UART_DLL       0x00 // R/W - only accessible when EFR[4] 1 and MCR[2] 1
#define UART_DLH       0x01 // R/W - only accessible when EFR[4] 1 and MCR[2] 1
#define UART_EFR       0x02 // ?   - only accessible when LCR is 0xBF
#define UART_TCR       0x06 // R/W - only accessible when EFR[4] 1 and MCR[2] 1
#define UART_TLR       0x07 // R/W - only accessible when EFR[4] 1 and MCR[2] 1

// UART flags
#define UART_CHANNEL_SHIFT           1
#define UART_ADDR_SHIFT              3
#define UART_READ_ENABLE             0x80
#define UART_FCR_TX_FIFO_RESET       0x04
#define UART_FCR_RX_FIFO_RESET       0x02
#define UART_FCR_FIFO_EN             0x01
#define UART_LCR_DIV_LATCH_EN        0x80
#define UART_LCR_1_STOP_BITS         0x00
#define UART_LCR_2_STOP_BITS         0x04
#define UART_EFR_ENABLE_ENHANCED_FNS 0x10
#define UART_IOControl_RESET         0x08

#endif
