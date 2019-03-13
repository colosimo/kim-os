#
# Author: Aurelio Colosimo, 2016
#
# This file is part of kim-os project: https://github.com/colosimo/kim-os
# According to kim-os license, you can do whatever you want with it,
# as long as you retain this notice.

A:=arch/$(ARCH)

CFLAGS += -Werror -Wall -mcpu=cortex-m0 -mthumb -Os -mfloat-abi=soft \
  -nostartfiles
CROSS_COMPILE=arm-none-eabi-
INCFLAGS += -I$A/cpu-$(CPU)/include
INCFLAGS += -I$A/cpu-$(CPU)/soc-$(SOC)/include

LDS = $A/cpu-$(CPU)/soc-$(SOC)/kim.lds
GCCVER = $(shell $(CROSS_COMPILE)gcc --version|grep ^arm|cut -f 3 -d ' ')
LFLAGS += -T $(LDS) -L/usr/lib/gcc/arm-none-eabi/$(GCCVER)/armv6-m -lgcc \
	-nostdlib -nostartfiles
LAST_LD = $(CC)

OBJS += $(patsubst %.c,%.o,$(wildcard $A/cpu-$(CPU)/*.c))
OBJS += $(patsubst %.c,%.o,$(wildcard $A/cpu-$(CPU)/soc-$(SOC)/*.c))
OBJS += $A/cpu-$(CPU)/soc-$(SOC)/board/$(BOARD).o

POST_LD += $(OBJCOPY) -O binary $(EXE) $(EXE).bin;
POST_LD += $(OBJCOPY) -O ihex $(EXE) $(EXE).hex;
