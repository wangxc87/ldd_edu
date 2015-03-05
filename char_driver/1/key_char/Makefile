
CUR_DIR = $(shell pwd)

#MINI2440_DIR =/home/wxc/work/sdb/mini
MINI2440_DIR =/home/wxc/dsp/ex_part/mini

ROOTFS_DIR =$(MINI2440_DIR)/rootfs_qtopia_qt4
EXE_DIR =$(ROOTFS_DIR)/home

ifeq ($(p),)
KERNEL_DIR = $(MINI2440_DIR)/linux-2.6.32.2
else
LINUXKERNEL_INSTALL_DIR :=/home/wxc/dsp/DVRRDK_04.01.00.02/ti_tools/linux_lsp/kernel/linux-dvr-rdk
KERNEL_DIR := $(LINUXKERNEL_INSTALL_DIR)
endif

CC_CFLAGS = -I $(KERNEL_DIR)/include
CC = arm-linux-gcc

ARCH = arm
CROSS_COMPILE = arm-linux-
export ARCH
export CROSS_COMPILE

MODULE_DEV :=key_char
APP := key_char_app


obj-m := $(MODULE_DEV).o
#obj-m += $(MODULE_DEV)_nums.o
#module-objs := $(MODULE_DEV).o
PWD :=$(shell pwd)


.PHONY: all clean install

all: module app

module:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

app:
	$(CC) $(APP).c -o $(APP) $(CC_CFLAGS)

clean:
	rm -f *.o *.ko *.order *mod.c *.symve* *_app

