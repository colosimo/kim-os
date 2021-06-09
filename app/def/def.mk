APP = app/def

OBJS += tasks/task-cli.o
OBJS += $(APP)/def.o
OBJS += $(APP)/lcd.o
OBJS += $(patsubst %.c,%.o,$(wildcard cli/*.c))
