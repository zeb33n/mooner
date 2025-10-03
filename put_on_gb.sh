set -e
set -x
udisksctl mount -b /dev/sda1
cp mooner.elf /media/zebbond/6A7D-8DE9
sudo eject /media/zebbond/6A7D-8DE9
