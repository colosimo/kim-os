OBJS += tasks/task-cli.o
OBJS += tasks/task-clock.o
OBJS += $(patsubst %.c,%.o,$(wildcard cli/*.c))
