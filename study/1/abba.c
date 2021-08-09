#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
int cnt = 0;

void add1(){
    cnt++;
}

void *mythr1(){
    for(int i =0; i< 1000; i++){
        pthread_mutex_lock(&m2);
        pthread_mutex_lock(&m1);
        printf("mythr1 - %d\n", i);
        add1();
        pthread_mutex_unlock(&m1);
        pthread_mutex_unlock(&m2);
        sleep(0.01);
    }
}

void *mythr2(){
    for(int i =0; i< 1000; i++){
        pthread_mutex_lock(&m1);
        pthread_mutex_lock(&m2);
        printf("mythr2 - %d\n", i);
        add1();
        pthread_mutex_unlock(&m2);
        pthread_mutex_unlock(&m1);
        sleep(0.01);
    }
}

void main(){
    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, mythr1, NULL);
    pthread_create(&thread2, NULL, mythr2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    printf("end of threads: %d\n", cnt);
    exit(0);
}
