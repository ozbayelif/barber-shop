#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

pthread_mutex_t shop;
sem_t customers, barbers;
int waiting;

void *barber(void *arg) {
    int id = *((int *)arg);

    while(1) {
        if(sem_trywait(&customers) != 0) {
            printf("Barber %d sleeps.\n", id);
            fflush(stdout);
            sleep(1);
        } else {
            pthread_mutex_lock(&shop);
            waiting--;
            sem_post(&barbers);
            pthread_mutex_unlock(&shop);
            sleep(5);
            printf("Barber %d cuts hair.\n", id);
            fflush(stdout);
        }
    }
}

void *customer(void *arg) {
    int id = *((int *)arg);

    pthread_mutex_lock(&shop);
    printf("Customer %d enters the barber shop.\n", id);
    fflush(stdout);
    if(waiting < N) {
        waiting++;
        sem_post(&customers);
        pthread_mutex_unlock(&shop);
        sem_wait(&barbers);
        sleep(1);
    } else {
        pthread_mutex_unlock(&shop);
        printf("Customer %d leaves the barber shop.\n", id);
        fflush(stdout);
    }

}

int main() {
    pthread_t barber_t;
    pthread_t customer_t[N];
    int b_id = 0, c_id[N];
    sem_init(&customers, 0, 0);
    sem_init(&barbers, 0, 1);
    waiting = 0;

    pthread_create(&barber_t, NULL, &barber, &b_id);
    for(int i = 0; i < 100; i++) {
        sleep(random()%2);
        c_id[i] = i;
        pthread_create(&customer_t[i], NULL, &customer, &c_id[i]);
    }

    pthread_join(barber_t, NULL);
    for(int i = 0; i < 100; i++) {
        pthread_join(customer_t[i], NULL);
    }
}