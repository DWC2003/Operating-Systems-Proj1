#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]){
    pid_t pid;
    char buffer[BUFFER_SIZE];
    int readb;
    int p[2];
    int failedDeliveries = 0;
    int successfulDeliveries = 0;

    if (pipe(p) < 0){
        perror("pipe");
        exit(1);
    }
    pid = fork();

    if (pid < 0){
        printf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0){
        close(p[0]);
        dup2(p[1], STDOUT_FILENO);
        close(p[1]);

        execlp("./MultiThreading", "MultiThreading", (char *)NULL);
    }
    else{
        close(p[1]);

        while ((readb = read(p[0], buffer, BUFFER_SIZE - 1)) > 0){
            buffer[readb] = '\0';

            for (int i = 0; i < readb; i++){
                if (buffer[i] == '\n'){
                    if (buffer[i-1] == 'F'){
                        failedDeliveries++;
                    }
                    else if (buffer[i-1] == 'S'){
                        successfulDeliveries++;
                    }
                }
            }
        }
        printf("Failed Deliveries: %d\n", failedDeliveries);
        printf("Successful Deliveries: %d\n", successfulDeliveries);


        close(p[0]);

        wait(NULL);
        printf("Child process complete.");
    }

    return 0;
}