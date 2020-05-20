Cortex-M cores: from power-on to running application
====================================================

I think that one of the most instructive subjects to be mastered by any computer
programmer, especially if working in embedded scope, is the boot of a
processor. However, most of software APIs and RTOSes (especially those released
by hardware vendors) seem to have the goal of hiding that all; the reason is
quite simple: chip vendors just need to sell their hardware, so they want to
show *how easy* is to make them boot.

On the other side, KIM allows you to fully understand what happens in the low
levels and be able to completely handle the boot process. I hope this will be
useful, at least from an educational point of view.

Since Cortex-M is currently the only family of chips supported by KIM,
I will refer to ARM Cortex-M core boot process. It is based on ISR vector: at
each interrupt is assigned an address, which is automatically called when the
interrupt happens. In KIM, the ISR vector is defined as a C array of pointers,
defined in the files:

- ``arch/arm/cpu-cortex-m0/cpu.c`` for Cortex-M0

- ``arch/arm/cpu-cortex-m3/cpu.c`` for Cortex-M3

- ``arch/arm/cpu-cortex-m4/cpu.c`` for Cortex-M4

Let's have a look at Cortex-M0, which is the basic model:

.. code-block:: c

	static const void *attr_isrv_sys _isrv_sys[] = {
		/* Cortex-M0 system interrupts */
		STACK_TOP,	/* Stack top */
		isr_reset,	/* Reset */
		isr_none,	/* NMI */
		isr_none,	/* Hard Fault */
		0,		/* Reserved */
		0,		/* Reserved */
		0,		/* Reserved */
		0,		/* Reserved */
		0,		/* Reserved */
		0,		/* Reserved */
		0,		/* Reserved */
		isr_none,	/* SVC */
		0,		/* Reserved */
		0,		/* Reserved */
		isr_none,	/* PendSV */
		isr_systick,	/* SysTick */
	};


The boot is defined by the first two elements:

- the first one must contain the initial address of Stack Pointer; KIM
  sets it to the top of available RAM, so that all the RAM is available
  for the stack; this behaviour can be changed, since ``STACK_TOP`` is defined
  in the ``reg.h`` (register definition) file(s) (e.g.
  ``arch/arm/cpu-cortex-m4/soc-stm32f407xx/include/reg.h``)

- the second element in the above ISR vector is the ``isr_reset`` function,
  that is pointer to the function executed at power on or reset.

Actually, ARM expects the ISR vector to reside in the first part of the SoC
flash; the definition of where each compiled object will be put is done
by the so-called linker script. In KIM, the linker script has extension
*.lds*. For instance, the file ``arch/arm/cpu-cortex-m4/soc-stm32f407xx/kim.lds``
contains the "map" of how we expect the compiled objects be put together
in the final binary file.

Let's have a look at the linker script:


.. code-block:: c

	MEMORY
	{
	#ifdef SOC_VARIANT_stm32f407vg
		flash : o = 0x08000000, l = 1024k
		sram  : o = 0x20000000, l =  128k
	#else
	#error Unhandled SOC_VARIANT: $(SOC_VARIANT)
	#endif
	}

In the first part (here above), the MEMORY section, the address and size of RAM
and Flash are declared; it is conditioned within C pre-processor ``#ifdef``
directories, because the KIM Makefile runs it in order to obtain a lds compliant
with the specific SoC version (after make, you will find ``kim.ldscpp`` which is
the output of C preprocessor).

.. code-block:: c

	SECTIONS
	{
		.text 0x08000000: {
			*(isrv_sys); /* Interrupt Service Routine Vector - System */
			*(isrv_irq); /* Interrupt Service Routine Vector - Peripherals IRQs */
			. = 0x0000240;
			*(.text)
		} > flash


The second part declares the "sections": the first one is text, which
contains the executable instructions. All ISRs (``isrv_sys`` and ``isrv_irq``)
are located at the beginning of the flash, then all remaining compiled code
``*(.text)`` follows.

.. code-block:: c

		. = ALIGN(16);
		.etext = .;

		.rodata : {
				*(.rodata)
		} > flash

		. = ALIGN(16);
		_erom = .;

		.data : {
			__start_data_flash = LOADADDR(.data);
			__start_data_sram = .;
			*(.data);
			. = ALIGN(4);
			__start_tsks = .;
			*(tsks)
			__stop_tsks = .;
			__start_drvs = .;
			*(drvs);
			__stop_drvs = .;
			__start_devs = .;
			*(devs);
			__stop_devs = .;
		} > sram AT > flash
		__end_data_sram = .;

In the next sections, the data (r/w and readonly), are declared. According to
gcc definition, data contains all the initialized data. In KIM, some special
sections are defined (see ``include/linker.h`` for more information).
The goal of these sections is to provide a *declarative* approach for some
relevant structs. In this way, if you want to add a task to the system,
you will just have to declare, wherever you want, a ``struct task_t`` with
``attr_tasks`` attributes, and it will automagically be part of main task array.

.. code-block:: c

		.bss : {
			. = ALIGN(16);
			__start_bss = .;
			*(.bss);
			. = ALIGN(16);
			__end_bss = .;
		} > sram
	}

The final part just reminds the linker script to include the bss section, and
to put it at the end of the RAM; bss is the uninitialized data, and is set to
0 at startup.

Coming back to our boot process, let's see what happens in ``isr_reset`` routine.
Here follows the C code copied from ``arch/arm/cpu-cortex-m-common.c`` (shared
by any Cortex-M SoC):

.. code-block:: c

	void isr_reset(void)
	{
		unsigned char *src, *dest;

		/* Load data to ram */
		src = &__start_data_flash;
		dest = &__start_data_sram;
		while (dest != &__end_data_sram)
						*dest++ = *src++;

		/* Set bss section to 0 */
		dest = &__start_bss;
		while (dest != &__end_bss)
						*dest++ = 0;

		/* Skip to mach or board specific init */
		init();
	}

``isr_reset`` performs three main tasks:

- load data section into RAM;

- set to zero the bss section;

- call the ``init`` function, which is declared externally, and is specific for
  each SoC.

Going on with stm32f407xx configuration, ``isr_reset`` will call the init
function defined in ``arch/arm/cpu-cortex-m4/soc-stm32f407xx/init.c``:

.. code-block:: c

	void attr_used init(void)
	{
		u32 cpu_freq, ahb_freq, apb_freq;

		/* Init board */
		board_init(&cpu_freq, &ahb_freq, &apb_freq);

		/* Init system ticks */
		wr32(R_SYST_RVR, cpu_freq / SYSTICKS_FREQ);
		wr32(R_SYST_CVR, 0);
		wr32(R_SYST_CSR, BIT0 | BIT1 | BIT2);

		log("ahb freq is %d\n", (uint)ahb_freq);

		/* Skip to main */
		k_main();
	}

Here, three actions are executed:

- initialize the cpu according to the board needs (``board_init`` function);
  it is specific to each board; please have a look at
  ``arch/arm/cpu-cortex-m4/soc-stm32f407xx/board/discovery_f407vg.c`` source
  code for STM32 Discovery F407 board. Typically, the first settings performed
  by ``board_init`` consist in choosing the right configuration for the system
  clock:

.. code-block:: c

	/* Enable HSE (8MHz external oscillator) */
	or32(R_RCC_CR, BIT16);
	while (!(rd32(R_RCC_CR) & BIT17));

	/* PLLM=8 PLLN=336, PLLP=00 (2), PLLQ=7; f_PLL=168MHz, f_USB=48MHz */
	and32(R_RCC_PLLCFGR, ~0x0f037fff);
	or32(R_RCC_PLLCFGR, BIT22 | (7 << 24) | (336 << 6) | 8);
	or32(R_RCC_CR, BIT24);
	while (!(rd32(R_RCC_CR) & BIT25));

	/* Flash latency */
	or32(R_FLASH_ACR, 0b111);

	/* Use PLL as system clock, with AHB prescaler set to 4 */
	wr32(R_RCC_CFGR, (0x9 << 4) | 0x2);
	while (((rd32(R_RCC_CFGR) >> 2) & 0x3) != 0x2);

	*cpu_freq = *apb_freq = *ahb_freq = 42000000;


- initialize the System Ticks, using the ARM SysTick timer; System Ticks
  are widely explained on the Internet;

- call the ``k_main`` function, implemented in ``kernel/kim.c``: it is the place
  where tasks are started and the domain of source code becomes independent on
  the chip; tasks will be deeply investigated in :any:`tasks`.

.. note::

  An exhaustive description regarding the ARM system is out of the scope of
  the present document: more information about the boot process, the Core
  registers, the interrupt routines, etc, can be found in the `Arm Information
  Center website <http://infocenter.arm.com>`_. A basic knowledge of gcc linker
  script is also required to better understand KIM details.
