#include "vision.hpp"

//void ctrlc_handler(int){ ctrl_c_pressed = true; }

Mat pre_image(Mat origin){
    //const unsigned int print_mean = 0;
    Mat thresImg, grayImg;
    cvtColor(origin, grayImg, COLOR_BGR2GRAY);

    Scalar img_mean = mean(grayImg);
    cout << "mean1 : " << img_mean[0] << endl;
    //밝기 보정하는 영역
    grayImg = grayImg + (128 - img_mean[0]);

    img_mean = mean(grayImg);

    cout << "mean2 : " << img_mean[0] << endl;
    
    threshold(grayImg, thresImg, img_mean[0] + 25, 255, THRESH_BINARY);

    Mat roi = thresImg(Rect(Point(0, (grayImg.rows / 5)* 3), Point(grayImg.cols, grayImg.rows)));
    return roi;
}

fix_p::fix_p(int index, double distance) {
    this->index = index;
    this->distance = distance;
}
int fix_p::get_index(void) {
     return this->index;
}
double fix_p::get_distance() {
    return this->distance;
}

bool compare_function(fix_p& compair_one, fix_p& compair_two) {
    return compair_one.get_distance() < compair_two.get_distance();
}
