#include "vision.hpp"
#include "dxl.hpp"

bool ctrl_c_pressed;
bool mode = false;
double k = 0.5;
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

    VideoCapture line_video("in_line.mp4");
    if(!line_video.isOpened()) { cerr << "Can't open the video" << endl; return -1; }

    Dxl dxl;
    int leftvel, rightvel;

    Mat frame, pImage;
    Mat labels, stats, centroids;

    Point2d past_point(320, 45), present_point(320, 45); //과거 및 현재 좌표
    double distance; //중심점
    char ch;
    cv::TickMeter tm;
    int error = 0;
    while (true) {
        tm.reset();
        tm.start();

        if(dxl.kbhit()){
            ch = dxl.getch();
            if(ch == 'q') break;
            else if(ch == 's') mode = true;
        }
        if (ctrl_c_pressed) break;

        leftvel = int(100.0 - k * error);
        rightvel = -int(100.0 + k * error);
        if(mode) dxl.setVelocity(leftvel, rightvel);

        line_video >> frame;
        if (frame.empty()) {
            cerr << "frame empty!" << endl; break;
        }
        //이미지 전처리
        pImage = pre_image(frame);

        int lable_cnt = connectedComponentsWithStats(pImage, labels, stats, centroids);
        cvtColor(pImage, pImage, COLOR_GRAY2BGR);

        vector<fix_p> v;
        //0 번 배경은 빼고 다음부터 진행
        for (int i = 1; i < lable_cnt; i++) {
            double* p = centroids.ptr<double>(i);
            int* q = stats.ptr<int>(i);

            if (q[4] > 100) {
                distance = sqrt(pow((present_point.x - p[0]), 2) + pow((present_point.y - p[1]), 2));
                v.push_back(fix_p(i, distance));
            }
        }
        sort(v.begin(), v.end(), compare_function);

        //가장 가까운 객체 무게중심 좌표로 초기화
        double* p = centroids.ptr<double>(v[0].get_index());
        present_point = Point2d(p[0], p[1]);

        distance = sqrt(pow((present_point.x - past_point.x), 2) + pow((present_point.y - past_point.y), 2));
        
        if ((abs(present_point.x - past_point.x) > pImage.cols / 2) || (abs(present_point.y - past_point.y) > pImage.rows / 2)) {
            present_point = past_point;
            //cout << "distance : " << distance << endl;
        }

        error = pImage.cols / 2 - present_point.x;

        cout << "error: " << error << "\t";
        //cout << " / Point: " << present_point << endl;

        //진행 라인 O
        circle(pImage, present_point, 3, Scalar(0, 0, 255), -1);
        if(present_point.y < 135){
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

        origin << frame;
        preImg << pImage;    

        past_point = present_point; //과거좌표 초기화

        get_k_error(error,&k);

        tm.stop();
        cout << "Time : " << tm.getTimeMilli() << "ms" << endl;
        waitKey(30);
    }
    return 0;
}
