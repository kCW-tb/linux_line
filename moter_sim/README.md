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


##라인 시뮬레이션
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
    threshold(grayImg, thresImg, img_mean[0] + 10, 255, THRESH_BINARY);

    Mat roi = thresImg(Rect(Point(0, (grayImg.rows / 4)* 3), Point(grayImg.cols, grayImg.rows)));
    return roi;
}
```
mean을 계산하는 과정을 통해 전체적으로 밝은 날씨와 어두운 날씨에 대해 사용하지 못하게 되는 단점을 보완하였고 이진화는 평균값에 일정 값을 추가하여 진행하였다. 이후 3/4영역부터 1의 영역까지 행을 반환한다.

```
        int lable_cnt = connectedComponentsWithStats(pImage, labels, stats, centroids);
        cvtColor(pImage, pImage, COLOR_GRAY2BGR);

        vector<fix_p> v;
        //0 번 배경은 빼고 다음부터 진행
        for (int i = 1; i < lable_cnt; i++) {
            double* p = centroids.ptr<double>(i);
            int* q = stats.ptr<int>(i);

            if (q[4] > 100) {
                distance = sqrt(pow((present_point.x - p[0]), 2) + pow((present_point.y                 - p[1]), 2));
                v.push_back(fix_p(i, distance));
            }
        }
        sort(v.begin(), v.end(), compare_function);
```
lable_cnt 변수로 검출된 객체의 개수를 파악하고 너무 작은 면적을 가진 객체를 100이라는 임계값을 주어 stats행렬에서 무시하고 나머지 좌표들에 대해서 거리를 측정하여 fix_p 클래스 벡터에 차례대로 저장한다. 이후 클래스 벡터를 크기순으로 정렬한다.
![image](https://github.com/user-attachments/assets/fc3c6e2d-f03e-4da1-82c3-1fd8f680d83a)

```
//가장 가까운 객체 무게중심 좌표로 초기화
        double* p = centroids.ptr<double>(v[0].get_index());
        present_point = Point2d(p[0], p[1]);

        distance = sqrt(pow((present_point.x - past_point.x), 2) + pow((present_point.y - past_point.y), 2));
        
        if ((abs(present_point.x - past_point.x) > pImage.cols / 4) || (abs(present_point.y - past_point.y) > pImage.rows / 4)) {
            present_point = past_point;
            //cout << "distance : " << distance << endl;
        }

        error = pImage.cols / 2 - present_point.x;
```
검출된 좌표에 대해서 밖으로 나가서 사라지는 경우를 예방하기 위해 이전값과 비교하기 위해서 현재 잡힌 좌표와 이전에 잡힌 좌표의 거리를 비교하고 이 좌표가 이미지 전체에서 1/4영역보다 차이가 크면 무시하고 아닐 경우는 past_point를 present_point로 초기한다.

```
 //진행 라인 O
        circle(pImage, present_point, 3, Scalar(0, 0, 255), -1);
        if(present_point.y < 85){
            int *q = stats.ptr<int>(v[0].get_index());
            rectangle(pImage, Rect(q[0], q[1], q[2], q[3]), Scalar(0,0,255), 2);
        }
        //진행 라인 X
        for (size_t j = 1; j < v.size(); j++) {
            double* p = centroids.ptr<double>(v[j].get_index());
            int* q = stats.ptr<int>(v[j].get_index());
            circle(pImage, Point(p[0], p[1]), 3, Scalar(255, 0, 0), -1);
            rectangle(pImage, Rect(q[0], q[1], q[2], q[3]), Scalar(255, 0, 0), 2);
        }
```
검출된 영상에 대해서 진행하여야 하는 라인은 붉은색으로 아닌 라인은 파란색으로 구별하여 디버깅을 수행. 현재 좌표 present_point 좌표의 y 좌표가 85이상인 경우 라인이 하단 넘어로 넘어가 현재 보이지 않는 것으로 판단하여 85 미만에 있을 경우에만 Rounding Box를 취해준다. 단 해당 라인이 사라진 것을 확인하기 위해서 circle로 past_point를 지속하여 유지하여 준다.





##라인 트레이서 관련

```
    Dxl dxl;
    if(!dxl.open()) { cout << "dynamixel open error"<< endl; return -1; }
```
Dxl을 활성화 시켜주는 코드

```
        if(dxl.kbhit()){
            char ch = dxl.getch();
            if(ch == 'q') break;
            else if(ch == 's') {mode = true; cout << "mode is true" << endl;}
            else if(ch == 'h') {base_speed+=10; k+=2;}
            else if(ch == 'l') {base_speed-=10; k-=2;}
        }
```
변화하는 속도 (base_speed)에 따라 k값을 변화시켜 실시간으로 속도가 증가할 때 k값도 증가시키는 등의 작업으로 속도 변화에 대한 회전 값을 변경시킨다.
base_speed가 100인 경우에는 k값을 70으로 설정하였고 base_speed가 10씩 증감함에 따라 k값도 2씩 증감하게 하여 속도가 증가할수록 조금 더 빠르게 회전하는 것을 목표로 하였다.

```
//get_k_error에 사용되는 error값에 대한 정규화 진행
double normalize(int x) {
    return x / 350.0;
}

//error값에 따라 k값을 구하는 코드
double get_k_error(int error, double k_val){
    double norm_error;
    if(error != 0) {
        norm_error = normalize(error);
    }
    else norm_error = 0;
    
    cout << "norm_error : " << norm_error << endl;

    return k_val * norm_error;
}
```
nomalize는 error값을 -1과 1 사이의 값으로 정규화시켜주며 get_k_error에서는 base_speed에 따라 변경되는 k값을 받아 base_speed에 증감을 더할 속도 값을 반환하게 하였다.


```
        //증감에 이용할 speed 계산.
        double speed = get_k_error(error, k);
        cout << "speed : " << speed << endl;        
        
        leftvel = int(base_speed - speed);
        rightvel = -int(base_speed + speed);

        if(mode) {
            if(!dxl.setVelocity(leftvel, rightvel));
        }
```
조절된 k값과 error값의 계산은 이후 leftvel과 rightvel을 초기화하여 들어가 각각의 바퀴에 따른 속도를 조절하게 되어 모터 제어에 이용된다.



#작동 영상

라인 시뮬레이션 영상

동영상 1 : https://youtu.be/qKPKYbrr4PA

동영상 2 : https://youtu.be/mkuS4HHe4AQ

Dxl 제어 영상

동영상 1 : https://youtu.be/fCCPWCsQUZU

동영상 2 : https://youtu.be/9My9HEBeNtU

라인 트레이서 영상

100rpm 유지 동영상
동영상 1 : https://www.youtube.com/watch?v=M8UIMTLAlb8&list=PLD5xTFCRSYMl3mdoEcsJyxCHy7MI4VoYG&index=10

속도 변화하는 영상
동영상 1(컴퓨터 내) : https://youtu.be/WUuIuioKLGY
동영상 2(외부 영상) : https://www.youtube.com/watch?v=WUuIuioKLGY&list=PLD5xTFCRSYMl3mdoEcsJyxCHy7MI4VoYG&index=9
