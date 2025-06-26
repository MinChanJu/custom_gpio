#ifndef GPIO_IOCTL_H
#define GPIO_IOCTL_H

#include <linux/ioctl.h>  // 커널 모듈에서는 필요 (사용자 공간에서도 안전)

#define GPIO_IOCTL_MAGIC 'G'
#define DEVICE_PATH "/dev/custom_gpio"

#define IN  1
#define OUT 0

struct gpio_ctrl {
    int pin;   // GPIO 번호 (BCM 기준)
    int value; // IN/OUT 또는 HIGH/LOW 값
};

// IOCTL 명령 정의
#define GPIO_SET_DIR   _IOW(GPIO_IOCTL_MAGIC, 0, struct gpio_ctrl)
#define GPIO_SET_VALUE _IOW(GPIO_IOCTL_MAGIC, 1, struct gpio_ctrl)
#define GPIO_GET_VALUE _IOR(GPIO_IOCTL_MAGIC, 2, struct gpio_ctrl)

#endif