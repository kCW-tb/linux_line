CX = g++   
CXFLAGS = -g -Wall
CVFLAGS = `pkg-config opencv4 --cflags --libs`
SRCS = main.cpp      
TARGET = camera
$(TARGET):$(SRCS)
	$(CX) $(CXFLAGS) -o $(TARGET) $(SRCS) $(CVFLAGS)
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
