#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 

#define GPIO_PATH "/sys/class/sysprog_gpio"

#define MAX_BUFFER 3
#define MAX_PATH 50

#define IN 1      // in을 1로 처리
#define OUT 0     // out을 0으로 처리

int GPIOExport(int pin);
int GPIODirection(int pin, int dir);
int GPIOWrite(int pin, int value);
int GPIORead(int pin);

#endif