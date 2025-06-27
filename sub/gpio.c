#include "gpio.h"

int GPIOExport(int pin)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/export", GPIO_PATH); // export 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed toopen export for writing: %s\n", path);
    perror("Error");
    return -1;
  }

  char buffer[MAX_BUFFER];
  ssize_t bytes_written;
  bytes_written = snprintf(buffer, MAX_BUFFER, "%d", pin);

  write(fd, buffer, bytes_written); // 파일에 pin을 입력
  close(fd);
  return 0;
}

int GPIOUnexport(int pin)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/unexport", GPIO_PATH); // unexport 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    perror("Failed to open unexport for writing");
    return -1;
  }

  char buffer[MAX_BUFFER];
  ssize_t bytes_written;
  bytes_written = snprintf(buffer, MAX_BUFFER, "%d", pin);

  write(fd, buffer, bytes_written); // 파일에 pin을 입력
  close(fd);
  return 0;
}

int GPIODirection(int pin, int dir)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/gpio%d/direction", GPIO_PATH, pin); // direction 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to open direction for writing: %s\n", path);
    perror("Error");
    return -1;
  }

  const char *dir_str = (dir == IN) ? "in" : "out";

  write(fd, dir_str, strlen(dir_str)); // 파일에 dir을 입력
  close(fd);
  return 0;
}

int GPIORead(int pin)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_PATH, pin); // value 주소

  int fd = open(path, O_RDONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to open value for reading: %s\n", path);
    perror("Error");
    return -1;
  }

  char value_str[3];
  if (read(fd, value_str, 3) == -1) // 파일 읽기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to read value: %s\n", path);
    perror("Error");
    close(fd);
    return -1;
  }

  close(fd);
  return atoi(value_str); // 읽은 값을 정수로 반환
}

int GPIOWrite(int pin, int value)
{
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_PATH, pin); // value 주소

  int fd = open(path, O_WRONLY);
  if (fd == -1) // 파일 열기에 실패할 경우 에러 출력
  {
    fprintf(stderr, "Failed to open value for writing: %s\n", path);
    perror("Error");
    return -1;
  }

  char val_str[2];
  snprintf(val_str, 2, "%d", value);

  write(fd, val_str, 1); // 파일에 value를 입력
  close(fd);
  return 0;
}