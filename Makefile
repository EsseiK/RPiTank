CC = gcc
CFLAG = -Wall -Wextra -02
LDFLAGS = -pthread -lrt -lwiringPi
SRCS = ./test.c ./make_new_thread.c ./server/thread_server.c ./server/server.c ./server/command_analysis.c ./motor/motor.c ./motor/thread_motor.c
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
	$(RM) ./motor/*.o
	$(RM) ./server/*.o
	$(RM) $(TARGET)