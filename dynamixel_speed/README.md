다이나믹셀 속도 조절 코드

```
        cout << "left speed:";
        cin >> leftspeed;
        cout << "right speed:";
        cin >> rightspeed;
        mx.setVelocity(leftspeed, rightspeed);
        usleep(20*1000); //20msec sleep
        if (ctrl_c_pressed) break;
```
왼쪽 바퀴와 오른쪽 바퀴에 할당할 속도를 각각 입력하여 setVelocity에서 입력된 값을 바퀴에 전달
usleep로 과부화 안걸리게 적절한 휴식 시간 제공 (20ms) 

코드 다운로드 이후 make로 실행파일 생성 이후 sudo ./실행파일명 으로 실행

![image](https://github.com/user-attachments/assets/99693918-5ed4-437f-adfc-a3d8850d9b87)

스피드 왼쪽 오른쪽 각기 조절
