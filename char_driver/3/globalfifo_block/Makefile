
#MINI2440_DIR =/home/wxc/work/sdb/mini
MINI2440_DIR =/home/wxc/mini

include $(MINI2440_DIR)/env.mk

ifeq ($(p), )
KERNEL_DIR = $(MINI2440_DIR)/linux-2.6.32.2
ROOTFS_DIR =$(MINI2440_DIR)/rootfs_qtopia_qt4
EXE_DIR =$(ROOTFS_DIR)/home
else
LINUXKERNEL_INSTALL_DIR :=/home/wxc/dsp/DVRRDK_04.01.00.02/ti_tools/linux_lsp/kernel/linux-dvr-rdk
KERNEL_DIR := $(LINUXKERNEL_INSTALL_DIR)
ROOTFS_DIR =/home/wxc/dsp/DVRRDK_04.01.00.02/target/rfs_816x
EXE_DIR =$(ROOTFS_DIR)/home/root/mini_app
endif

CC_CFLAGS = -I $(KERNEL_DIR)/include
CC = arm-linux-gcc

ARCH = arm
CROSS_COMPILE = arm-linux-
export ARCH
export CROSS_COMPILE

MODULE_DEV :=globalfifo

obj-m := $(MODULE_DEV).o
#obj-m += $(MODULE_DEV)_two.o
#module-objs := $(MODULE_DEV).o
PWD :=$(shell pwd)

APP1 := $(MODULE_DEV)_app_select
APP2 := $(MODULE_DEV)_app_poll

.PHONY: all clean install

all: module app

module:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

app:
	$(CC) $(APP1).c -o $(APP1) $(CC_CFLAGS)
	$(CC) $(APP2).c -o $(APP2) $(CC_CFLAGS)


install:
	mkdir -p $(EXE_DIR)
	cp $(APP1) $(APP2) $(MODULE_DEV).ko $(EXE_DIR)

clean:
	rm -f *.o *.ko *.order *mod.c *.symve* *_app

