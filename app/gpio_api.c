#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include "gpio_api.h"

static int gpio_fd = -1;

static void handle_sigint(int sig)
{
  printf("\n[API] Ctrl+C 감지됨. 종료 중...\n");
  gpio_cleanup();
  exit(0);
}

int gpio_init()
{
  gpio_fd = open(DEVICE_PATH, O_RDWR);
  if (gpio_fd < 0)
  {
    perror("open");
    return -1;
  }
  signal(SIGINT, handle_sigint); // SIGINT 핸들러 등록
  return 0;
}

void gpio_cleanup()
{
  if (gpio_fd >= 0)
  {
    close(gpio_fd);
    gpio_fd = -1;
    printf("[API] 디바이스 닫힘\n");
  }
}

int gpio_set_direction(int pin, int direction)
{
  if (gpio_fd < 0)
    return -1;
  struct gpio_ctrl ctrl = {.pin = pin, .value = direction};
  return ioctl(gpio_fd, _IOW('G', 0, struct gpio_ctrl), &ctrl);
}

int gpio_write(int pin, int value)
{
  if (gpio_fd < 0)
    return -1;
  struct gpio_ctrl ctrl = {.pin = pin, .value = value};
  return ioctl(gpio_fd, _IOW('G', 1, struct gpio_ctrl), &ctrl);
}

int gpio_read(int pin)
{
  if (gpio_fd < 0)
    return -1;
  struct gpio_ctrl ctrl = {.pin = pin};
  if (ioctl(gpio_fd, _IOR('G', 2, struct gpio_ctrl), &ctrl) < 0)
    return -1;
  return ctrl.value;
}