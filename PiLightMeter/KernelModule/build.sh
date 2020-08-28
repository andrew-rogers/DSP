#!/bin/sh

wget https://raw.githubusercontent.com/raspberrypi/linux/rpi-5.4.y/sound/soc/codecs/adau7002.c

patch -p1 < adau7002.diff

make

sudo cp adau7002.ko /lib/modules/$(uname -r)/kernel/sound/soc/codecs/snd-soc-adau7002.ko

DTO=dtoverlay=adau7002-simple
if [ ! `grep "$DTO" "/boot/config.txt"` ]; then
    cp /boot/config.txt config.txt.orig
    cp config.txt.orig config.txt
    echo "$DTO" >> config.txt
    sudo cp config.txt /boot/config.txt
fi
