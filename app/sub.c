#include <stdio.h>
#include <unistd.h>
#include "gpio_api.h"

#define WARNING 17
#define MESSAGE 27

int main() {
  if (gpio_init() < 0) return 1;

  // 핀 방향 설정
  if (gpio_set_direction(WARNING, IN) < 0 ||
      gpio_set_direction(MESSAGE, OUT) < 0) {
    perror("gpio set direction");
    gpio_cleanup();
    return 1;
  }

  int prev_state = 0;
  int curr_state = 0;

  gpio_write(MESSAGE, 0); // 초기 알림 OFF
  printf("화재 경보 시스템 시작\n");

  while (1) {
    curr_state = gpio_read(WARNING);
    if (curr_state < 0) {
      perror("gpio read");
      break;
    }

    if (prev_state == 0 && curr_state == 1) {
      printf("화재 발생 대피 바람\n");
      if (gpio_write(MESSAGE, 1) < 0) break;
    }
    else if (prev_state == 1 && curr_state == 0) {
      printf("화재 경보 꺼짐 안심하세요\n");
      if (gpio_write(MESSAGE, 0) < 0) break;
    }

    prev_state = curr_state;
    usleep(10 * 1000); // 10ms
  }

  gpio_cleanup();
  return 0;
}