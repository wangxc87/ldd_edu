#include <stdio.h>
//#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{

    int fd;
    int ret;
    char dev_name[32];
    int char_num = 255;
    int read_nums = 10;
    char buf[64];
    #define WRITE_NUMS 255
#define READ_NUMS 10
    #define LSEEK_POS 20
    /* char char_buf; */
    
    if(argc < 2){
        fprintf(stderr, "Info: %s dev_name \n", argv[0]);
        return -1;
    }
    
    strcpy(dev_name, argv[1]);
    fd = open(dev_name, O_RDONLY);
    if(fd < 0){
        fprintf(stderr, "open %s Error.\n", dev_name);
        return -1;
    }

    int i;
    fprintf(stdout, "***write %d datas Test - 1***\n", char_num);
    for(i = 0; i < char_num; i++){
        ret = write(fd, &i, 1);
        if(ret < 0){
            fprintf(stderr, "write %d Error.\n", i);
            close(fd);
            return -1;
        }
    }
    fprintf(stdout, "***write %d datas success***\n", char_num);
    
    fprintf(stdout, "Now start to read %d datas - 2...\n", read_nums);
    fprintf(stdout, "read data is [index-value]: \n");
    for(i = 0; i < read_nums; i ++){
        ret = read(fd, buf, 1);
        if(ret < 0){
            fprintf(stderr, "read data Error.\n");
            close (fd);
            return -1;
        }
        fprintf(stdout, "%d-0x%x  ", i, buf[0]);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "***llseek to position 0x%x  - 3", LSEEK_POS);
    lseek(fd, LSEEK_POS, SEEK_SET);

    fprintf(stdout, "Now start to read %d datas...\n", read_nums);
    fprintf(stdout, "read data is [index-value]: \n");
    for(i = 0; i < read_nums; i ++){
        ret = read(fd, buf, 1);
        if(ret < 0){
            fprintf(stderr, "read data Error.\n");
            close (fd);
            return -1;
        }
        fprintf(stdout, "%d-0x%x  ", i, buf[0]);
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "Test Over , exit...\n");
    close(fd);

    return 0;
}
