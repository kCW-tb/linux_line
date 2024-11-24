#ifndef _VISION_HPP_
#define _VISION_HPP_

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "opencv2/opencv.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace cv;
using namespace std;

Mat pre_image(Mat origin);
//void ctrlc_handler(int);

class fix_p {
private:
    int index;
    double distance;
public:
    fix_p(int index, double distance);
    int get_index(void);
    double get_distance();
};

bool compare_function(fix_p& compair_one, fix_p& compair_two);

#endif
