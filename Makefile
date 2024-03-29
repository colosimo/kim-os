#
# Author: Aurelio Colosimo, 2016
# Originally modified from P-SDR Makefile: https://github.com/colosimo/p-sdr
#

# In config dir, some typical configurations are stored.
# Default is unix

-include .config

ifeq ($(CONFIG),)
$(info Please set CONFIG var)
$(info Available configs: $(patsubst config/%.mk,%,$(wildcard config/*)))
endif

-include config/$(CONFIG).mk

KIMPATH := $(CURDIR)

# Cross compiling configuration
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CROSS_COMPILE)cpp
AR              = $(CROSS_COMPILE)ar
LD              = $(CROSS_COMPILE)ld
OBJCOPY         = $(CROSS_COMPILE)objcopy
STRIP           = $(CROSS_COMPILE)strip

ifeq ($(APP),)
EXE := kim.elf
else
EXE := $(APP).elf
endif

OUTPUT_DIR := generated
EXE_NOEXT := $(patsubst %.elf,%,$(EXE))
TARGET_LIB := $(OUTPUT_DIR)/lib$(EXE_NOEXT).a
TARGET_MK := $(patsubst %.a,%.mk,$(TARGET_LIB))
TARGET_BIGOBJ := $(OUTPUT_DIR)/$(EXE_NOEXT)-bigobj.o

# Verbosity
ifdef VERB
CFLAGS += -DVERB=$(VERB)
endif

# Arch
CFLAGS += -DARCH_$(ARCH)

# Board
CFLAGS += -DBOARD_$(BOARD)

# CLI prompt
ifdef CLI_PROMPT
CFLAGS += -DCLI_PROMPT=\"$(CLI_PROMPT)\"
endif

ifeq ($(NOLOGTIME),y)
CFLAGS += -DNOLOGTIME
endif

GIT_VERSION := $(shell git rev-parse --short HEAD)
ifneq ($(shell git diff),)
GIT_VERSION := $(GIT_VERSION)+
endif

COMPILE_DATE := $(shell date +"%Y%m%d")

# Targets begin

bin: config version $(EXE) $(TARGET_LIB)

version:
	@mkdir -p $(OUTPUT_DIR)
	@echo "#define GIT_VERSION \"$(GIT_VERSION)\"" > generated/version.h
	@echo "#define COMPILE_DATE \"$(COMPILE_DATE)\"" >> generated/version.h

config:
	@echo CONFIG=$(CONFIG) > .config

tools:
	make -C tools

cleantools:
	make -C tools clean

all: tools bin doc

doc:
	make -C doc latexpdf
	cp doc/_build/latex/kim-os.pdf doc

cleandoc:
	make -C doc clean

rebuild: clean bin

rebuildall: cleanall all

$(TARGET_MK): $(OBJS)
	@mkdir -p $(OUTPUT_DIR)
	@echo "# Created on $(shell date --rfc-3339=seconds)" > $(TARGET_MK)
	@echo CONFIG=$(CONFIG) >> $(TARGET_MK)
	@echo CFLAGS=$(CFLAGS) >> $(TARGET_MK)
	@echo LFLAGS=$(LFLAGS) >> $(TARGET_MK)
	@echo INCFLAGS=$(INCFLAGS) >> $(TARGET_MK)
	@echo ARCH=$(ARCH) >> $(TARGET_MK)
	@echo CROSS_COMPILE=$(CROSS_COMPILE) >> $(TARGET_MK)
	@echo LAST_LD=$(LAST_LD) >> $(TARGET_MK)
	@echo CPU=$(CPU) >> $(TARGET_MK)
	@echo MACH=$(MACH) >> $(TARGET_MK)
	@echo BOARD=$(BOARD) >> $(TARGET_MK)
	@echo APP=$(APP) >> $(TARGET_MK)

# Find out obj list by looking for any .c file under proper directories
OBJS += $(patsubst %.c,%.o,$(wildcard lib/*.c)) \
        $(patsubst %.c,%.o,$(wildcard kernel/*.c)) \
        $(patsubst %.c,%.o,$(wildcard arch/$(ARCH)/*.c)) \

-include arch/$(ARCH)/$(ARCH).mk
-include app/$(APP)/$(APP).mk

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

INCFLAGS += -Iarch/$(ARCH)/include -Iapp/$(APP) -Iinclude -Igenerated -Itasks \
    -Idrivers
CFLAGS += -DSOC_$(SOC) -DSOC_VARIANT_$(SOC_VARIANT)

# The list of files to be cleaned is:
# 1) any .o and in obj or in any arch; 2) each .d corresponding to its .o;
# 3) everything in generated/ directory
CLEAN_LIST += $(TARGET_LIB) $(OBJS) $(TARGET_BIGOBJ) $(EXE) $(EXE).bin \
    $(EXE).hex \
    $(patsubst %.o,%.d,$(OBJS)) \
    $(wildcard $(patsubst %,%/*.o,$(wildcard arch/*))) \
    $(wildcard $(patsubst %,%/*.d,$(wildcard arch/*))) \
    $(wildcard $(OUTPUT_DIR)/*) \
    $(DATAOBJS)

$(TARGET_LIB): $(OBJS) $(TARGET_MK)
	$(AR) cr $(TARGET_LIB) $(OBJS)

$(TARGET_BIGOBJ): $(OBJS) $(TARGET_MK)
	$(LD) $(OBJS) -r -o $(TARGET_BIGOBJ)

$(EXE): $(TARGET_BIGOBJ)
	$(LAST_LD) $(TARGET_BIGOBJ) $(DATAOBJS) $(LFLAGS) -o $(EXE)
	$(POST_LD)

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCFLAGS) -MMD -o $@ $<

clean:
	rm -f $(CLEAN_LIST)
	rm -fr $(OUTPUT_DIR)

cleanall: clean cleandoc cleantools

.PHONY: all bin clean cleanall doc cleandoc rebuildall config tools cleantools \
	version
