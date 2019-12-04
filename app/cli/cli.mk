OBJS += tasks/task-cli.o
OBJS += $(patsubst %.c,%.o,$(wildcard cli/*.c))
