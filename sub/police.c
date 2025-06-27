#include "gpio.h"

#define RED 27 // gpio21에 연결
#define BLUE 17    // gpio17에 연결

int main() {
  GPIOExport(RED);
  GPIOExport(BLUE);

  GPIODirection(RED, OUT);
  GPIODirection(BLUE, OUT);

  GPIOWrite(RED, 0);
  GPIOWrite(BLUE, 0);

  int state = 1;
  short idx = 1;

  while (1) {
    state = !state;
    GPIOWrite(RED, state);
    GPIOWrite(BLUE, !state);
    printf("%3d RED %s, BLUE %s\n", idx++, state == 1 ? "켜짐" : "꺼짐", !state == 1 ? "켜짐" : "꺼짐");
    if (idx >= 1000) idx = 1;
    usleep(500 * 1000);
  }

  return 0;
}