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
    
    threshold(grayImg, thresImg, img_mean[0], 255, THRESH_BINARY);

    Mat roi = grayImg(Rect(Point(0, (grayImg.rows / 5)* 3), Point(grayImg.cols, grayImg.rows)));
    return roi;
}

