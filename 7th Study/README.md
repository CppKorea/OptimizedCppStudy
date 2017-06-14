Optinized Hot Statements
========================

차례
----

1. Remove Code from Loops
1. Remove Code from Functions
1. Optimize Expressions
1. Optimize Control Flow Idioms
1. Summary


Remove Code from _Loops_
------------------------

<img src="https://image.freepik.com/free-icon/refresh-arrow-loop_318-29909.jpg" width=200 />

* (예제 7-1) 최적화가 되지 않은 루프
```C++
char s[] = "This string has many space (0x20) chars. ";
    ...
for (size_t i = 0; i < strlen(s); ++i)
    if (s[i] == ' ')
        s[i] = '*';
```

* `strlen()` 함수가 매번 불리게 되므로 O(n^2)
  - VS2010: 13,238ms
  - VS2015: 11,467ms

### _Cache_ the Loop End Value

<img src="https://cdn-1.wp.nginx.com/wp-content/uploads/2015/07/cache-stock-photo.jpg" width=200 />

* (예제 7-2) 루프 종료 조건의 값이 캐시된 루프
```C++
for (size_t i = 0, len = strlen(s); i < len; ++i)
    if (s[i] == ' ')
        s[i] = '*';
```

* `strlen()` 결과를 `len` 변수에 캐시하므로 빨라짐
  - VS2010: 636ms (20배)
  - VS2015: 541ms

### Use More _Efficient_ Loop Statements

<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRZh53S4j1wy4MfWv0TOCRtcGEQQuIwWFXLz7yV0_utVPGs9_BZdg" width=200 />

* for 루프의 문법
```C++
for ( init-expression ; condition ; continue-expression ) controlled-statement
```

* 컴파일된 결과물
```C++
initial-expression ;
L1: if ( ! condition ) goto L2;
    controlled-statement ;
    continue-expression ;
    goto L1;
L2:
```

* do 루프의 문법
```C++
do controlled-statement while ( condition ) ;
```

* 컴파일된 결과물
```C++
L1: controlled-statement
    if ( condition ) goto L1;
```

* (예제 7-3) do 루프로 바꾼 for 루프
```C++
size_t i = 0, len = strlen(s); // for loop init-expression
do {
    if (s[i] == ' ')
        s[i] = '*';
    ++i;                       // for loop continue-expression
} while (i < len);             // for loop condition
```

* 속도
  - VS2010: 482ms (12% 빨라짐)
  - VS2015: 674ms (25% ___느려짐___)

### Count _Down_ Instead of Up

<img src="http://cdn2.hubspot.net/hubfs/360031/main_decrease.jpg" width=200 />

* (예제 7-4) 카운터를 줄이기
```C++
for (int i = (int)strlen(s)-1; i >= 0; --i)
    if (s[i] == ' ')
        s[i] = '*';
```

* `int` 대신 `size_t`를 쓰면 `i >= 0` 조건이 무한루프에 빠질 수 있으므로 주의
* 속도
  - VS2010: 619ms (응?)
  - VS2015: 571ms

### Remove _Invariant_ Code from Loops

<img src="https://invariants.org.uk/assets/logo.png" width=200 />

* (예제 7-5) 값이 변하지 않는 코드가 루프 안에 포함
```C++
int i,j,x,a[10];
    ...
for (i=0; i<10; ++i) {
    j = 100;
    a[i] = i + j * x * x;
}
```

* `j * x * x`는 변하지 않는 값이므로 루프 밖에서 한번만 계산해도 됨

* (예제 7-7) 값이 변하지 않는 코드가 루프 밖으로 빠진 형태
```C++
int i,j,x,a[10];
    ...
j = 100;
int tmp = j * x * x;
for (i=0; i<10; ++i) {
    a[i] = i + tmp;
}
```

### Remove _Unneeded_ Function Calls from Loops

<img src="http://planet.globalservicejam.org/sites/default/files/styles/gsj_jamsite_header_logo/public/gsj13/project/unneeded_0.png?itok=TaGGTkni" width=200 />

* (예제 7-1) 다시
```C++
char* s = "sample data with spaces";
    ...
for (size_t i = 0; i < strlen(s); ++i)
    if (s[i] == ' ')
        s[i] = '*'; // change ' ' to '*'
```

* (예제 7-7) `strlen()`은 변하지 않는 값이므로 밖으로
```C++
char* s = "sample data with spaces";
    ...
size_t end = strlen(s);
for (size_t i = 0; i < end; ++i)
    if (s[i] == ' ')
        s[i] = '*'; // change ' ' to '*'
```

* (예제 7-8) `strlen()`이 변하는 값일 경우
```C++
char* s = "sample data with spaces";
size_t i;
    ...
for (i = 0; i < strlen(s); ++i)
    if (s[i] == ' ')
        strcpy(&s[i], &s[i+1]); // remove space
s[i] = '\0';
```

* `s`의 공백이 제거되면서 길이가 점점 짧아지므로 `strlen()` 함수의 값도 계속 바뀜
* 그러면 함수의 결과 값이 불변임을 어떻게 판단하고 루프 밖으로 빼낼까?
  - 대체로 _pure functoin_ 의 경우 안심하고 밖으로 빼내면 됨
  - pure function 이란? (함수 내부 상태 정보가 없이) 인자의 변화에 의해서만 결과 값이 바뀌는 함수

* (예제 7-9) pure function을 포함하는 `rotate()` 함수
```C++
void rotate(std::vector<Point>& v, double theta) {
    for (size_t i = 0; i < v.size(); ++i) {
        double x = v[i].x_, y = v[i].y_;
        v[i].x_ = cos(theta) * x - sin(theta) * y;
        v[i].y_ = sin(theta) * x + cos(theta) * y;
    }
}
```

* 속도
  - VS2010: 7,502ms
  - VS2015: 6,864ms

* (예제 7-10) pure function을 루프 밖으로 빼낸 `rotate_invariant()` 함수
```C++
void rotate_invariant(std::vector<Point>& v, double theta) {
    double sin_theta = sin(theta);
    double cos_theta = cos(theta);
    for (size_t i = 0; i < v.size(); ++i) {
        double x = v[i].x_, y = v[i].y_;
        v[i].x_ = cos_theta * x - sin_theta * y;
        v[i].y_ = sin_theta * x + cos_theta * y;
    }
}
```

* 속도
  - VS2010: 7,382ms (3% 빨라짐. 애걔..)
  - VS2015: 6,620ms
  - 요즘 컴퓨터의 CPU는 대게 FPU(floating point unit)을 포함하고 있어서 대략 빠름

* 자주 나타나는 실수
```C++
UsefulTool subsystem;
InputHandler input_getter;
    ...
while (input_getter.more_work_available()) {
    subsystem.initialize();    // 냄새!
    subsystem.process_work(input_getter.get_work());
}
```

### Remove _Hidden_ Function Calls from Loops

<img src="https://ichef.bbci.co.uk/news/624/cpsprodpb/7924/production/_91021013_thinkstockphotos-517519673.jpg" width=200 />

* 변수를 사용할 때 암시적으로 호출되는 함수들
  - 클래스 변수를 선언할 때 => 생성자
  - 클래스 변수를 초기화 할 때 => 생성자
  - 클래스 변수를 대입(할당)할 때 => 대입(할당) 연산자
  - 클래수 변수와 연관된 연산을 할 때 => 연산자 멤버 함수
  - scope를 벗어날 때 => destructor
  - 함수의 인자로 전달할 때 => 복사 생성자
    * 참조나 포인터로 전달하면 복사를 피할 수 있음
  - 함수에서 반환될 때 => 이동 혹은 복사 생성자
    * 반환할 클래스 객체를 함수의 참조/포인터 인자로 받으면 피할 수 있음
  - container에 추가할 때 => 이동 혹은 복사 생성자
  - 벡터에 추가하는 중 벡터의 메모리가 재할당 될 때 => 이동 혹은 복사 생성자 (모든 원소에 대해)

* 할당 혹은 초기화가 루프와 관련이 없다면 밖으로 빼는 방법이 있음
  - before
    ```C++
    for (...) {
        std::string s("<p>");
        ...
        s += "</p>";
    }
    ```
  - after
    ```C++
    std::string s;
    for (...) {
        s.clear();
        s += "<p>";
        ...
        s += "</p>";
    }
    ```
  - 불필요한 생성자 호출을 막고, 기존에 확보된 버퍼를 재사용

### Remove _Expensive_, Slow-Changing Calls from Loops

<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSzQR4ysveNB1IxyFRHN28iNObhJCWEowBrYcHEERzKpsJCm1Mqqg" width=200 />

* (예제 7-11) `timetoa()` 함수: 문자열 배열에 시간을 포맷팅하는 함수
```C++
# include <ctime>

char* timetoa(char *buf, size_t bufsz) {
    if (buf == 0 || bufsz < 9)
        return nullptr; // invalid arguments
    time_t t = std::time(nullptr); // get time from O/S
    tm tm = *std::localtime(&t); // break time into hours, minutes, seconds*
    size_t sz = std::strftime(buf, bufsz, "%c", &tm); // format into buffer
    if (sz == 0) strcpy(buf, "XX:XX:XX"); // error
    return buf;
}
```

* 이 함수의 수행에 약 700ns 정도를 소비함
  - 단순히 로깅을 위한 목적이라면 매 로그 한 줄에 시간을 핸들링하는 비용이 너무 큼
  - 한번 얻은 시간을 재사용하며 로그 10줄에 한번 정도 시간을 갱신

### Push Loops Down into Functions to Reduce Call _Overhead_

<img src="https://cdn2.omidoo.com/sites/default/files/imagecache/full_width/images/bydate/jun_4_2012_-_100pm/shutterstock_77366545.jpg" width=200 />

* 루프 안에 있는 함수 호출을 함수 안에 루프로 바꾸기
  - 문자열 안에 출력 불가능한 문자를 '.'으로 바꾸는 함수 예제
  - before
    ```C++
    void replace_nonprinting(char& c) {
        if (!isprint(c))
            c = '.';
    }

    for (unsigned i = 0, e = str.size(); i < e; ++i)
        replace_nonprinting(str[i]);
    ```
  - after
    ```C++
    void replace_nonprinting(std::string& str) {
        for (unsigned i = 0, e = str.size(); i < e; ++i)
            if (!isprint(str[i]))
                c = '.';
    }
    ```

### Do Some Actions _Less_ Frequently

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/81/LESS_Logo.svg/1200px-LESS_Logo.svg.png" width=200 />

* 상황
  - 1초에 1,000번의 transaction을 처리하는 시스템
  - 종료 명령(flag)을 전달하면 1초 안에 종료되어야 함 (평균 500ms +- 100ms 안에)
  - 이런 경우 400ms보다 자주 종료 flag를 체크할 필요가 없음
* 만약
  - 임베디드 환경에서 키가 눌렸다 올라가는 값을 체크하기 위해 50ms가 필요하다면
  - 체크하는 시간을 감안하면 매 350ms에 한번씩 체크해야 함
  - 평균적으로 1초에 2.5회의 체크가 필요하므로
  - 1,000 - (2.5 * 50) = 875 transaction을 처리하게 됨
* (예제 7-12)
```C++
void main_loop(Event evt) {
    static unsigned counter = 1;
    if ((counter % 350) == 0)
        if (poll_for_exit())
            exit_program(); // noreturn
    ++counter;
    switch (evt) {
        ...
    }
}
```

### What About _Everything Else_?

<img src="http://2.bp.blogspot.com/-LE5_beStiDE/TVR6gwg0F1I/AAAAAAAAAKo/CTiN6kgOAhs/s1600/ev.jpeg" width=200 />

* 위에 소개한 이 외에 인터넷에 떠도는 트릭들은 다 쓰레기
* 30년간의 컴파일러 최적화의 발전이 왠만한 개발자의 튜닝을 능가함


Remove Code from Functions
--------------------------


Optimize Expressions
--------------------


Optimize Control Flow Idioms
----------------------------


Summary
-------
