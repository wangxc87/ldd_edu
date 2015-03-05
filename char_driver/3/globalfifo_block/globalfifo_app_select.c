/*======================================================================
  A test program in userspace   
  This example is to introduce the ways to use "select"
  and driver poll                 
  ======================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#define FIFO_CLEAR 0x1
#define BUFFER_LEN 128

int main(int argc, char *argv[])
{
    int fd, num;
    char rd_ch[BUFFER_LEN];
    fd_set rfds,wfds;
    struct timeval tv;  
    int ret = 0;
    int i;
    
    /*以非阻塞方式打开/dev/globalmem设备文件*/
    fd = open("/dev/globalfifo", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        printf("Device open failure\n");
    }
  
    /*FIFO清0*/
    if (ioctl(fd, FIFO_CLEAR, 0) < 0)
    {
        printf("ioctl command failed\n");
    }

    while (1)
    {
#define POLL_WRITE
/* #define POLL_READ */
        FD_ZERO(&rfds);
#ifdef POLL_READ
        FD_SET(fd, &rfds);
#endif
        FD_ZERO(&wfds);
#ifdef POLL_WRITE
        FD_SET(fd, &wfds);
#endif
        tv.tv_sec = 1;
        tv.tv_usec = 0;
      
        ret = select(fd + 1, &rfds, &wfds, NULL, &tv);
        switch (ret){
        case  0:
            printf("wait timeout-%d\n", i++);
            break;
        case  -1:
            printf("select Error.\n");
            break;
        default:
            printf("select return %d.\n", ret);
#ifdef POLL_READ
            /*数据可获得*/
            if (FD_ISSET(fd, &rfds))
            {
                printf("Poll monitor:can be read\n");
                ret = read(fd, rd_ch, BUFFER_LEN);
                if(ret < 0){
                    printf("read error.\n");
                    close(fd);
                    return -1;
                }

                printf("read data: ");
                for(i = 0;i < ret; i++){
                    printf("%c", rd_ch[i]);
                }
            }
#endif
#ifdef POLL_WRITE
            /*数据可写入*/
            if (FD_ISSET(fd, &wfds))
            {
                printf("Poll monitor:can be written\n");
            }
#endif
        }
    }

    close(fd);

    return 0;
}
