CC = gcc
CFLAG = -Wall -Wextra -02
LDFLAGS = -pthread -lwiringPi
SRCS = ./server/test.c ./server/make_new_thread.c ./server/thread_server.c ./server/server.c ./server/command_anaysis.c ./motor/motor.c
OBJS = $(SRCS:.c=.o)
TARGET = ./test

.c.o:
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

default: all

$(TARGET): $(OBJS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

all: $(TARGET)

clean:
	$(RM) *.o
	$(RM) $(TARGET)