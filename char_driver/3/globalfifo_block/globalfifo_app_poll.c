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
#include <poll.h>

#define FIFO_CLEAR 0x1
#define BUFFER_LEN 128

int main(int argc, char *argv[])
{
    int fd, num;
    char rd_ch[BUFFER_LEN];
    struct timeval tv;  
    struct pollfd poll_fds[1];
    int ret;
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
    poll_fds[0].fd = fd;
    poll_fds[0].events = 0;

#define POLL_WRITE
#define POLL_READ

#ifdef POLL_WRITE
    poll_fds[0].events |= POLLOUT;
#endif

#ifdef POLL_READ
        poll_fds[0].events |= POLLIN;
#endif
    while (1)
    {
        ret = poll(poll_fds, 1, 1000); //wait 1s
        switch (ret){
        case   0:
            printf("wait timeout-%d.\n", i++);
            break;
        case  -1:
            printf("poll Error.\n");
            break;
        default:
            /*数据可获得*/
            if (poll_fds[0].revents & POLLIN)
            {
                printf("Poll monitor:can be read\n");
                /* ret = read(fd, rd_ch, BUFFER_LEN); */
                /* if(ret < 0){ */
                /*     printf("read error.\n"); */
                /*     close(fd); */
                /*     return -1; */
                /* } */

                /* printf("read data: "); */
                /* for(i = 0;i < ret; i++){ */
                /*     printf("%c", rd_ch[i]); */
                /* } */
            }

            /*数据可写入*/
            if (poll_fds[0].revents & POLLOUT)
            {
                printf("Poll monitor:can be written\n");
            }
        }
    }

    close(fd);

    return 0;
}
