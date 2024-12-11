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
    if (!source.isOpened()) { cout << "Camera error" << endl; return -1; }

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

    VideoWriter preImg(pre_img, 0, (double)30, Size(640, 90), true);
    if (!preImg.isOpened()) { cerr << "Writer open failed!" << endl; return -1; }

```

낮은 계산양과 상단의 여러 방해물을 제거하기 위해 pre_image 함수를 이용하여 하단 영역만을 남긴다.
```
Mat pre_image(Mat origin){
    //const unsigned int print_mean = 0;
    Mat thresImg, grayImg;
    cvtColor(origin, grayImg, COLOR_BGR2GRAY);

    Scalar img_mean = mean(grayImg);
    //cout << "mean1 : " << img_mean[0] << endl;
    //밝기 보정하는 영역
    grayImg = grayImg + (128 - img_mean[0]);
    img_mean = mean(grayImg);
    threshold(grayImg, thresImg, img_mean[0] + 25, 255, THRESH_BINARY);

    Mat roi = thresImg(Rect(Point(0, (grayImg.rows / 4)* 3), Point(grayImg.cols, grayImg.rows)));
    return roi;
}
```
mean을 계산하는 과정을 통해 전체적으로 밝은 날씨와 어두운 날씨에 대해 사용하지 못하게 되는 단점을 보완하였고 이진화는 평균값에 일정 값을 추가하여 진행하였다. 이후 3/4영역부터 1의 영역까지 행을 반환한다.


Port를 8001과 8002로 나누어 각각 원본과 이진화 영상을 전송한다.
```
source << frame;
source2 << preImg;
```

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

        //진행 라인 O, y좌표가 87 이상이면 라인이 사라진것으로 파악 박스를 그리지 않는다.
        circle(pImage, present_point, 3, Scalar(0, 0, 255), -1);
        if(present_point.y < 87){
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

void get_speed(int error, double k_val){
    if(error != 0) {
        get_k = normalize(error);
    }
    else get_k = 0;

    return get_k * k_val;
}
```
nomalize는 error값을 -1과 1 사이의 값으로 정규화시켜주며 get_k_error에서는 base_speed에 따라 변경되는 k값을 받아 base_speed에 증감을 더할 속도 값을 반환하게 하였다.
base_speed가 100인 경우에는 k값을 70으로 설정하였고 base_speed가 10씩 증감함에 따라 k값도 2씩 증감하게 하여 속도가 증가할수록 조금 더 빠르게 회전하는 것을 목표로 하였다.

```
void set_dxl(double k, int error){
    leftvel = int(base_speed - get_speed(error, k));
    rightvel = -int(base_speed + get_speed(error, k));
        
    if(mode) dxl.setVelocity(leftvel, rightvel);
        
    //cout << "leftvel : " << leftvel << ",  rightvel : " << rightvel << endl;
    //cout << "K_val : " << k << endl;    
}
```
조절된 k값과 error값의 계산은 이후 set_dxl 함수에 들어가 각각의 바퀴에 따른 속도를 조절하게 되어 모터 제어에 이용된다.



#작동 영상

동영상 1 : https://youtu.be/fCCPWCsQUZU

동영상 2 : https://youtu.be/9My9HEBeNtU

