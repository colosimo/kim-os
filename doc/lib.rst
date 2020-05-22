The *lib* library
=================

.. index:: CRC

In the folder lib are located some common and hw independent facilities for
the firmware development:

- ``basic.c``: some basic functions, replacement of what would be normally found
  in C standard libraries;

- ``cbuf.c``: an implementation of a circular buffer;

- ``crc.c``: at present, it only includes CRC16-CCIT algorithm;

- ``kprint.c``: the replacement of printf-like functions, including k_sprintf
  for string formatting in a buffer.

The header files (*.h*) for lib and kernel source code are mixed in include/
subfolder; so, for instance, ``include/basic.h`` contains the declarations for
the functions defined in ``lib/basic.c``

The file ``include/basic.h`` also contains more useful definition, e.g. those
to manipulate the registers:


.. code-block:: c

	/* 32 bits registers */
	static inline void wr32(volatile u32 *reg, u32 val) {*reg = val;}
	static inline u32 rd32(volatile u32 *reg) {return *reg;}
	static inline void or32(volatile u32 *reg, u32 val) {*reg |= val;}
	static inline void and32(volatile u32 *reg, u32 val) {*reg &= val;}
	static inline void clr32(volatile u32 *r, int nbit) {and32(r, ~(1 << nbit));}
	static inline void set32(volatile u32 *reg, int nbit) {or32(reg, 1 << nbit);}

	/* 16 bits registers */
	static inline void wr16(volatile u16 *reg, u16 val) {*reg = val;}
	static inline u16 rd16(volatile u16 *reg) {return *reg;}
	static inline void or16(volatile u16 *reg, u16 val) {*reg |= val;}
	static inline void and16(volatile u16 *reg, u16 val) {*reg &= val;}
	static inline void clr16(volatile u16 *reg, int nbit) {and16(reg, ~(1 << nbit));}
	static inline void set16(volatile u16 *reg, int nbit) {or16(reg, 1 << nbit);}

	/* 8 bits registers */
	static inline void wr8(volatile u8 *reg, u8 val) {*reg = val;}
	static inline u8 rd8(volatile u8 *reg) {return *reg;}
	static inline void or8(volatile u8 *reg, u8 val) {*reg |= val;}
	static inline void and8(volatile u8 *reg, u8 val) {*reg &= val;}
	static inline void clr8(volatile u8 *reg, int nbit) {and8(reg, ~(1 << nbit));}
	static inline void set8(volatile u8 *reg, int nbit) {or8(reg, 1 << nbit);}
