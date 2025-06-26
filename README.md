make
sudo insmod custom_gpio.ko
lsmod | grep custom_gpio
dmesg | tail
dmesg | grep "custom_gpio"
ls /dev/custom_gpio
sudo rmmod custom_gpio
sudo chmod 666 /dev/custom_gpio

git clone https://github.com/MinChanJu/custom_gpio.git

cd custom_gpio

make

sudo ./app/sub

sudo ./app/main