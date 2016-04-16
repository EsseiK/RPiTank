CC = g++
CFLAGS =  -Wall -Wextra `pkg-config --cflags opencv` `pkg-config --libs opencv`
INCLUDES = 
#INCLUDES = -I/usr/local/include/opencv `pkg-config opencv --cflags` `pkg-config opencv --libs`
LDFLAGS = -pthread -lrt -lwiringPi -ldl -lm
#-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lm
SRCS =	./test.cpp ./thread_camera.cpp ./send.cpp

OBJS = $(SRCS:.cpp=.o)
#OUTDIR = ./bin
TARGET = ./camtest

.cpp.o:
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

default: all

$(TARGET): $(OBJS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

#all: $(SRCS)
#	@echo "Compiling $<..."
#	$(CC) $(CFLAGS) -o $(OBJS) $(INCLUDES)

all: $(TARGET)

clean:
	$(RM) *.o
	$(RM) $(TARGET)