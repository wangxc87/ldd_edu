#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>

#define TEST_IOCTL_CMD 0
#define WAKEUP_COMPLETION_CMD 1

/*接收到异步读信号后的动作*/
void input_handler(int signum)
{
  printf("receive a signal from globalfifo,signalnum:%d\n",signum);
}

int main(int argc, char *argv[])
{
    int fd, oflags;
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

    //启动信号驱动机制
    signal(SIGIO, input_handler); //让input_handler()处理SIGIO信号
    fcntl(fd, F_SETOWN, getpid());
    oflags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oflags | FASYNC);


 input0:
    printf("Press 'y' to wakeup completion\n");
    if(getchar() != 'y'){
        printf("***press 'y'****\n");
        goto input0;
    }
        
    
    //ioctrl 访问
    ret = ioctl(fd, WAKEUP_COMPLETION_CMD, NULL);
    if(ret < 0){
        fprintf(stderr, "Ioctl Error.\n");
        return -1;
    }

    close(fd);
    return 0;
}


