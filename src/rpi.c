#include "rpi.h"
#include "bcm2711.h"
#include "utilities.h"
#include <stdarg.h>

#define MAX_BUFFER_SIZE 128

struct GPIO {
  u32 GPFSEL[6];
  u32 : 32;
  u32 GPSET0;
  u32 GPSET1;
  u32 : 32;
  u32 GPCLR0;
  u32 GPCLR1;
  u32 : 32;
  u32 GPLEV0;
  u32 GPLEV1;
  u32 : 32;
  u32 GPEDS0;
  u32 GPEDS1;
  u32 : 32;
  u32 GPREN0;
  u32 GPREN1;
  u32 : 32;
  u32 GPFEN0;
  u32 GPFEN1;
  u32 : 32;
  u32 GPHEN0;
  u32 GPHEN1;
  u32 : 32;
  u32 GPLEN0;
  u32 GPLEN1;
  u32 : 32;
  u32 GPAREN0;
  u32 GPAREN1;
  u32 : 32;
  u32 GPAFEN0;
  u32 GPAFEN1;
  u32 _unused[21];
  u32 PUP_PDN_CNTRL_REG[4];
};

struct AUX {
  u32 IRQ;
  u32 ENABLES;
};

struct SPI {
  u32 CNTL0;       // Control register 0
  u32 CNTL1;       // Control register 1
  u32 STAT;        // Status
  u32 PEEK;        // Peek
  u32 _unused[4];
  u32 IO_REGa;     // Data
  u32 IO_REGb;     // Data
  u32 IO_REGc;     // Data
  u32 IO_REGd;     // Data
  u32 TXHOLD_REGa; // Extended Data
  u32 TXHOLD_REGb; // Extended Data
  u32 TXHOLD_REGc; // Extended Data
  u32 TXHOLD_REGd; // Extended Data
};

static volatile struct GPIO* const gpio  =  (struct GPIO*)(GPIO_BASE);
static volatile struct AUX*  const aux   =  (struct AUX*)(AUX_BASE);
static volatile struct SPI*  const spi[] = { (struct SPI*)(AUX_BASE + 0x80), (struct SPI*)(AUX_BASE + 0xC0) };

static void setup_gpio(u32 pin, u32 setting, u32 resistor) {
  u32 reg = pin / 10;
  u32 shift = (pin % 10) * 3;
  u32 status = gpio->GPFSEL[reg];        // read status
  status &= ~(7u << shift);              // clear bits
  status |=  (setting << shift);         // set bits
  gpio->GPFSEL[reg] = status;            // write back

  reg   =  pin / 16;
  shift = (pin % 16) * 2;
  status = gpio->PUP_PDN_CNTRL_REG[reg]; // read status
  status &= ~(3u << shift);              // clear bits
  status |=  (resistor << shift);        // set bits
  gpio->PUP_PDN_CNTRL_REG[reg] = status; // write back
}

void init_gpio() {
  setup_gpio(18, GPIO_ALTFN4, GPIO_NONE);
  setup_gpio(19, GPIO_ALTFN4, GPIO_NONE);
  setup_gpio(20, GPIO_ALTFN4, GPIO_NONE);
  setup_gpio(21, GPIO_ALTFN4, GPIO_NONE);
}

void init_spi(u32 channel) {
  u32 reg = aux->ENABLES;
  reg |= (2 << channel);
  aux->ENABLES = reg;
  spi[channel]->CNTL0 = SPI_CNTL0_Clear_FIFOs;
  u32 speed = (700000000 / (2 * 0x400000)) - 1; // for maximum bitrate 0x400000
  spi[channel]->CNTL0 = (speed << SPI_CNTL0_SPEED_SHIFT)
                      | SPI_CNTL0_VAR_WIDTH
                      | SPI_CNTL0_Enable
                      | SPI_CNTL0_In_Rising
                      | SPI_CNTL0_SO_MSB_FST;
  spi[channel]->CNTL1 = SPI_CNTL1_SI_MSB_FST;
}

static void spi_send_recv(u32 channel, const char* sendbuf, usize sendlen, char* recvbuf, usize recvlen) {
  usize sendidx = 0;
  usize recvidx = 0;
  while (sendidx < sendlen || recvidx < recvlen) {
    u32 data = 0;
    usize count = 0;

    // prepare write data
    for (; sendidx < sendlen && count < 24; sendidx += 1, count += 8) {
      data |= (sendbuf[sendidx] << (16 - count));
    }
    data |= (count << 24);

    // always need to write something, otherwise no receive
    while (spi[channel]->STAT & SPI_STAT_TX_FULL) asm volatile("yield");
    if (sendidx < sendlen) {
      spi[channel]->TXHOLD_REGa = data; // keep chip-select active, more to come
    } else {
      spi[channel]->IO_REGa = data;
    }

    // read transaction
    while (spi[channel]->STAT & SPI_STAT_RX_EMPTY) asm volatile("yield");
    data = spi[channel]->IO_REGa;

    // process data, if needed, assume same byte count in transaction
    usize max = (recvlen - recvidx) * 8;
    if (count > max) count = max;
    for (; count > 0; recvidx += 1, count -= 8) {
      recvbuf[recvidx] = (data >> (count - 8)) & 0xFF;
    }
  }
}

static void uart_write_register(usize spi_channel, usize uart_channel, char reg, char data) {
  char req[2] = {0};
  req[0] = (uart_channel << UART_CHANNEL_SHIFT) | (reg << UART_ADDR_SHIFT);
  req[1] = data;
  spi_send_recv(spi_channel, req, 2, NULL, 0);
}

static char uart_read_register(usize spi_channel, usize uart_channel, char reg) {
  char req[2] = {0};
  char res[2] = {0};
  req[0] = (uart_channel << UART_CHANNEL_SHIFT) | (reg << UART_ADDR_SHIFT) | UART_READ_ENABLE;
  spi_send_recv(spi_channel, req, 2, res, 2);
  return res[1];
}

static void uart_init_channel(usize spi_channel, usize uart_channel, usize baud_rate, usize stop_bits) {
  // set baud rate
  uart_write_register(spi_channel, uart_channel, UART_LCR, UART_LCR_DIV_LATCH_EN);
  uint32_t bauddiv = 14745600 / (baud_rate * 16);
  uart_write_register(spi_channel, uart_channel, UART_DLH, (bauddiv & 0xFF00) >> 8);
  uart_write_register(spi_channel, uart_channel, UART_DLL, (bauddiv & 0x00FF));

  // set stop bits
  uart_write_register(spi_channel, uart_channel, UART_LCR, stop_bits | 0x3);

  // clear and enable fifos, (wait since clearing fifos takes time)
  uart_write_register(spi_channel, uart_channel, UART_FCR, UART_FCR_RX_FIFO_RESET | UART_FCR_TX_FIFO_RESET | UART_FCR_FIFO_EN);
  for (int i = 0; i < 65535; ++i) asm volatile("yield");
}

void init_uart(uint32_t spi_channel) {
  uart_write_register(spi_channel, 0, UART_IOControl, UART_IOControl_RESET); // resets both channels
  uart_init_channel(spi_channel, 0, 115200, UART_LCR_1_STOP_BITS);
  uart_init_channel(spi_channel, 1,   2400, UART_LCR_2_STOP_BITS);
}

char uart_getc(usize spi_channel, usize uart_channel) {
  while (uart_read_register(spi_channel, uart_channel, UART_RXLVL) == 0) asm volatile("yield");
  return uart_read_register(spi_channel, uart_channel, UART_RHR);
}

void uart_putc(usize spi_channel, usize uart_channel, char c) {
  while (uart_read_register(spi_channel, uart_channel, UART_TXLVL) == 0) asm volatile("yield");
  uart_write_register(spi_channel, uart_channel, UART_THR, c);
}

void uart_puts(usize spi_channel, usize uart_channel, const cstring buffer, usize buffer_len) {
  static const size_t max = 32;
  char temp[max];
  temp[0] = (uart_channel << UART_CHANNEL_SHIFT) | (UART_THR << UART_ADDR_SHIFT);
  size_t tlen = uart_read_register(spi_channel, uart_channel, UART_TXLVL);
  if (tlen > max) tlen = max;
  for (size_t bidx = 0, tidx = 1;;) {
    if (tidx < tlen && bidx < buffer_len) {
      temp[tidx] = buffer[bidx];
      bidx += 1;
      tidx += 1;
    } else {
      spi_send_recv(spi_channel, temp, tidx, NULL, 0);
      if (bidx == buffer_len) break;
      tlen = uart_read_register(spi_channel, uart_channel, UART_TXLVL);
      if (tlen > max) tlen = max;
      tidx = 1;
    }
  }
}

void uart_printf(usize spi_channel, usize uart_channel, cstring fmt, ...) {
  va_list vl;
  va_start(vl, fmt);

  for (char* ptr = fmt; *ptr != '\0'; ptr ++) {
    if (*ptr == '%') {
      ptr ++;
      switch (*ptr) {
      case 's': {
	cstring buffer = va_arg(vl, cstring);
	uart_puts(spi_channel, uart_channel, buffer, strlen(buffer));
	break;
      }
      case 'c': {
	char buffer = va_arg(vl, int);
	uart_putc(spi_channel, uart_channel, buffer);
	break;
      }
      case 'd': {
	i32 number = va_arg(vl, i32);
	char buffer[MAX_BUFFER_SIZE];
	i32_to_cstring(number, buffer, 10);
	uart_puts(spi_channel, uart_channel, buffer, strlen(buffer));
	break;
      }
      case 'p': {
	void* value = va_arg(vl, void*);
	char buffer[MAX_BUFFER_SIZE];
	u64_to_cstring((u64)value, buffer, 16);
	uart_puts(spi_channel, uart_channel, buffer, strlen(buffer));
	break;
      }
      case 'u': {
	u32 number = va_arg(vl, u32);
	char buffer[MAX_BUFFER_SIZE];
	u32_to_cstring(number, buffer, 10);
	uart_puts(spi_channel, uart_channel, buffer, strlen(buffer));
	break;
      }
      case '%': {
	uart_putc(spi_channel, uart_channel, '%');
	break;
      }
      }
    } else {
      uart_putc(spi_channel, uart_channel, *ptr);
    }
  }

  va_end(vl);
}

