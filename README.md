# linux_line

![image](https://github.com/user-attachments/assets/b2d212d4-a2b6-4942-9c2f-da380a2b3317)


2001180 김찬우

카메라 테스트 및 흑백, 이진화 진행

```
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    string src = "nvarguscamerasrc sensor-id=0 ! \
        video/x-raw(memory:NVMM), width=(int)640, height=(int)360, \
        format=(string)NV12, framerate=(fraction)30/1 ! \
        nvvidconv flip-method=0 ! video/x-raw, \
        width=(int)640, height=(int)360, format=(string)BGRx ! \
        videoconvert ! video/x-raw, format=(string)BGR ! appsink";
    
    VideoCapture source(src, CAP_GSTREAMER);
    if (!source.isOpened()) { cout << "Camera error" << endl; return -1; }

    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.152 port=8001 sync=false";
    
    VideoWriter writer1(dst1, 0, (double)30, Size(640, 360), true);
    if (!writer1.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}

    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.152 port=8002 sync=false";
    
    VideoWriter writer2(dst2, 0, (double)30, Size(640, 360), false);
    if (!writer2.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}

    string dst3 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.152 port=8003 sync=false";
    
    VideoWriter writer3(dst3, 0, (double)30, Size(640, 360), false);
    if (!writer3.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}
    

    cv::TickMeter tm;
    Mat frame, gray, thresh;
    while (true) {
        tm.reset();
        tm.start();
        
        source >> frame;
        if (frame.empty()){ cerr << "frame empty!" << endl; break; }
        
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        threshold(gray, thresh, 0, 255, THRESH_OTSU | THRESH_BINARY);
        
        writer1 << frame;
        writer2 << gray;
        writer3 << thresh;
        
        tm.stop();

        cout << "Time : " << tm.getTimeMilli() << "ms" << endl;
        waitKey(30);
    }
    return 0;
}
```

각 포트별로 8001~8003의 포트를 window에서 열어주어 대기상태 유지
프레임당 시간을 리셋[tm.reset()]시켜 시간이 누적되는 현상 방지
cvtColor로 RGB에서 Gray로 변환시키고 변환한 영상을 threshold를 통해 이진화 진행하여
각 포트의 인자가 전달되는 VideoWriter 객체에 전송.
