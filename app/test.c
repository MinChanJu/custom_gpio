#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE_PATH "/dev/custom_gpio"

#define GPIO_IOCTL_MAGIC 'G'
#define GPIO_SET_DIR _IOW(GPIO_IOCTL_MAGIC, 0, struct gpio_ctrl)
#define GPIO_SET_VALUE _IOW(GPIO_IOCTL_MAGIC, 1, struct gpio_ctrl)
#define GPIO_GET_VALUE _IOR(GPIO_IOCTL_MAGIC, 2, struct gpio_ctrl)

#define IN 1
#define OUT 0

struct gpio_ctrl
{
  int pin;   // 핀 번호
  int value; // 방향(IN/OUT) or 상태(0/1)
};

int main()
{
  int fd = open(DEVICE_PATH, O_RDWR);
  if (fd < 0)
  {
    perror("device open");
    return 1;
  }

  struct gpio_ctrl led = {17, OUT}, button = {21, IN};
  if (ioctl(fd, GPIO_SET_DIR, &led) < 0 || ioctl(fd, GPIO_SET_DIR, &button) < 0)
  {
    perror("set LED or BUTTON direction");
    close(fd);
    return 1;
  }
  
  while (1)
  {
    if (ioctl(fd, GPIO_GET_VALUE, &button) < 0)
    {
      perror("get button value");
      break;
    }

    led.value = button.value;
    if (ioctl(fd, GPIO_SET_VALUE, &led) < 0)
    {
      perror("set led value");
      break;
    }

    printf("Button: %d\n", button.value);
    usleep(100000); // 100ms
  }

  close(fd);
  return 0;
}