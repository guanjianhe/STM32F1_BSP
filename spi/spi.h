#ifndef SPI_H
#define SPI_H
#include "stm32f10x.h"

extern void spi_init(void);
extern uint8_t spi_xchg(uint8_t dat);
extern void spi_readmulti(uint8_t* buff, uint16_t btr);
extern void spi_writemulti(const uint8_t* buff, uint16_t btx);
extern void spi_fclkslow(void);
extern void spi_fclkfast(void);

#endif
