#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void* pthread_mutex_lock(pthread_mutex_t * m){
    int lock = 1;
    pthread_t* pid;
    void* (*lockp)(pthread_mutex_t * m);
    void* (*selfp)();
    lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
    selfp = dlsym(RTLD_NEXT, "pthread_self");

    if(dlerror() != NULL)
        exit(1);
    pid = selfp();
    // printf("LOCK --- %p, %p\n", pid, m);
    
    int fd = open(".ddtrace", O_WRONLY | O_SYNC);
    
    char msg[20];
    memcpy(msg, (char*)&lock, 4);
    memcpy(msg+4, (char*)&pid, 8);
    memcpy(msg+12, (char*)&m, 8);
    for(int i =0; i < 20;) i += write(fd, msg + i, 20-i);

    lockp(m);
}

void* pthread_mutex_unlock(pthread_mutex_t * m){
    int unlock = 0;
    pthread_t* pid;
    void* (*unlockp)(pthread_mutex_t * m);
    void* (*selfp)();
    unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
    selfp = dlsym(RTLD_NEXT, "pthread_self");

    if(dlerror() != NULL)
        exit(1);
    pid = selfp();
    // printf("UNLOCK --- %p, %p\n", pid, m);

    int fd = open(".ddtrace", O_WRONLY | O_SYNC);

    char msg[20];
    memcpy(msg, (char*)&unlock, 4);
    memcpy(msg+4, (char*)&pid, 8);
    memcpy(msg+12, (char*)&m, 8);
    for(int i =0; i < 20;) i += write(fd, msg + i, 20-i);

    unlockp(m);
}