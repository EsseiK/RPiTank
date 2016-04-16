CC = gcc
CXX = g++
CAM = camera/
CFLAGS =  -Wall -Wextra `pkg-config --cflags opencv` `pkg-config --libs opencv`
INCLUDES = -I /usr/local/include/opencv
LDFLAGS = -pthread -lrt -lwiringPi -ldl -lm
SRCS =	./main.c ./make_new_thread.c \
	./server/thread_server.c ./server/server.c ./server/command_analysis.c \
	./motor/motor.c ./motor/thread_motor.c
#	./camera/camera.cpp ./camera/thread_camera.cpp
SRCS_CPP = $(CAM)camera.cpp $(CAM)thread_camera.cpp

OBJS = $(SRCS:.c=.o)
OBJS_CPP = $(SRCS_CPP:.cpp=.o)
TARGET = ./RPiTank

#include $(CAM)Makefile

.c.o:
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

#.cpp.o:
#	@echo "Compiling $<..."
#	$(CXX) $(CFLAGS) -c $< -o $@ $(INCLUDES)

default: all

$(TARGET): $(OBJS)
	@echo "Compiling $<..."
	$(CXX) $(CFLAGS) -c $(CAM)thread_camera.cpp -o $(CAM)thread_camera.o $(INCLUDES)
	$(CXX) $(CFLAGS) -c $(CAM)camera.cpp -o $(CAM)camera.o $(INCLUDES)
	@echo "Linking $<..."
	$(CXX) $(CFLAGS) -o $@ $(OBJS) $(OBJS_CPP) $(LDFLAGS)

all: $(TARGET)

ncam: $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(RM) *.o
	$(RM) ./motor/*.o
	$(RM) ./server/*.o
	$(RM) ./camera/*.o
	$(RM) $(TARGET)