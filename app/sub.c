#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "gpio_ioctl.h" 

#define WARNING 17  // 입력으로 사용하는 센서 핀
#define MESSAGE 21  // 출력으로 사용할 알림 핀

int fd;

struct gpio_ctrl warning_gpio = {WARNING, IN};
struct gpio_ctrl message_gpio = {MESSAGE, OUT};

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
      ioctl(fd, GPIO_SET_DIR, &message_gpio) < 0)
  {
    perror("gpio set direction");
    close(fd);
    return 1;
  }

  int prev_state = 0;
  int curr_state = 0;

  // 초기 알림 메시지 OFF
  message_gpio.value = 0;
  if (ioctl(fd, GPIO_SET_VALUE, &message_gpio) < 0)
  {
    perror("gpio init write");
    close(fd);
    return 1;
  }

  printf("화재 경보 시스템 시작\n");

  while (1)
  {
    // 센서 값 읽기
    if (ioctl(fd, GPIO_GET_VALUE, &warning_gpio) < 0)
    {
      perror("gpio read");
      break;
    }
    curr_state = warning_gpio.value;

    if (prev_state == 0 && curr_state == 1)
    {
      printf("화재 발생 대피 바람\n");
      message_gpio.value = 1;
      if (ioctl(fd, GPIO_SET_VALUE, &message_gpio) < 0)
        break;
    }
    else if (prev_state == 1 && curr_state == 0)
    {
      printf("화재 경보 꺼짐 안심하세요\n");
      message_gpio.value = 0;
      if (ioctl(fd, GPIO_SET_VALUE, &message_gpio) < 0)
        break;
    }

    prev_state = curr_state;
    usleep(10 * 1000); // 10ms
  }

  close(fd);
  return 0;
}