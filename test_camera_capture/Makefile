#######################################################
# Makefile for grasshopper camera capture with OpenCV #
#######################################################
TARGET	= cam_capture
TARGET2	= cam_capture2

# compiler flag
GCC	= gcc -g

# additional include path
INC_PATH	= -I/usr/local/include/opencv -I/usr/local/include/opencv2 -I/usr/include/flycapture 

# additional lib path
LIB_PATH	= -L/usr/local/lib 

# additional library
LIB	= -lopencv_highgui -lopencv_core -lopencv_imgproc -lflycapture-c -lstdc++

all:
	$(GCC) $(INC_PATH) $(LIB_PATH) -o $(TARGET) ./opencv_viewer.cpp $(LIB)
	$(GCC) $(INC_PATH) $(LIB_PATH) -o $(TARGET2) ./opencv_viewer2.cpp $(LIB)

clean:
	rm -f ./*~ ./$(TARGET) ./$(TARGET2)




