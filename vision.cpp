#include "vision.hpp"
//void ctrlc_handler(int){ ctrl_c_pressed = true; }

Mat pre_image(Mat origin){
    Mat thresImg, grayImg;
    cvtColor(origin, grayImg, COLOR_BGR2GRAY);

    Scalar img_mean = mean(grayImg);
    cout << "mean1 : " << img_mean[0] << endl;
    //밝기 보정하는 영역
    grayImg += (128 - img_mean[0]);

    img_mean = mean(grayImg);
    
    cout << "mean2 : " << img_mean[0] << endl;
    
    threshold(grayImg, thresImg, (double)img_mean[0] + 20, 255, THRESH_BINARY);

    Mat roi = thresImg(Rect(Point(0, (grayImg.rows / 5)* 3), Point(grayImg.cols, grayImg.rows)));
    cout << "roi size : " << roi.size() << endl;
    cvtColor(roi , roi, COLOR_GRAY2BGR);
    return roi;
}

Mat line_detect(Mat roi){
    static double error = roi.cols / 2.0;

    Mat label, stats, centroids;
    int num = cv::connectedComponentsWithStats(roi, label, stats, centroids);
    int close_error = 640;
    //이전 error와 가까운 좌표 인덱스 뽑기.
    for(int i = 0; i < num; i++){
        if(close_error > abs(int(error-centroids.at<double>(0,i)) )){
            error = centroids.at<double>(0,i);
        }
    }
    //stats입력 위치.
    rectangle(roi, Point(stats.at<uchar>(i,0),stats.at<uchar>(i,1),),Point());
}
