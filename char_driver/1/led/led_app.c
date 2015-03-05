#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>

#define LED1 5
#define LED2 6
#define LED3 7
#define LED4 8

#define LED_OFF 0
#define LED_ON  1

#define LED_SET_STATE_IOCTL_CMD  1
#define LED_GET_STATE_IOCTL_CMD   2
#define LED_SET_ON_IOCTL_CMD 3
#define LED_SET_OFF_IOCTL_CMD 4

struct led_ctrl {
    int led_num;
    int led_state;
};

int main(int argc, char *argv[])
{
    int ret, i;
    int fd;
    char dev_name[32];
    struct led_ctrl led_set;

    if(argc < 2){
        fprintf(stderr, "Info: %s dev_name \n", argv[0]);
        return -1;
    }
    strcpy(dev_name, argv[1]);
    fd = open(dev_name, O_RDONLY);
    if(fd < 0){
        fprintf(stdout, "open %s Error.\n", dev_name);
        return -1;
    }

    while(1){
#if 1
        ret = ioctl(fd, LED_SET_ON_IOCTL_CMD, NULL);
        if(ret < 0){
            fprintf(stderr, "SET_ON_IOCTL_CMD Error.\n");
            goto err_exit;
        }
        sleep(1);
        ret = ioctl(fd, LED_SET_OFF_IOCTL_CMD, NULL);
        if(ret < 0){
            fprintf(stderr, "SET_OFF_IOCTL_CMD Error.\n");
            goto err_exit;
        }
        sleep(1);
#endif

#if 1
        led_set.led_num = LED2;
        led_set.led_state = LED_ON;
        ret = ioctl(fd, LED_SET_STATE_IOCTL_CMD, &led_set);
        if(ret < 0){
            fprintf(stderr, "SET_STATE_IOCTL_CMD Error.\n");
            goto err_exit;
        }
        sleep(1);
        
        led_set.led_num = LED2;
        led_set.led_state = LED_OFF;
        ret = ioctl(fd, LED_SET_STATE_IOCTL_CMD, &led_set);
        if(ret < 0){
            fprintf(stderr, "SET_STATE_IOCTL_CMD Error.\n");
            goto err_exit;
        }
        sleep(1);
#endif
        fprintf(stdout, "led_app test %d \n", i++);
    }
    close(fd);
    return 0;

 err_exit:
    close(fd);
    return -1;
}
