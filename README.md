# System Programming
## Custom GPIO

### 설치
```bash
git clone https://github.com/MinChanJu/custom_gpio.git
cd custom_gpio
make
```

### 삭제
```bash
make clean
cd ..
rm -rf custom_gpio/
```

## 프로그램
이 프로그램은 두 개의 Raspberry Pi 간 GPIO 핀을 통해 화재 경보 시스템을 구성하는 시스템 프로그래밍 과제입니다. 한 Pi에서는 버튼 입력을 통해 경보를 발생시키고, 다른 Pi에서는 해당 경보를 수신하여 경고 메시지를 출력합니다.

---

## 📁 파일 구성

- `app/main.c`: 버튼 입력을 감지하고 경보 상태를 전송하는 송신 측 프로그램
- `app/sub.c`: 경보 상태를 수신하고 경고 메시지를 출력하는 수신 측 프로그램
- `app/gpio_api.c` / `app/gpio_api.h`: 커널 드라이버를 쉽게 사용할 수 있도록 만든 사용자 공간용 API

---

## ⚙️ 시스템 구성

- **Raspberry Pi 1 (Sender)**:
  - `main.c` 실행
  - 버튼 입력 → 경보 ON/OFF 전송
- **Raspberry Pi 2 (Receiver)**:
  - `sub.c` 실행
  - GPIO 신호 수신 → 메시지 출력

---

## 🧩 GPIO 핀 연결 예시

- WARNING
  - 송신 Pi GPIO 17 (OUT) - 수신 Pi GPIO 17 (IN)
- MESSAGE
  - 송신 Pi GPIO 27 (IN) - 수신 Pi GPIO 27 (OUT)
- BUTTON
  - 송신 Pi GPIO 21 (IN) - BUTTON - 송신 Pi 5v PWR
- GND
  - 송신 Pi GND - 수신 Pi GND

> ⚠️ 반드시 한쪽은 `OUT`, 다른 쪽은 `IN` 으로 설정되어야 합니다.

---

### 프로그램 실행
- **Raspberry Pi 1 (Sender)**:

  ```bash
  sudo ./app/main
  ```
- **Raspberry Pi 2 (Receiver)**:

  ```bash
  sudo ./app/sub
  ```