/*
 * Author: Aurelio Colosimo, 2019, MIT License
 */

#ifndef _SPI_H_
#define _SPI_H_

#include <basic.h>
#include <kim-io.h>
#include <kim-io-defs.h>

#define MINOR_SPI1 0
#define MINOR_SPI2 1
#define MINOR_SPI3 2

#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__

int spi_init(void);

struct spi_data_t {
	u16 cs_io; /* io(port, pin) of GPIO used as chip select */
	int fd;
};

#define declare_spi_dev(minor, _cs_io, _name) \
	static struct spi_data_t attr_used spi##minor = {.cs_io = _cs_io }; \
	declare_dev(MAJ_SOC_SPI, minor, &spi##minor, _name);

#endif /* _SPI_H_ */
