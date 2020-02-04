/*
 * Author: Aurelio Colosimo, 2020, MIT License
 */

#ifndef _I2C_H_
#define _I2C_H_

#include <basic.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <cbuf.h>

#define MINOR_I2C1 0
#define MINOR_I2C2 1
#define MINOR_I2C3 2
#define MINOR_I2C4 3

struct i2c_xfer_t {
	u8 addr;
	u8 *buf;
	size_t len;
	u8 dir;
	u8 nostop;
};

#endif /* _I2C_H_ */
