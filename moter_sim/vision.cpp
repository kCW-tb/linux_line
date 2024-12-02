#include "vision.hpp"

Mat pre_image(Mat origin){
    //const unsigned int print_mean = 0;
    Mat thresImg, grayImg;
    cvtColor(origin, grayImg, COLOR_BGR2GRAY);

    Scalar img_mean = mean(grayImg);
    //cout << "mean1 : " << img_mean[0] << endl;
    //밝기 보정하는 영역
    grayImg = grayImg + (128 - img_mean[0]);

    img_mean = mean(grayImg);

    //cout << "mean2 : " << img_mean[0] << endl;
    
    threshold(grayImg, thresImg, img_mean[0] + 25, 255, THRESH_BINARY);

    Mat roi = thresImg(Rect(Point(0, (grayImg.rows / 5)* 3), Point(grayImg.cols, grayImg.rows)));
    return roi;
}
//좌표 초기화 생성자.
fix_p::fix_p(int index, double distance) {
    this->index = index;
    this->distance = distance;
}
//변경된 객체에 대한 인덱스 부여
int fix_p::get_index(void) {
     return this->index;
}
//거리 차를 초기화시키는 함수
double fix_p::get_distance() {
    return this->distance;
}

//sort함수 수행을 위해 생성한 코드. (오름차순과 내림차순을 결정)
bool compare_function(fix_p& compair_one, fix_p& compair_two) {
    return compair_one.get_distance() < compair_two.get_distance();
}

//get_k_error에 사용되는 error값에 대한 정규화 진행
double normalize(int x) {
    return (x + 310.0) / 620.0;
}

//error값에 따라 k값을 구하는 코드
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
