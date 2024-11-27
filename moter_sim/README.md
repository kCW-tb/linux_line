다이나믹셀을 제어하는 dxl.cpp, dxl.hpp

VideoCapture로 카메라에 대한 이미지를 받고 VideoWriter에서 Windows에서 디버깅용 화면을 출력하기 위해 변수들을 선언 및 객체를 열어준다.

```
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

```


Port를 8001과 8002로 나누어 각각 원본과 이진화되어 하단의 영역을 표시
프레임당 connectedComponentsWithStats를 실행하여 이진화된 영상에 대하여 객체를 판별하고 이전 past_point와 present_point를 비교하여 가장 거리 차이가 적은 점을 다음 프레임의 라인으로 잡는다.

```
        //가장 가까운 객체 무게중심 좌표로 초기화
        double* p = centroids.ptr<double>(v[0].get_index());
        present_point = Point2d(p[0], p[1]);

        distance = sqrt(pow((present_point.x - past_point.x), 2) + pow((present_point.y - past_point.y), 2));
        
        if ((abs(present_point.x - past_point.x) > pImage.cols / 2) || (abs(present_point.y - past_point.y) > pImage.rows / 2)) {
            present_point = past_point;
            //cout << "distance : " << distance << endl;
        }
```

이미지의 중앙에 대하여 error값을 x좌표로 구하고 디버깅용으로 라인이 잘 나오는지 확인.
```
error = pImage.cols / 2 - present_point.x;

        cout << "error: " << error << "\t";
        //cout << " / Point: " << present_point << endl;

        //진행 라인 O, y좌표가 135 이상이면 라인이 사라진것으로 파악 박스를 그리지 않는다.
        circle(pImage, present_point, 3, Scalar(0, 0, 255), -1);
        if(present_point.y < 135){
            int *q = stats.ptr<int>(v[0].get_index());
            rectangle(pImage, Rect(q[0], q[1], q[2], q[3]), Scalar(0,0,255), 2);
        }
        //진행 라인 X, 나머지 객체에 대해 박스를 그려 파악한다.
        for (size_t j = 1; j < v.size(); j++) {
            double* p = centroids.ptr<double>(v[j].get_index());
            int* q = stats.ptr<int>(v[j].get_index());
            circle(pImage, Point(p[0], p[1]), 3, Scalar(255, 0, 0), -1);
            rectangle(pImage, Rect(q[0], q[1], q[2], q[3]), Scalar(255, 0, 0), 2);
        }
```

