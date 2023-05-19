APP = app/def

OBJS += tasks/task-cli.o
OBJS += $(APP)/def.o
OBJS += $(APP)/lcd.o
OBJS += $(APP)/leds.o
OBJS += $(APP)/ant.o
OBJS += $(APP)/eeprom.o
OBJS += $(APP)/keys_$(BOARD).o
OBJS += $(APP)/rtc.o
OBJS += $(APP)/menu_$(BOARD).o
OBJS += $(APP)/rfrx.o
OBJS += $(APP)/db.o
OBJS += $(APP)/bluetooth.o
OBJS += $(APP)/deadline.o
OBJS += $(patsubst %.c,%.o,$(wildcard cli/*.c))

ifeq ($(BOARD),elo_new)
OBJS += $(APP)/osm.o
endif
