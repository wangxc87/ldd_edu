
CUR_DIR = $(shell pwd)

#MINI2440_DIR =/home/wxc/work/sdb/mini
MINI2440_DIR =/home/wxc/mini

ROOTFS_DIR =$(MINI2440_DIR)/rootfs_qtopia_qt4
EXE_DIR =$(ROOTFS_DIR)/home

KERNEL_DIR = $(MINI2440_DIR)/linux-2.6.32.2
#CROSS_COMPILE_DIR = /opt/

CC_CFLAGS = -I $(KERNEL_DIR)/include
CC = arm-linux-gcc

ARCH = arm
CROSS_COMPILE = arm-linux-
export ARCH
export CROSS_COMPILE

MODULE_DEV :=helloworld

obj-m := $(MODULE_DEV).o
module-objs := $(MODULE_DEV).o
PWD :=$(shell pwd)

APP := $(MODULE_DEV)_app

.PHONY: all clean install

all: module app

module:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

app:
	$(CC) $(APP).c -o $(APP) $(CC_CFLAGS)

install:
	cp $(APP) $(MODULE_DEV).ko $(EXE_DIR)

clean:
	rm -f *.o *.ko *.order

