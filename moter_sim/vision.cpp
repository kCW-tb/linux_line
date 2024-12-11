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
    
    threshold(grayImg, thresImg, img_mean[0] + 10, 255, THRESH_BINARY);
    //하단 1/4영역 호출
    Mat roi = thresImg(Rect(Point(0, grayImg.rows * (3.0/4.0)), Point(grayImg.cols, grayImg.rows)));
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

//sort함수 수행을 위해 생성한 코드.
bool compare_function(fix_p& compair_one, fix_p& compair_two) {
    return compair_one.get_distance() < compair_two.get_distance();
}

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
