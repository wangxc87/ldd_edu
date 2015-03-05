#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
static int test_done = 0;
void sigterm_handler(int signo) 
{ 
    printf("Have caught sig N.O. %d\n", signo);
    switch(signo){
    case SIGINT:
        printf("[ctrl+c] Pressed.\n");
        /* test_done = 1; */
        break;
    case SIGTERM:
        printf("SIGTERM：recv kill signal.\n");
        break;
    default:
        break;
    }
    exit(0);  //进程退出
 } 

int main(void) 
 { 
    signal(SIGINT, sigterm_handler);
    signal(SIGTERM, sigterm_handler); 
    while(!test_done)
        sleep(1);
    
    return 0; 
} 
