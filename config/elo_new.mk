# General library configuration
ARCH := arm
CPU := cortex-m4
SOC := stm32f401xx
SOC_VARIANT := stm32f401ret7
BOARD := elo_new
APP := def
CFLAGS += -DUART_CLI_FNAME="\"uart1\""

