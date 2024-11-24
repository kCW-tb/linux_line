
#include "vision.hpp"

int main(void)
{
    VideoCapture line_video("out_line.mp4");
    if (!line_video.isOpened()) { cerr << "Can't open the video" << endl; return -1; }

    Mat frame, pImage;
    Mat labels, stats, centroids;

    Point2d past_point(320, 45), present_point(320, 45); //과거 및 현재 좌표
    double distance; //중심점

    double time1;
    while (true)
    {
        clock_t start, end;
        start = clock();
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
        //if(distance > pImage.rows / 3) { //카메라가 30fps로 전달하는데 물리적으로 이동이 가능하지 않을 때
        if ((abs(present_point.x - past_point.x) > pImage.cols / 2) || (abs(present_point.y - past_point.y) > pImage.rows / 2)) {
            present_point = past_point;
            //cout << "distance : " << distance << endl;
        }

        int error = pImage.cols / 2 - present_point.x;

        cout << "error: " << error << "\t";
        //cout << " / Point: " << present_point << endl;

        //Blue
        for (int j = 1; j < v.size(); j++) {
            double* p = centroids.ptr<double>(v[j].get_index());
            int* q = stats.ptr<int>(v[j].get_index());
            circle(pImage, Point(p[0], p[1]), 3, Scalar(255, 0, 0), -1);
            rectangle(pImage, Rect(q[0], q[1], q[2], q[3]), Scalar(255, 0, 0));
        }
        //Red
        circle(pImage, present_point, 3, Scalar(0, 0, 255), -1);
        int *q = stats.ptr<int>(v[0].get_index());
        rectangle(pImage, Rect(q[0], q[1], q[2], q[3]), Scalar(0,0,255));


        imshow("originImage", frame);
        imshow("pImage", pImage);

        past_point = present_point; //과거좌표 초기화

        waitKey(1);
        end = clock();
        cout << "time:" << end-start << endl;
    }
    return 0;
}
