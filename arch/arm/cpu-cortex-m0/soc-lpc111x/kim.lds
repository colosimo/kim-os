/*
 * Author: Aurelio Colosimo, 2016
 */

MEMORY
{
	flash : o = 0x00000000, l = 32k
	sram  : o = 0x10000000, l =  4k
}

SECTIONS
{
	. = ORIGIN(flash);

	.text : {
		*(isrv_sys); /* Interrupt Service Routine Vector - System */
		*(isrv_irq); /* Interrupt Service Routine Vector - Peripherals IRQs */
		. = 0x000000c0;
		*(conf);
		*(.text);
		*(.rodata);
	} > flash

	. = ORIGIN(sram);

	.data : {
		__start_data_flash = LOADADDR(.data);
		__start_data_sram = .;
		*(.data);
		__start_drvs = .;
		*(drvs)
		__stop_drvs = .;
		__start_devs = .;
		*(devs)
		__stop_devs = .;
		__start_regs = .;
		*(regs)
		__stop_regs = .;
		__start_tsks = .;
		*(tsks)
		__stop_tsks = .;
		__end_data_sram = .;
	} > sram AT > flash

	__start_bss = .;
	.bss : {
		*(.bss);
	} > sram
	__end_bss = .;
}
