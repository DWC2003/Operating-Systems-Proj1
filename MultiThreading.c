#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Global variables/Shared resources
int netProfit = 0;
int storeStock = 250;
// Trucks is the same as the number of threads - 1
int trucks = 0;
// Mutex locks
pthread_mutex_t lock;
pthread_mutex_t lock2;

// Structure to be passed to thread function
struct Input{
    int tasks[100]; // List of deliveries/shipments. Max of 100.
    int id; // Designates what type of truck
};

// Thread function
void* runner(void* param){
    int i;
    struct Input *info = (struct Input*)param; // Structure for delivery/shipment information

    int delnum = 0; // Number of deliveries done
    static int shipnum = 0; // Number of shipments done. Can be accessed by all
    bool flag = true; // Flag for outputting failed shipment so it doesn't output constantly

    while (delnum < 10 || shipnum < 100){
        pthread_mutex_lock(&lock); // Locks code with first lock

        if (info->id < trucks){ // Delivery truck segment
            if (delnum >= 10){
                pthread_mutex_unlock(&lock);
                break;
            }
            if ((storeStock - info->tasks[delnum]) < 0){ // Not enough stock for delivery
                if (shipnum >= 35){
                    printf("\nNot enough stock for truck %d(%d deliveries remaining). \n", (info->id)+1, 10-(delnum));
                    pthread_mutex_unlock(&lock);
                    break;
                }
                if (flag){
                    printf("\n Delivery number %d (%d products) in truck %d could not leave due to lack of stock. Must wait for shipment. F\n", (delnum+1), info->tasks[delnum], (info->id + 1));
                    flag = !flag;
                }
                else{
                    pthread_mutex_unlock(&lock);
                    continue;
                }
            }
            else{ // Successful delivery
                printf("\n Delivery number %d has left in truck %d with %d products. S\n", (delnum+1), ((info->id) + 1), info->tasks[delnum]);
                storeStock -= info->tasks[delnum];
                printf("Current stock: %d\n", storeStock);

                // Second mutex lock to access the other shared resource
                pthread_mutex_lock(&lock2);
                netProfit += 2*info->tasks[delnum];
                printf("Current net profit: %d\n", netProfit);
                pthread_mutex_unlock(&lock2);

                delnum++;
                flag = true;

                
            }
        } // Shipment truck segment
        else{
            if (shipnum >= 35){
                pthread_mutex_unlock(&lock);
                break;
            }

            printf("\n Shipment number %d has arrived with %d products.\n", (shipnum+1), (info->tasks[shipnum]));
            storeStock += info->tasks[shipnum];
            printf("Current stock: %d\n", storeStock);

            // Second mutex lock to access the other shared resource
            pthread_mutex_lock(&lock2);
            netProfit -= info->tasks[shipnum];
            printf("Current net profit: %d\n", netProfit);
            pthread_mutex_unlock(&lock2);
            shipnum++;
        }

        // Releases lock and pauses for 1 millisecond
        pthread_mutex_unlock(&lock);
        sleep(1);

    }
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    int i;
    if (argv[1] == NULL){ // Checks for argument
        perror("No argument.");
        exit(EXIT_FAILURE);
    }
    trucks = atoi(argv[1]); // Gets number of trucks from argument 1
    if (trucks == 0){ // 0 Delivery trucks
        perror("Must be an integer or more than 0.");
        exit(EXIT_FAILURE);
    }

    // Prints number of delivery trucks and allocates memory for all of their deliveries
    printf("Delivery trucks: %d\n",trucks);
    struct Input *deliveries[trucks];
    for (i = 0; i < trucks; i++){
        deliveries[i] = (struct Input*) malloc(sizeof(struct Input));
        deliveries[i]->id = i;
        int j;
        for (j = 0; j < 10; j++){
            deliveries[i]->tasks[j] = (j+1)*10;
        }
    }
      
    // Allocates memory for shipments
    struct Input *shipment = (struct Input*) malloc(sizeof(struct Input));
    shipment->id = trucks;
    for (i = 0; i < 35; i++){
        shipment->tasks[i] = 100+i;
    }

    // Day begins
    printf("\nStart of day stock: %d\n", storeStock);

    // pthread initializations
    pthread_t tid[trucks];
    pthread_attr_t attr;
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock2, NULL);
    pthread_attr_init(&attr);

    // Creates all threads for deliveries and then shipment thread
    for (i = 0; i < trucks; i++){
        pthread_create(&tid[i], &attr, runner, deliveries[i]);

    }
    pthread_create(&tid[trucks], &attr, runner, shipment);
    
    // Joins threads
    for (i = 0; i < trucks+1; i++){
        pthread_join(tid[i], NULL);
    }

    // Destroys lock and outputs end of day stats
    pthread_mutex_destroy(&lock);

    printf("\nEnd of day stock: %d\nEnd of day net profit: %d\n", storeStock, netProfit);
}