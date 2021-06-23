APP = app/def

OBJS += tasks/task-cli.o
OBJS += $(APP)/def.o
OBJS += $(APP)/lcd.o
OBJS += $(APP)/leds.o
OBJS += $(APP)/pwm.o
OBJS += $(APP)/eeprom.o
OBJS += $(APP)/keys.o
OBJS += $(APP)/rtc.o
OBJS += $(APP)/menu.o
OBJS += $(APP)/rfrx.o
OBJS += $(patsubst %.c,%.o,$(wildcard cli/*.c))
