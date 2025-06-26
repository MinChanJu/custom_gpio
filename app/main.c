#include <stdio.h>
#include <unistd.h>
#include "gpio_api.h"

#define WARNING 17
#define BUTTON 21
#define MESSAGE 27

int main() {
  if (gpio_init() < 0) return 1;

  // 핀 방향 설정
  if (gpio_set_direction(WARNING, OUT) < 0 ||
      gpio_set_direction(BUTTON, IN) < 0 ||
      gpio_set_direction(MESSAGE, IN) < 0) {
    perror("gpio set dir");
    gpio_cleanup();
    return 1;
  }

  int state = 0;
  int prev_button = 0;

  gpio_write(WARNING, state); // 초기 상태 OFF
  printf("화재 경보 시스템 시작\n");

  while (1) {
    int curr_button = gpio_read(BUTTON);
    int sub_state = gpio_read(MESSAGE);

    if (curr_button < 0 || sub_state < 0) {
      perror("gpio read");
      break;
    }

    if (sub_state != state) {
      printf("뭔가 이상함 Message: %d, state: %d\n", sub_state, state);
    }

    // 버튼 눌렀다가 뗀 순간
    if (prev_button == 1 && curr_button == 0) {
      state = !state;
      if (gpio_write(WARNING, state) < 0) {
        perror("gpio set warning");
        break;
      }
      printf("Button 누름 - 화재 경보 %s\n", state ? "ON" : "OFF");
    }

    prev_button = curr_button;
    usleep(10 * 1000); // 10ms
  }

  gpio_cleanup();
  return 0;
}