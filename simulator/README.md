라인 찾는 시뮬레이터

pImage = pre_image(frame);
전처리 함수.

```
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
```
밝고 어두운 환경을 보완하기 위해 평균값을 맞춰주고 이 평균값에 대해서 gray영상을 이진화해준다. 이후 하단 3/5영역을 뽑아 반환한다.

```
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
```
lable_cnt 변수로 검출된 객체의 개수를 파악하고 너무 작은 면적을 100이라는 임계값을 주어 무시하고 나머지 좌표들에 대해서 거리를 측정하여 fix_p 클래스 벡터에 차례대로 저장한다.
이후 클래스 벡터를 정렬한다.


```
//가장 가까운 객체 무게중심 좌표로 초기화
        double* p = centroids.ptr<double>(v[0].get_index());
        present_point = Point2d(p[0], p[1]);

        distance = sqrt(pow((present_point.x - past_point.x), 2) + pow((present_point.y - past_point.y), 2));
        
        if ((abs(present_point.x - past_point.x) > pImage.cols / 2) || (abs(present_point.y - past_point.y) > pImage.rows / 2)) {
            present_point = past_point;
            //cout << "distance : " << distance << endl;
        }

        error = pImage.cols / 2 - present_point.x;
```
검출된 좌표에 대해서 밖으로 나가서 사라지는 경우를 예방하기 위해 이전값과 비교하기 위해서 distance라는 변수에 대해 현재 잡힌 좌표와 거리를 비교하고 이 좌표가 이미지 전체에서 1/2영역보다 차이가 크면 무시하고 아닐 경우는 past_point를 present_point로 변경한다.


```
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
```
검출된 영상에 대해서 진행하여야 하는 라인은 붉은색으로 아닌 라인은 파란색으로 구별하여 디버깅을 수행.


동영상 링크

1.  https://youtu.be/OXlMzol8CkY

2.  https://youtu.be/eRowOr8A568

linux환경을 사용할 수 없어 후에 windows환경 코드를 linux환경과 일치시켜 수정 예정 (완료)
