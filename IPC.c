#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096 // Constant buffer size

int main(int argc, char *argv[]){
    pid_t pid; // Process ID
    char buffer[BUFFER_SIZE]; // Buffer for pipe
    int readb; // Bytes read
    int p[2]; // Pipe array

    // Variables for final report
    int failedDeliveries = 0;
    int successfulDeliveries = 0;


    if (argv[1] == NULL){ // Checks for argument
        printf("Must provide argument.\n");
        exit(EXIT_FAILURE);
    }
    if (atoi(argv[1]) == 0){ // 0 Delivery trucks
        perror("Must be an integer and more than 0.\n");
        exit(EXIT_FAILURE);
    }

    if (pipe(p) < 0){ // Creates pipe
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork(); // Forks process

    if (pid < 0){ // Failed to fork process
        perror("Fork Failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0){ // Child process
        close(p[0]); // Close read end
        dup2(p[1], STDOUT_FILENO); // Redirect output into pipe
        close(p[1]);

        // Replace child process with multi-threading application
        execlp("./MultiThreading", "MultiThreading", argv[1], (char *)NULL);
    }
    else{ // Parent process
        close(p[1]); // Close write end
        printf("Beginning child process, this may take a minute.\n");
        printf("%d trucks with 10 deliveries each.\nPlease wait...\n", atoi(argv[1]));

        while ((readb = read(p[0], buffer, BUFFER_SIZE - 1)) > 0){ // Reads string from buffer continuously
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

        // Generates report
        printf("Failed Deliveries: %d\n", failedDeliveries);
        printf("Successful Deliveries: %d/%d\n", successfulDeliveries, atoi(argv[1])*10);
        if (failedDeliveries > successfulDeliveries){
            printf("More failed deliveries than successful. More shipments might be needed for smoother business.\n");
        }
        if (successfulDeliveries == atoi(argv[1])*10){
            printf("All deliveries for the day successfully delivered.\n");
        }
        else{
            printf("Not all deliveries completed.\n");
        }


        close(p[0]);

        // Waits on child process
        wait(NULL);
        printf("Child process complete.\n");
    }

    return 0;
}