#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int storeStock = 50;
pthread_mutex_t lock;
struct Input{
        int tasks[100];
        int id;
};

void* runner(void* param){
    int i;
    struct Input *info = (struct Input*)param;

    int delnum = 0;
    static int shipnum = 0;
    bool flag = true;

    while (delnum < 10 || shipnum < 100){
        pthread_mutex_lock(&lock);

        if (info->id < 20){
            if (delnum >= 10){
                pthread_mutex_unlock(&lock);
                continue;
            }
            if ((storeStock - info->tasks[delnum]) < 0){
                if (shipnum >= 35){
                        printf("\nNot enough stock for truck %d(%d deliveries remaining). \n", (info->id+1), 10-(delnum+1));
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
            else{
                printf("\n Delivery number %d has left in truck %d with %d products. S\n", (delnum+1), (info->id + 1), info->tasks[delnum]);
                storeStock -= info->tasks[delnum];
                printf("Current stock: %d\n", storeStock);
                delnum++;
                flag = true;
            }
        } 
        else{
            if (shipnum >= 35){
                pthread_mutex_unlock(&lock);
                break;
            }
            printf("\n Shipment number %d has arrived.\n", (shipnum+1));
            storeStock += info->tasks[shipnum];
            printf("Current stock: %d\n", storeStock);
            shipnum++;
        }
        pthread_mutex_unlock(&lock);
        sleep(1);

    }
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    int i;
    struct Input *deliveries[20];
    for (i = 0; i < 20; i++){
        deliveries[i] = (struct Input*) malloc(sizeof(struct Input));
        deliveries[i]->id = i;
        int j;
        for (j = 0; j < 10; j++){
            deliveries[i]->tasks[j] = (j+1)*10;
        }
    }
      
    struct Input *shipment = (struct Input*) malloc(sizeof(struct Input));
    shipment->id = 20;
    for (i = 0; i < 35; i++){
        shipment->tasks[i] = 100+i;
    }

    printf("\nStart of day stock: %d\n", storeStock);

    pthread_t tid[21];
    pthread_attr_t attr;
    pthread_mutex_init(&lock, NULL);
    pthread_attr_init(&attr);

    for (i = 0; i < 20; i++){
        pthread_create(&tid[i], &attr, runner, deliveries[i]);

    }
    pthread_create(&tid[20], &attr, runner, shipment);
    

    for (i = 0; i < 21; i++){
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("\nEnd of day stock = %d\n", storeStock);
}