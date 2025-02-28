#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int netProfit = 0;
int storeStock = 250;
int trucks = 0;
pthread_mutex_t lock;
pthread_mutex_t lock2;

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

        if (info->id < trucks){
            if (delnum >= 10){
                pthread_mutex_unlock(&lock);
                break;
            }
            if ((storeStock - info->tasks[delnum]) < 0){
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
            else{
                printf("\n Delivery number %d has left in truck %d with %d products. S\n", (delnum+1), ((info->id) + 1), info->tasks[delnum]);
                storeStock -= info->tasks[delnum];
                printf("Current stock: %d\n", storeStock);

                pthread_mutex_lock(&lock2);
                netProfit += 2*info->tasks[delnum];
                printf("Current net profit: %d\n", netProfit);
                pthread_mutex_unlock(&lock2);

                delnum++;
                flag = true;

                
            }
        } 
        else{
            if (shipnum >= 35){
                pthread_mutex_unlock(&lock);
                break;
            }
            printf("\n Shipment number %d has arrived with %d products.\n", (shipnum+1), (info->tasks[shipnum]));
            storeStock += info->tasks[shipnum];
            printf("Current stock: %d\n", storeStock);

            pthread_mutex_lock(&lock2);
            netProfit -= info->tasks[shipnum];
            printf("Current net profit: %d\n", netProfit);
            pthread_mutex_unlock(&lock2);
            shipnum++;
        }
        pthread_mutex_unlock(&lock);
        sleep(1);

    }
    pthread_exit(0);
}

int main(int argc, char *argv[]){
    int i;
    trucks = atoi(argv[1]);
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
      
    struct Input *shipment = (struct Input*) malloc(sizeof(struct Input));
    shipment->id = trucks;
    for (i = 0; i < 35; i++){
        shipment->tasks[i] = 100+i;
    }

    printf("\nStart of day stock: %d\n", storeStock);

    pthread_t tid[trucks];
    pthread_attr_t attr;
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock2, NULL);
    pthread_attr_init(&attr);

    for (i = 0; i < trucks; i++){
        pthread_create(&tid[i], &attr, runner, deliveries[i]);

    }
    pthread_create(&tid[trucks], &attr, runner, shipment);
    

    for (i = 0; i < trucks+1; i++){
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("\nEnd of day stock: %d\nEnd of day net profit: %d\n", storeStock, netProfit);
}