# ABEL-OS ( kernel development )

Abel-OS is a very simple educational operating system.

The idea is to learn deep operating systems concepts and apply them in a practical way.

## Project structure

Currently the project is divided in two main directories, these are:

### bootloader/

Here the bootstrap asm files are located. These are currently designed to work on x86 platforms, but in the future ARM or RISC-V options may be added.

### os/

In the **os/** directory you will find the operating system source code. It is divided in multiple subdirectories as follows:

```
os/
|
| - include/
|      |
|      | - arch/...
|      | - kernel/...
|      | - drivers/...
|      | - ...
|
| - src/
|    |
|    | - arch/...
|    | - kernel/...
|    | - drivers/...
|    | - ...
```

As you see there is an src and an include mirror directories, one containes source files and the other the header files.

Further documentation is to be added on the kernel structure. 

## Build 

To build the project you will need:

- make
- gcc
- ld
- nasm

All those tools are used in order to produce the final **disk.img**. 

To compile and build the project, once all dependencies are installes, simply run "**make**" or "**make all**":

```
make 
```

## Run

To run the OS the recommended option is using **qemu**. Once qemu is installed you can use "**make qemu**" and a virtual machine will start booting from **disk.img**. 

You may also want to flash the OS image into a usb drive. To do that you can simply run:

```
dd if=disk.img of=/dev/sdb bs=512 status=progress conv=fsync
``` 

No make command is used for this, but a shell script may be added in the future with this purpose. 


