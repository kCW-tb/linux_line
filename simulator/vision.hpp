#ifndef _VISION_HPP_
#define _VISION_HPP_

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat pre_image(Mat origin);
//void ctrlc_handler(int);

#endif
