#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(){
    pid_t pid;

    pid = fork();
    int p[2], i;

    if (pipe(p) < 0){
        perror("pipe");
        exit(1);
    }

    if (pid < 0){
        printf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0){
        execlp("./MultiThreading", "MultiThreading", NULL);
    }
    else{
        wait(NULL);
        printf("Child Complete\n");
    }

    return 0;
}