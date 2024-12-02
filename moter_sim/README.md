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

생성되는 Error값들은 값이 약 -300~300으로 정의되고 이를 조절하는 k의 값을 조금 더 유동적으로 움직일 수 있게 하기위해
함수를 생성.

```
double normalize(int x) {
    return (x + 310.0) / 620.0;
}

void get_k_error(int error, double *k_val){
    double get_k;
    if(error != 0) {
        get_k = normalize(error);
    }
    else get_k = 0;

    *k_val = pow(get_k*(1/2.0), 2);
    if(error < 0) *k_val *= (-1);

    if(error < -40 && error < 40) *k_val = 0.2;
}
```
nomalize는 error값을 0과 1 사이의 값으로 일축시켜주며 get_k_error에서는 x^2의 그래프를 완만하게 해준 그래프에 대해 적용시켜 error값이 크다면 k값도 크게 하였다. 안쪽 라인을 지날 땐 빠르게 방향을 움직이며 바깥쪽을 다닐 때는 k값도 낮추어 다양한 속도에 맞춰 적용시키려 코드를 구성하였다.

```
void set_dxl(double k, int error){
    leftvel = int(100.0 - k * error);
    rightvel = -int(100.0 + k * error);
        
    if(mode) dxl.setVelocity(leftvel, rightvel);
        
    cout << "leftvel : " << leftvel << ",  rightvel : " << rightvel << endl;
    //cout << "K_val : " << k << endl;    
}
```
조절된 k값과 error값은 이후 set_dxl 함수에 들어가 각각의 바퀴에 따른 속도를 조절하게 되어 라인트레이서가 완성된다.
