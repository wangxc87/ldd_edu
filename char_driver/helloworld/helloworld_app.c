#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define HELLOWORLD_IOCTL_CMD 0

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    char dev_name[64];
    if(argc < 2){
        fprintf(stderr, "Info: %s  dev_name \n", argv[0]);
        return -1;
    }

    strcpy(dev_name, argv[1]);

    //打开设备
    fd = open(dev_name, O_RDONLY);
    if(fd < 0){
        fprintf(stderr,"Open %s Error.\n", dev_name);
        return -1;
    }

    //ioctrl 访问
    ret = ioctl(fd, HELLOWORLD_IOCTL_CMD, NULL);
    if(ret < 0){
        fprintf(stderr, "Ioctl Error.\n");
        return -1;
    }

    close(fd);
    return 0;
}


