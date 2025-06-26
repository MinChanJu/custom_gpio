#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>
#include "gpio_ioctl.h" 

#define WARNING 17
#define BUTTON 21
#define MESSAGE 27

int fd; // 디바이스 파일 디스크립터

struct gpio_ctrl warning_gpio = {WARNING, OUT};
struct gpio_ctrl button_gpio = {BUTTON, IN};
struct gpio_ctrl message_gpio = {MESSAGE, IN};

void handle_signal(int sig)
{
  printf("\nCtrl+C 감지됨. 프로그램 종료 중...\n");
  close(fd);
  exit(0);
}

int main()
{
  signal(SIGINT, handle_signal); // Ctrl+C 핸들러

  fd = open(DEVICE_PATH, O_RDWR);
  if (fd < 0)
  {
    perror("device open");
    return 1;
  }

  // 핀 방향 설정
  if (ioctl(fd, GPIO_SET_DIR, &warning_gpio) < 0 ||
      ioctl(fd, GPIO_SET_DIR, &button_gpio) < 0 ||
      ioctl(fd, GPIO_SET_DIR, &message_gpio) < 0)
  {
    perror("gpio set dir");
    close(fd);
    return 1;
  }

  int state = 0;
  int prev_button = 0;
  int curr_button = 0;
  int sub_state = 0;

  // 초기 상태 OFF
  warning_gpio.value = state;
  if (ioctl(fd, GPIO_SET_VALUE, &warning_gpio) < 0)
  {
    perror("gpio set init value");
    close(fd);
    return 1;
  }

  printf("화재 경보 시스템 시작\n");

  while (1)
  {
    // 버튼 상태 읽기
    if (ioctl(fd, GPIO_GET_VALUE, &button_gpio) < 0)
    {
      perror("gpio read button");
      break;
    }
    curr_button = button_gpio.value;

    // message 핀 상태 읽기
    if (ioctl(fd, GPIO_GET_VALUE, &message_gpio) < 0)
    {
      perror("gpio read message");
      break;
    }
    sub_state = message_gpio.value;

    if (sub_state != state)
    {
      printf("뭔가 이상함 Message: %d, state: %d\n", sub_state, state);
      // return -1;
    }

    // 버튼 눌렀다가 뗀 순간
    if (prev_button == 1 && curr_button == 0)
    {
      state = !state;
      warning_gpio.value = state;
      if (ioctl(fd, GPIO_SET_VALUE, &warning_gpio) < 0)
      {
        perror("gpio set warning");
        break;
      }
      printf("Button 누름 - 화재 경보 %s\n", state ? "ON" : "OFF");
    }

    prev_button = curr_button;
    usleep(10 * 1000); // 10ms
  }

  close(fd);
  return 0;
}