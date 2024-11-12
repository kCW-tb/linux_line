다이나믹셀 방향 조절

```
gettimeofday(&start,NULL);
        if (mx.kbhit()) //키보드입력 체크
        {
            char c = mx.getch(); //키입력 받기
            switch(c)
            {
                case 's': vel1 = 0; vel2 = 0; break; //정지
                case 'f': vel1 = 50; vel2 = -50; break; //전진
                case 'b': vel1 = -50; vel2 = 50; break; //후진
                case 'l': vel1 = -50; vel2 = -50; break; //좌회전
                case 'r': vel1 = 50; vel2 = 50; break; //우회전
                default : vel1 = 0; vel2 = 0; break; //정지
            }
            mx.setVelocity(vel1,vel2);
        }
```
키보드 입력을 계속해서 확인후 확인이 되면 입력받은 키를 확인
's', 'f', 'b', 'l', 'r' 중 하나이면 각 바퀴에 들어갈 속도에 대한 변수 설정
다른 키를 입력받게 되면 정지와 같은 변수값 설정
이후 받은 변수값에 대해서 dynamixel의 속도값 조정(setVelocity)

ctrl_c_pressed == true로 Ctrl+c를 받게 되면 while문 탈출

코드 다운로드 이후 make로 실행파일 생성 이후
sudo ./실행파일명 으로 실행



![image](https://github.com/user-attachments/assets/d88fa627-4b2d-4bec-bbec-fbf5e7bfa62b)

front입력 (양측의 속도의 부호가 달라야지 한 방향으로 이동.)
