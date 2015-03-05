#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>  //ioctl cmd headfile
#include <string.h>

#include "key_char.h"


int main(int argc, char *argv[])
{
    int ret, i;
    int fd;
    char dev_name[32];
    struct key_ctrl key_set;
    int press_times = 0;

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
        key_set.key_num = KEY1_OFFSET;
        ret = ioctl(fd, KEY_GET_STATE_IOCTL_CMD, &key_set);
        if(ret < 0){
            fprintf(stderr, "ioctl Error.\n");
            close(fd);
            return -1;
        }

        switch (key_set.key_state) {
        case KEY_PRESSED :
            press_times ++;
            fprintf(stdout, "The key%d is Pressed-%d.\n",key_set.key_num, press_times);
            break;
        case KEY_NOT_PRESSED :
            fprintf(stdout, "The key%d not Pressed.\n", key_set.key_num);
            break;
        default:
            fprintf(stderr, "invalid key%d state.\n", key_set.key_num);
        }
        sleep(1);
    }

    close(fd);
    fprintf(stdout, "Test run Over, EXIT.\n");
    return 0;
}

