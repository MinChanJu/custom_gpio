/* system programming custom gpio driver (multi pin supported) */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/slab.h>

#define CLASS_NAME "custom_gpio"
#define DEVICE_NAME "custom_gpio"
#define MAX_GPIO 10
#define GPIOCHIP_BASE 512

#define GPIO_IOCTL_MAGIC 'G'
#define GPIO_SET_DIR _IOW(GPIO_IOCTL_MAGIC, 0, struct gpio_ctrl)
#define GPIO_SET_VALUE _IOW(GPIO_IOCTL_MAGIC, 1, struct gpio_ctrl)
#define GPIO_GET_VALUE _IOR(GPIO_IOCTL_MAGIC, 2, struct gpio_ctrl)

#define IN 1
#define OUT 0

struct gpio_ctrl
{
  int pin;
  int value;
};

struct gpio_entry
{
  int bcm;
  struct gpio_desc *desc;
};

static dev_t dev_num;
static struct cdev gpio_cdev;
static struct class *gpio_class;
static struct gpio_entry *gpio_list[MAX_GPIO];

static int find_gpio_index(int bcm)
{
  for (int i = 0; i < MAX_GPIO; i++)
  {
    if (gpio_list[i] && gpio_list[i]->bcm == bcm)
      return i;
  }
  return -1;
}

static int register_gpio(int bcm)
{
  int i = find_gpio_index(bcm);
  if (i >= 0)
    return i;

  for (i = 0; i < MAX_GPIO; i++)
  {
    if (!gpio_list[i])
    {
      gpio_list[i] = kzalloc(sizeof(struct gpio_entry), GFP_KERNEL);
      if (!gpio_list[i])
        return -ENOMEM;
      gpio_list[i]->bcm = bcm;
      gpio_list[i]->desc = gpio_to_desc(GPIOCHIP_BASE + bcm);
      if (!gpio_list[i]->desc)
      {
        kfree(gpio_list[i]);
        gpio_list[i] = NULL;
        return -EINVAL;
      }
      return i;
    }
  }
  return -ENOMEM;
}

static long gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  struct gpio_ctrl ctrl;
  int idx;
  if (copy_from_user(&ctrl, (void __user *)arg, sizeof(ctrl)))
    return -EFAULT;

  idx = register_gpio(ctrl.pin);
  if (idx < 0)
    return idx;

  switch (cmd)
  {
  case GPIO_SET_DIR:
    if (ctrl.value == IN)
      gpiod_direction_input(gpio_list[idx]->desc);
    else
      gpiod_direction_output(gpio_list[idx]->desc, 0);
    break;
  case GPIO_SET_VALUE:
    gpiod_set_value(gpio_list[idx]->desc, ctrl.value);
    break;
  case GPIO_GET_VALUE:
    ctrl.value = gpiod_get_value(gpio_list[idx]->desc);
    if (copy_to_user((void __user *)arg, &ctrl, sizeof(ctrl)))
      return -EFAULT;
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

static struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = gpio_ioctl,
};

static int __init custom_gpio_init(void)
{
  if (alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME) < 0)
    return -1;
  cdev_init(&gpio_cdev, &gpio_fops);
  if (cdev_add(&gpio_cdev, dev_num, 1) < 0)
    return -1;
  gpio_class = class_create(CLASS_NAME);
  if (IS_ERR(gpio_class))
    return PTR_ERR(gpio_class);
  device_create(gpio_class, NULL, dev_num, NULL, DEVICE_NAME);
  pr_info("[custom_gpio] module loaded\n");
  return 0;
}

static void __exit custom_gpio_exit(void)
{
  for (int i = 0; i < MAX_GPIO; i++)
  {
    if (gpio_list[i])
      kfree(gpio_list[i]);
  }
  device_destroy(gpio_class, dev_num);
  class_destroy(gpio_class);
  cdev_del(&gpio_cdev);
  unregister_chrdev_region(dev_num, 1);
  pr_info("[custom_gpio] module unloaded\n");
}

module_init(custom_gpio_init);
module_exit(custom_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Minchan");
MODULE_DESCRIPTION("Multi-Pin Custom GPIO Driver");
