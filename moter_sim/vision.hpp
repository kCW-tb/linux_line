#ifndef _VISION_HPP_
#define _VISION_HPP_

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace cv;
using namespace std;
//이미지 전처리
Mat pre_image(Mat origin);

Mat line_detect(Mat roi);
//라인 선정에 대해 사용하는 fix_p 클래스.
class fix_p {
private:
    int index;
    double distance;
public:
    fix_p(int index, double distance);
    int get_index(void);
    double get_distance();
};
//sort함수 오름차순 or 내림차순
bool compare_function(fix_p& compair_one, fix_p& compair_two);
//error값을 정규화해주는 함수(타 함수에 이용)
double normalize(int x);
//dxl속도 설정
void set_dxl(double k, int error);
//k값 변화하는 함수
void get_k_error(int error, double *k_val);
//시스템 종료
void ctrlc(int);

#endif
