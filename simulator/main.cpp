#include "vision.hpp"
#include "dxl.hpp"

bool ctrl_c_pressed;
void ctrlc(int)
{
    ctrl_c_pressed = true;
}
int main(void)
{
    // image transport
    string src = "nvarguscamerasrc sensor-id=0 ! \
    video/x-raw(memory:NVMM), width=(int)640, height=(int)360, \
    format=(string)NV12, framerate=(fraction)30/1 ! \
    nvvidconv flip-method=0 ! video/x-raw, \
    width=(int)640, height=(int)360, format=(string)BGRx ! \
    videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    
    VideoCapture source(src, CAP_GSTREAMER);
    
    if (!source.isOpened()){ cout << "Camera error" << endl; return -1; }

    string camera = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.152 port=8001 sync=false";
    
    VideoWriter origin(camera, 0, (double)30, Size(640, 360), true);
    if (!origin.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}

    string pre_img = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.152 port=8002 sync=false";
    
    VideoWriter preImg(pre_img, 0, (double)30, Size(640, 144), true);
    if (!preImg.isOpened()) { cerr << "Writer open failed!" << endl; return -1; }
    
    VideoCapture line_video("out_line.mp4");
    if(!line_video.isOpened()) { cerr << "Can't open the video" << endl; return -1; }

    cv::TickMeter tm;
    while (true) {
        tm.reset();
        tm.start();
        Mat frame;

        line_video >> frame;
        origin << frame;
        preImg << pre_image(frame);
        
        tm.stop();

        cout << "Time : " << tm.getTimeMilli() << "ms" << endl;
        waitKey(30);
    }
    return 0;
}
