/*
 * Author: Aurelio Colosimo, 2019
 */
MEMORY
{
#ifdef SOC_VARIANT_stm32f103rb
	flash : o = 0x08000000, l = 128k
	sram  : o = 0x20000000, l =  20k
#elif defined SOC_VARIANT_stm32f103rc
	flash : o = 0x08000000, l = 256k
	sram  : o = 0x20000000, l =  64k
#else
#error Unhandled SOC_VARIANT: $(SOC_VARIANT)
#endif
}

SECTIONS
{
	.text 0x08000000: {
		*(isrv_sys); /* Interrupt Service Routine Vector - System */
		*(isrv_irq); /* Interrupt Service Routine Vector - Peripherals IRQs */
		. = 0x0000240;
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
