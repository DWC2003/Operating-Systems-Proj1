#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int storeStock = 50;
pthread_mutex_t lock;
struct Input{
        int tasks[10];
        int id;
};

void* runner(void* param){
    int i;
    struct Input *info = (struct Input*)param;

    static int delnum = 0;
    static int shipnum = 0;
    bool flag = true;

    while (delnum <= 9 || shipnum <= 9){
        if ((int) info->id == 0){
            if (delnum == 10){
                continue;
            }
            if ((storeStock - info->tasks[delnum]) < 0){
                if (flag){
                    printf("\n Delivery number %d (%d products) could not leave due to lack of stock. Must wait for shipment.\n", (delnum+1), info->tasks[delnum]);
                    flag = !flag;
                }
                else{
                    continue;
                }
            }
            else{
                pthread_mutex_lock(&lock);
                printf("\n Delivery number %d has left.\n", (delnum+1));
                storeStock -= info->tasks[delnum];
                printf("Current stock: %d\n", storeStock);
                delnum++;
                flag = true;
                pthread_mutex_unlock(&lock);
            }
        } 
        else {
            if (shipnum == 10){
                continue;
            }
            pthread_mutex_lock(&lock);
            printf("\n Shipment number %d has arrived.\n", (shipnum+1));
            storeStock += info->tasks[shipnum];
            printf("Current stock: %d\n", storeStock);
            shipnum++;
            pthread_mutex_unlock(&lock);

        }
        sleep(1);

    }
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    struct Input *delivery = (struct Input*)
        malloc(sizeof(struct Input));
    struct Input *shipment = (struct Input*)
        malloc(sizeof(struct Input));
    delivery->id = 0;
    shipment->id = 1;
    int i;

    for (i = 0; i < 10; i++){
        delivery->tasks[i] = i*10;
        shipment->tasks[i] = i*10;
    }

    printf("\nStart of day stock: %d\n", storeStock);

    pthread_t tid[2];
    pthread_attr_t attr;
    pthread_mutex_init(&lock, NULL);
    pthread_attr_init(&attr);

    
    pthread_create(&tid[0], &attr, runner, delivery);
    pthread_create(&tid[1], &attr, runner, shipment);


    for (i = 0; i < 2; i++){
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("\nEnd of day Stock = %d\n", storeStock);
}