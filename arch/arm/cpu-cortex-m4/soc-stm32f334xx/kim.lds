/*
 * Author: Aurelio Colosimo, 2019
 */
MEMORY
{
#ifdef SOC_VARIANT_stm32f334r6
	flash : o = 0x08000000, l = 32k
#else
#error Unhandled SOC_VARIANT: $(SOC_VARIANT)
#endif
	sram : o = 0x20000000,  l =   12k
}

SECTIONS
{
	.text 0x08000000: {
		*(isrv_sys); /* Interrupt Service Routine Vector - System */
		*(isrv_irq); /* Interrupt Service Routine Vector - Peripherals IRQs */
		. = 0x0000200;
		*(.text)
	} > flash

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

	.bss : {
		. = ALIGN(16);
		__start_bss = .;
		*(.bss);
		. = ALIGN(16);
		__end_bss = .;
	} > sram
}
