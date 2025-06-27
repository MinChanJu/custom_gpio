#ifndef GPIO_API_H
#define GPIO_API_H

#define DEVICE_PATH "/dev/custom_gpio"

#define IN 1
#define OUT 0

struct gpio_ctrl {
  int pin;
  int value;
};

#define GPIO_IOCTL_MAGIC 'G'
#define GPIO_SET_DIR _IOW(GPIO_IOCTL_MAGIC, 0, struct gpio_ctrl)
#define GPIO_SET_VALUE _IOW(GPIO_IOCTL_MAGIC, 1, struct gpio_ctrl)
#define GPIO_GET_VALUE _IOR(GPIO_IOCTL_MAGIC, 2, struct gpio_ctrl)

// API 함수
int gpio_init();                       // fd 열기
void gpio_cleanup();                    // fd 닫기

int gpio_set_direction(int pin, int direction);
int gpio_write(int pin, int value);
int gpio_read(int pin);

#endif