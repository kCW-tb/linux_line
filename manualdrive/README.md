배터리를 달고서 움직이는 코드.


```
    VideoWriter writer1(dst1,0, (double)30,Size(640,360),true);
    
    if(!writer1.isOpened()) {cerr<<"Writer open failed!"<<endl; return -1;}
    
    Mat frame, gray;

    VideoWriter save;
    
    double fps = 25.0; // 영상 프레임
	int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G'); // 코덱 설정

    save.open("./save_Video.avi", fourcc, fps, Size(640,360), true);
```

VideoWriter 클래스에 대하여 save용 객체 생성

```
        source >> frame;
        save << frame;
        if (frame.empty()){ cerr << "frame empty!" << endl; break; }
```

카메라로부터 받는 영상을 그대로 save에 저장.

```
if (mx.kbhit())
        {
            char c = mx.getch();
            switch(c)
            {
                case 's': goal1 = 0; goal2 = 0; break;
                case ' ': goal1 = 0; goal2 = 0; break;
                case 'f': goal1 = 50; goal2 = -50; break;
                case 'b': goal1 = -50; goal2 = 50; break;
                case 'l': goal1 = -50; goal2 = -50; break;
                case 'r': goal1 = 50; goal2 = 50; break;
                default : goal1 = 0; goal2 = 0; break;
            }
        }
        // generate accel and decel motion
        if(goal1>vel1) vel1 += 5;
        else if(goal1<vel1) vel1 -= 5;
        else vel1 = goal1;
        
        if(goal2>vel2) vel2 += 5;
        else if(goal2<vel2) vel2 -= 5;
        else vel2 = goal2;
```
키를 받으면 해당 문자를 파악하여 각 모터에 대해 속도를 조절하고 입력값이 너무 차이날 때 천천히 증가 혹은 감소하도록 +5씩 증가하거나 -5씩 감소하도록 함.

