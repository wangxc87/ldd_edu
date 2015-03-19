MINI2440_DIR =/home/wxc/mini

include $(MINI2440_DIR)/env.mk

ROOTFS_DIR =$(MINI2440_DIR)/rootfs_qtopia_qt4

ifeq ($(p), )
KERNEL_DIR = $(MINI2440_DIR)/linux-2.6.32.2
EXE_DIR =$(ROOTFS_DIR)/home
else
LINUXKERNEL_INSTALL_DIR :=/home/wxc/dsp/DVRRDK_04.01.00.02/ti_tools/linux_lsp/kernel/linux-dvr-rdk
KERNEL_DIR := $(LINUXKERNEL_INSTALL_DIR)
EXE_DIR = /rfs_816x/home/root/mini_app/
endif

CC_CFLAGS = -I $(KERNEL_DIR)/include
CC = arm-linux-gcc

ARCH = arm
CROSS_COMPILE = arm-linux-
export ARCH
export CROSS_COMPILE

APP := fbdev_app

.PHONY: all clean install

all:  app

app:
	$(CC) $(APP).c -o $(APP) $(CC_CFLAGS)

install:
	cp *_app   $(EXE_DIR)

clean:
	rm -f *.o *.ko *.order *mod.c *.symve* *_app
