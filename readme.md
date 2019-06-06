#RaspberryPI Bootloader

##a simple bootloader for raspberry to avoid sdcard plug and unplug,can download hex file or img file.both can get from elf file through objcopy

    img: $(ARMGNU)-objcopy kernel8.elf -O binary kernel8.img
    hex: $(ARMGNU)-objcopy kernel8.elf -O ihex   kernel8.img

build:

binary:

    make clean
    make
hex:

    make clean
    make TYPE=hex

usage:

    raspiloader-img /dev/ttyUSBx test.img
    raspiloader-hex /dev/ttyUSBx test.hex
    
    ctrl+d to exit.
