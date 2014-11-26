#######################################################
# Makefile for grasshopper camera capture with OpenCV #
#######################################################
TARGET	= rec_movie

# compiler flag
GCC	= g++ -g

# additional include path
INC_PATH	= -I/usr/local/include/opencv -I/usr/local/include/opencv2 -I/usr/include/flycapture 

# additional lib path
LIB_PATH	= -L/usr/local/lib 

# additional library
LIB	= -lopencv_highgui -lopencv_core -lopencv_imgproc -lflycapture-c

all:
	$(GCC) $(INC_PATH) $(LIB_PATH) -o $(TARGET) ./record_movie_by_grasshopper.cpp $(LIB)

clean:
	rm -f ./*~ ./$(TARGET)




