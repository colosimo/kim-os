I/O system: devices and drivers
===============================

.. index:: device, driver

The I/O model adopted by KIM is similar to the basic one in Linux, according
to the principle "everything is a file", with a major difference: there is
no need of *open* and *close* the device: everything is always available,
and is up to the programmer avoid any conflict; instead, the typical *read*,
*write* and *ioctl* functions are present with the same meaning and syntax
of common Unix systems.

The lack of open and close is thought to simplify the whole system, giving more
flexibility to the programmer.

Going directly in the source code, you can find the definition of device and
driver related structs in the file ``include/kim-io.h``:

.. code-block:: c

	/* dev id <-> (major, minor) conversions */
	#define dev_id(maj, min) ((uint16_t)((uint16_t)(maj) << 8 | (min)))
	#define dev_major(did) ((uint8_t)(did >> 8))
	#define dev_minor(did) ((uint8_t)(did & 0xff))

	#define declare_dev(maj, min, _priv, _name) \
			const k_dev_t attr_devs dev_##_name = { \
			.id = dev_id(maj, min), \
			.priv = _priv, \
			.name = str(_name), \
	};


.. code-block:: c

	struct __attribute__((packed)) k_drv_t {
		int (*init)(int fd);
		int (*read)(int fd, void *buf, size_t count);
		int (*write)(int fd, const void *buf, size_t count);
		int (*avail)(int fd);
		int (*ioctl)(int fd, int cmd, void *buf, size_t count);
		const char *name;
		uint8_t maj;
		uint8_t unused[3];
	};
	typedef struct k_drv_t k_drv_t;

	struct __attribute__((packed)) k_dev_t {
		uint16_t id;
		const struct k_drv_t *drv;
		void *priv;
		const char *name;
		u16 class_id;
	};
	typedef struct k_dev_t k_dev_t;

Each device has an ``id``, a 16 bit number actually composed of two parts:
the *major* and the *minor*, as well as it happens in Linux kernel space
drivers (I suggest a search on the Internet if you're not familiar with it).

The major identifies the *kind* of the device, whereas the minor identifies
each device of the same kind; in other words, the major is a property of the
driver, while the minor is what identifies each device associated
with the driver. A device uses the driver having the matching major number.

One more property of devices is the ``name`` field: it is a mnemonic,
human-friendly string which uniquely identifies each device.

The typical procedure to use a device is:

- obtain its *file descriptor* (``fd``) by calling k_fd_byname (for a search
  by name) or k_fd for a search with major and minor;

- call any of k_read, k_write, k_ioctl or k_avail as they are defined in
  ``include/kim-io.h``

To have a general idea about how to write your driver, I suggest a look at
``arch/arm/cpu-cortex-m4/soc-stm32f407xx/uart.c`` source file.

Devices and drivers must be statically *declared*, by using the proper
attributes. Typically, the driver is declared in the same file as its
callback functions are defined:

.. code-block:: c

	const k_drv_t attr_drvs uart_drv = {
		.maj = MAJ_SOC_UART,
		.name = "stm32-uart",
		.read = uart_read,
		.write = uart_write,
		.avail = uart_avail,
	};

Devices are instead tipically declared in the board initialization file:

.. code-block:: c

	const k_dev_t attr_devs uart2_dev = {
		.id = dev_id(MAJ_SOC_UART, MINOR_UART2),
		.name = "uart2",
	};

Anyway, this is not mandatory, because the linker script mechanism will group
any device and driver in the proper section.
