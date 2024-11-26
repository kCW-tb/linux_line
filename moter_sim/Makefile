CX = g++
CXFLAGS = -g -Wall
DXLFLAGS = -I/usr/local/include/dynamixel_sdk -ldxl_x64_cpp
CVFLAGS = `pkg-config opencv4 --cflags --libs`

TARGET = moter_sim
OBJS = main.o dxl.o vision.o

$(TARGET) : $(OBJS)
	$(CX) -o $(TARGET) $(OBJS) $(DXLFLAGS) $(CVFLAGS)

main.o : main.cpp
	$(CX) $(CXFLAGS) -c main.cpp $(DXLFLAGS) $(CVFLAGS)

dxl.o : dxl.hpp dxl.cpp
	$(CX) $(CXFLAGS) -c dxl.cpp $(DXLFLAGS)

vision.o : vision.hpp vision.cpp
	$(CX) $(CXFLAGS) -c vision.cpp $(CVFLAGS)

.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)
