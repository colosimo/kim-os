/*
 * Author: Aurelio Colosimo, 2019
 */
MEMORY
{
#ifdef SOC_VARIANT_stm32f030r8
	flash : o = 0x08000000, l = 64k
	sram  : o = 0x20000000, l =  8k
#elif defined SOC_VARIANT_stm32f030c8
	flash : o = 0x08000000, l = 64k
	sram  : o = 0x20000000, l =  8k
#else
#error Unhandled SOC_VARIANT: $(SOC_VARIANT)
#endif

}

SECTIONS
{
	.text 0x08000000: {
		*(isrv_sys); /* Interrupt Service Routine Vector - System */
		*(isrv_irq); /* Interrupt Service Routine Vector - Peripherals IRQs */
		. = 0x000000C0;
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
		__start_tsks = .;
		*(tsks)
		__stop_tsks = .;
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
