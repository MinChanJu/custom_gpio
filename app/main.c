#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#define GPIO_PATH "/sys/class/sysprog_gpio"

#define MAX_BUFFER 3
#define MAX_PATH 50

#define IN 1        // in을 1로 처리
#define OUT 0       // out을 0으로 처리
#define WARNING 17  // gpio17에 연결
#define BUTTON 21   // gpio21에 연결
#define MESSAGE 27  // gpio27에 연결

int GPIOExport(int pin);             // pin 번호에 대한 gpio 설정
int GPIOUnexport(int pin);           // pin 번호에 대한 gpio 해제
int GPIODirection(int pin, int dir); // pin 번호에 대한 gpio의 direction을 dir에 따라 설정
int GPIORead(int pin);               // pin 번호에 대한 gpio의 value 읽음
int GPIOWrite(int pin, int value);   // pin 번호에 대한 gpio의 value에 value 적용
void handle_signal(int sig);         // sig 신호를 받으면 모든 pin에 대해 unexport 해주는 핸들러

int main()
{
  // Ctrl+C(SIGINT) 핸들러 등록
  signal(SIGINT, handle_signal);

  // Button과 WARNINGd의 GPIO export 및 방향 설정
  if (GPIOExport(BUTTON) == -1 || GPIOExport(WARNING) == -1 || GPIOExport(MESSAGE) == -1)
    return -1;
  if (GPIODirection(BUTTON, IN) == -1 || GPIODirection(WARNING, OUT) == -1 || GPIODirection(MESSAGE, IN) == -1)
    return -1;

  int state = 0;                    // 현재 led 상탱 (0: 꺼짐, 1: 켜짐)
  int prev_button = 0;              // 이전 버튼 상태 (0: 안 눌림, 1: 눌림)
  int curr_button = 0;              // 현재 버튼 상태 (0: 안 눌림, 1: 눌림)
  int sub_state = 0;
  
  // LED 초기 상태는 OFF
  if (GPIOWrite(WARNING, state) == -1)
    return -1;
  fprintf(stdout, "화재 경보 시스템 시작\n");

  while (1)
  {
    curr_button = GPIORead(BUTTON);
    sub_state = GPIORead(MESSAGE);

    if (sub_state != state) {
      fprintf(stdout, "뭔가 이상함 Message: %d, state: %d\n", sub_state, state);
      // return -1;
    }

    if (prev_button == 1 && curr_button == 0)
    { // 버튼에서 짧게 누르고 손을 뗀 경우
      state = !state;
      if (GPIOWrite(WARNING, state) == -1)
        return -1;
      fprintf(stdout, "Button 누름 - 화재 경보 %s\n", state ? "ON" : "OFF");
    }

    prev_button = curr_button;
    usleep(10 * 1000); // 10ms
  }

  handle_signal(0);
  return 0;
}

int GPIOExport(int pin)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/export", GPIO_PATH); // export 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed toopen export for writing: %s\n", path);
    perror("Error");
    return -1;
  }

  char buffer[MAX_BUFFER];
  ssize_t bytes_written;
  bytes_written = snprintf(buffer, MAX_BUFFER, "%d", pin);

  write(fd, buffer, bytes_written); // 파일에 pin을 입력
  close(fd);
  return 0;
}

int GPIOUnexport(int pin)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/unexport", GPIO_PATH); // unexport 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    perror("Failed to open unexport for writing");
    return -1;
  }

  char buffer[MAX_BUFFER];
  ssize_t bytes_written;
  bytes_written = snprintf(buffer, MAX_BUFFER, "%d", pin);

  write(fd, buffer, bytes_written); // 파일에 pin을 입력
  close(fd);
  return 0;
}

int GPIODirection(int pin, int dir)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/gpio%d/direction", GPIO_PATH, pin); // direction 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to open direction for writing: %s\n", path);
    perror("Error");
    return -1;
  }

  const char *dir_str = (dir == IN) ? "in" : "out";

  write(fd, dir_str, strlen(dir_str)); // 파일에 dir을 입력
  close(fd);
  return 0;
}

int GPIORead(int pin)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_PATH, pin); // value 주소

  int fd = open(path, O_RDONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to open value for reading: %s\n", path);
    perror("Error");
    return -1;
  }

  char value_str[3];
  if (read(fd, value_str, 3) == -1) // 파일 읽기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to read value: %s\n", path);
    perror("Error");
    close(fd);
    return -1;
  }

  close(fd);
  return atoi(value_str); // 읽은 값을 정수로 반환
}

int GPIOWrite(int pin, int value)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_PATH, pin); // value 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to open value for writing: %s\n", path);
    perror("Error");
    return -1;
  }

  char val_str[2];
  snprintf(val_str, 2, "%d", value);

  write(fd, val_str, 1); // 파일에 value를 입력
  close(fd);
  return 0;
}

void handle_signal(int sig)
{
  fprintf(stdout, "\nCtrl+C 감지됨. GPIO 정리 중...\n");

  if (GPIOUnexport(WARNING) == -1)
    fprintf(stderr, "LED unexport 실패\n");
  else
    fprintf(stdout, "LED unexport 성공\n");

  if (GPIOUnexport(BUTTON) == -1)
    fprintf(stderr, "BUTTON unexport 실패\n");
  else
    fprintf(stdout, "BUTTON unexport 성공\n");

  fprintf(stdout, "화재경보 시스템 종료\n");
  exit(0); // 정상 종료
}