APP = app/def

OBJS += tasks/task-cli.o
OBJS += $(APP)/def.o
OBJS += $(APP)/lcd.o
OBJS += $(APP)/leds.o
OBJS += $(APP)/pwm.o
OBJS += $(patsubst %.c,%.o,$(wildcard cli/*.c))
