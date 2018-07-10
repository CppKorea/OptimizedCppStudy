# C++ Concurrency Facilities Refresher

## Mutexes
C++은 Mutual exclusion을 제공하기 위해 여러 가지의 Mutex template을 지원한다.<br/>
`<mutex>` 헤더를 추가해서 사용할 수 있으며, 특정 운영체제에 의존적인 native 클래스들을 제공한다.

### `std::mutex` (C++11)
* 간단한 Mutex 구현체
* `Busy-wait`을 먼저 시도하다가, mutex를 빨리 얻지 못하면 System call을 수행.
* 동일한 mutex로 lock을 여러 번 걸면 deadlock에 걸릴 수 있다.
* [예제](http://en.cppreference.com/w/cpp/thread/mutex)

### `std::recursive_mutex` (C++11)
* 이미 Mutex를 가지고 있는 스레드가 중첩된 함수 호출 등을 이용해 다시 얻으려고 시도할 때 allow하는 구현체.
* `lock()`을 여러 번 호출해도 deadlock에 걸리지 않지만, `lock()`을 호출한 횟수만큼 `unlock()`을 호출해야 한다.
* 카운트를 체크해야 할 필요가 있기 때문에 덜 효율적이다.

### `std::timed_mutex` (C++11)
* 시간을 이용하여 mutex 소유를 시도할 수 있다. (`try_lock_for()`, `try_lock_until()`)
* OS의 개입이 있기 때문에 `std::mutex`에 비해 latency가 상당히 증가한다.

### `std::recursive_timed_mutex` (C++11)
* `recursive` + `timed`의 기능을 모두 가지고 있다.
* 그래서 매우 비싸다.

Recursive, timed Mutex는 개발자로 하여금 Mutex의 scope를 추론하기 어렵게(`lock()`이 몇 번이나 불렸는지, 현재 시점에서 mutex를 소유하고 있는지 파악이 어려움) 하기 때문에 데드락에 걸릴 확률이 높아질 수 있으니 **꼭 필요할 때만 사용**하자.<br/>
C++14에서는 shared mutex를 위한 `<shared_mutex>` 헤더가 추가되었다.

### shared mutex란?
* `read / write lock`으로도 불린다.
* read / write에 각각 lock을 걸 수 있다.
* write lock은 **1개의 스레드만 접근 가능**하다.
* read lock은 **여러 개의 스레드에서 접근 가능**하다.

### `std::shared_timed_mutex` (C++14)
* 시간적 / 비시간적 접근 모두를 지원하는 `shared mutex`

### `std::shared_mutex` (C++17)
* 기본적인 shared mutex
* [예제](http://en.cppreference.com/w/cpp/thread/shared_mutex)

`shared mutex`는 다수의 write thread / 소수의 read thread 상황에서 `starvation`을 불러올 수 있으니 **필요한 경우에만 사용**하자.<br/>
또한 상황에 맞는, 간단하고 예측 가능한 mutex를 사용하자.

## Locks
C++ 표준 라이브러리는 `<mutex>` 헤더를 통해 **lock 구현체를 제공**한다.

### `std::lock_guard`
* 간단한 `RAII`(Resource acquisition is initialization) 락 구현체
* 생성 시 lock 획득, 소멸 시 lock 해제
* 뮤텍스를 생성자의 파라미터로 받는다.
* [예제](http://en.cppreference.com/w/cpp/thread/lock_guard)

### `std::unique_lock` (C++11)
* 일반적인 목적으로 사용되는 mutex ownership wrapper 클래스
* 아래와 같은 것들을 할 수 있다.
  * deferred locking
  * time-constrained attempts at locking
  * recursive locking
  * transfer of lock ownership
* [예제](http://en.cppreference.com/w/cpp/thread/unique_lock)

C++14에서는 `<Shared_mutex>` 헤더 파일에 shared mutex를 위한 lock이 추가되었다.

### `std::shared_lock` (C++14)
* `unique_lock`과 쓰임새는 같지만 shared_mutex를 위한 ownership wrapper 클래스

## Condition Variables
`<condition_variable>` 헤더를 통해 Java의 `Monitor`, Win32의 `Event 객체`를 사용한 것과 같은 동기화 방법을 C++에서도 적용할 수 있으며, 결과적으로 **스레드 간 통신**을 할 수 있다.

### `std::condition_variable` (C++11)
* `std::unique_lock`을 통해 사용할 수 있는 condition_variable
* [예제](http://en.cppreference.com/w/cpp/thread/condition_variable)

### `std::condition_variable_any` (C++11)
* `shared_lock`에서도 동작한다. (정확히는 `BasicLockable` lock을 구현한 모든 lock에서 동작)
* `condition_variable`보다 **덜 효율적**이다.

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

void cv_example() {
    std::mutex m;
    std::condition_variable cv;
    bool terminate = false;
    int shared_data = 0;
    int counter = 0;

    auto consumer = [&]() {
        std::unique_lock<std::mutex> lk(m);
        do {
            while (!(terminate || shared_data != 0))
                cv.wait(lk);
            if (terminate)
                break;
            std::cout << "consuming " << shared_data << std::endl;
            shared_data = 0;
            cv.notify_one();
        } while (true);
    };

    auto producer = [&]() {
        std::unique_lock<std::mutex> lk(m);
        for (counter = 1; true; ++counter) {
            cv.wait(lk, [&]() { return terminate || shared_data == 0; });
            if (terminate)
                break;
            shared_data = counter;
            std::cout << "producing " << shared_data << std::endl;
            cv.notify_one();
        }
    };

    auto p = std::thread(producer);
    auto c = std::thread(consumer);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    {
        std::lock_guard<std::mutex> l(m);
        terminate = true;
    }
    std::cout << "total items consumed " << counter << std::endl;
    cv.notify_all();
    p.join();
    c.join();
}

int main() {
    cv_example();
    return 0;
}
```

## Atomic Operations on Shared Variables
`<atomic>` 헤더를 통해 memory fence, atomic 연산 등을 사용할 수 있다.

### `std::atomic` (C++11)
* `load()`, `store()`, `is_lock_free()` 등 임의의 data에 대한 atomic operation을 제공한다.
* `std::atomic`의 퍼포먼스는 타겟 프로세서에 따라 다르다.
  * `Intel-architecture PC`에서는 다양한 read-modify-write 명령어들이 있어서, `memory fence`에 따라 cost가 다르다.
  * `단일 코어 프로세서`에서는 **extra code가 전혀 생성되지 않는다.**
  * atomic한 read-modify-write 연산을 지원하지 않는 프로세서에서는, **mutual exclusion을 사용하여 atomic한 연산을 구현**한다.

### Memory fences
* **CPU에 특정 연산의 순서를 강제하게 할 수 있는 기능**이다.

#### Why use memory fences?
* 여러 스레드가 동시에 돌아가는 경우, 코드의 실행 순서가 바뀌어 실행되는 동안 다른 스레드에서 그 부분에 대한 메모리를 접근하여 **잘못된 결과**를 내놓을 수 있기 때문이다.
* `Memory fence`를 만나면, 그전까지의 CPU 코어의 레지스터나 캐시 값의 변경을 메인 메모리로 반영하게 하여 다른 CPU 코어에서 변경된 값을 읽을 수 있도록 하는 것이다.
* `Memory fence`는 **값을 변경하는 CPU 코어 뿐만 아니라, 값을 읽게 되는 CPU 코어도 잘 사용**해야 하는데, 기껏 값을 메인 메모리에 반영해도 읽어가는 CPU 코어가 자신의 레지스터나 캐시에서 읽어 버린다면 소용이 없기 때문이다.

#### Example
```
초기 시작 시, x, y는 0으로 초기화되었다고 가정한다.
```

```cpp
// Thread 1
while (x == 0);
// Point 1
std::cout << y;
```

```cpp
// Thread 2
y = 10;
// Point 2
x = 1;
```

* 스레드 1에서 `std::cout << y` 가 의도대로 `10`을 출력하려면 `x == 0`을 통과해야 한다.
* 스레드 2에서 `y = 10`과 `x = 1`이 의존성이 없는 코드이기 때문에, 컴파일러나 CPU가 연산의 순서를 바꾸는 경우가 있다.
* 마찬가지로, 스레드 1의 `std::cout << y`에서 `y`를 읽어들이는 동작이 `x == 0`에서 `x`의 값을 읽어들이는 동작보다 먼저 일어나는 경우도 있다.
* 그래서, Point 1, 2에 **Memory fence를 설치해 두 명령의 순서를 지키도록 강제**하여 원하는 결과를 얻을 수 있다.

### `std::memory_order`
* 대부분의 `std::atomic` 함수는 `memory_order` 인자를 받을 수 있으며, 지정하지 않을 경우 `acquire`와 `release` 연산에 fence를 설치하는 `memory_order_acq_rel`을 사용한다.
* 아래와 같은 memory_order 옵션으로 읽기/쓰기에 선택적으로 fence를 설치할 수 있다.
  * `memory_order_relaxed`
  * `memory_order_consume`
  * `memory_order_acquire`
  * `memory_order_release`
  * `memory_order_acq_rel`
  * `memory_order_seq_cst`

### Use atomic operation carefully

Atomic access를 남용하는 것은 만병 통치약이 아니다. **memory fence를 설치하는 것은 cost가 비싸**기 때문이다.
예를 들어, 아래 두 코드의 수행 시간은 `약 14배`나 차이가 난다.
  * `Atomic version`: 15,318 millis
  * `Non-atomic version`: 992 millis

```cpp
// Atomic version
typedef unsigned long long counter_t;
std::atomic<counter_t> x;
for (counter_t i = 0, iterations = 10'000'000 * multiplier; i < iterations; ++i)
    x = i;
```

```cpp
// Non-atomic version
typedef unsigned long long counter_t;
counter_t x;
for (counter_t i = 0, iterations = 10'000'000 * multiplier; i < iterations; ++i)
    x = i;
```

x86이 아닌 작은 프로세서라면, mutex를 사용하여 mutual exclusion을 구현하므로 차이는 몇 배나 더 날 수 있다. 그러므로 `std::atomic`의 사용은 **타겟 하드웨어에 대한 학습이 선행**되어야 한다.

## On Deck: Future C++ Concurrency Features
`C++17`에서는 `Coroutine`, `SIMD instruction 지원` 등 concurrency에 대한 강력한 지원이 있을 것이다.

---

# Optimize Threaded C++ Programs

## Prelude: There ain't no such thing as a free lunch
근래에 사용되는 데스크탑 프로세서는 고도화된 아키텍쳐를 사용하고 있으며, 이 프로세서를 효과적으로 사용하기 위해서는 concurrent programming이 필수인데, 그러려면 프로세서의 아키텍쳐를 잘 알고 있어야 한다.

## Prefer `std::async` to `std::thread`
`std::thread`를 사용하면 새로운 스레드를 생성해서 시작하는데, 아래와 같은 직/간접적인 cost로 인해 비싸다.
* OS가 스레드를 위한 table을 만듬
* 메모리와 스택, 레지스터 셋을 할당
* 스레드 실행을 위한 스케쥴 연산
* 컨텍스트 스위칭 발생 시 보존할 메모리 공간 필요
* 다수의 스레드가 있을 때 발생하는 스레싱

아무것도 하지 않는 아래 코드도 10,000번의 thread invocation 발생 시 `1,350 millis`의 시간이 소요된다.

```cpp
std::thread t;
t = std::thread([]() { return; });
t.join();
```

`std::async`를 사용하여 스레드를 계속 생성하지 말고, 재사용하자. 아래 코드는 10,000의 invocation이 발생해도 `86 millis` 밖에 걸리지 않아, 위 코드보다 약 14배 빠르다.

```cpp
std::async(std::launch::async, []() { return });
```

스레드 풀을 직접 짜는 것도 한 방법인데, 그러려면 배보다 배꼽이 크다.
`std::async`로 작성하면, 표준 라이브러리가 개선될 때마다 같이 꿀(?)을 빨 수 있다.

### But...
`std::async` 구현체가 컴파일러마다 다른데, `Visual Studio`의 구현은 잘 되어 있고, 나머지(libc++, libstdc++) 는 **개떡같이**(매 호출마다 `newthread()`를 함) 되어 있다고 한다. [[참조 링크]](http://rpgmakerxp.tistory.com/63)

## Create as Many Runnable Threads as Cores
`std::thread::hardware_concurrency()`는 사용 가능한 코어의 갯수를 리턴하며, 이를 이용하여 **효율적인 스레드의 개수를 추정하고 사용**할 수 있다.<br/>
그러나 많은 개발자들이 만드는 거대한 프로그램이나 3rd-party 라이브러리를 다수 사용하는 프로그램에서는, 프로그램 어딘가에서 수행되고 있는 스레드들이 존재하기 때문에, 현재 가용 스레드의 개수를 추정하기 어려울 수 있음에 유의해야 한다.

## Implement a Task Queue and Thread Pool
`Task-oriented programming`을 이용하면 아래와 같은 장점들이 있다.
  * 효율적으로 I/O 완료 이벤트를 핸들링할 수 있다.
  * 스레드 재생성을 하지 않으므로 짧은 task들의 스레드 시작 시 오버헤드를 줄여준다.
  * 가용 스레드의 개수를 추정하기 더 쉬워진다.

단점으로는 프로그램에 의해 구체적으로 flow를 제어하는 것이 아니라 완료 이벤트 메시지 수신으로 flow가 제어되는, **제어의 역전 현상이 발생하여 디버깅을 어렵게** 만들 수 있다.<br/>
`C++17`에서, 표준 Thread pool과 Task queue를 이용할 수 있으며, 현재는 `Boost library`를 통해 이용할 수 있다.

## Perform I/O in a Separate Thread
XML이나 JSON 데이터를 Rest API를 통해 받아온다고 가정하면, `request - reading - parsing` 등의 작업으로 나눌 수 있다.<br/>
이와 같이 네트워크 연결이나 디스크 접근 등의 I/O 작업은 **필연적으로 딜레이를 발생**시키기 때문에, **다른 스레드에서 Concurrent하게 수행하기 좋은 작업 후보**이다.