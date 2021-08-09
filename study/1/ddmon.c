#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>

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
    printf("LOCK --- %p, %p\n", m, pid);
    
    int fd = open(".ddtrace", O_WRONLY | O_SYNC);
    for(int i =0; i < sizeof(lock);)
        i += write(fd, ((char*)&lock) + i, sizeof(lock)-i);

    for(int i =0; i < sizeof(pid);)
        i += write(fd, ((char*)&pid) + i, sizeof(pid) -i);

    for(int i =0; i < sizeof(m);)
        i += write(fd, ((char*)&m) + i, sizeof(m) -i);


    lockp(m);
}

void* pthread_mutex_unlock(pthread_mutex_t * m){
    int unlock = 1;
    pthread_t* pid;
    void* (*unlockp)(pthread_mutex_t * m);
    void* (*selfp)();
    unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
    selfp = dlsym(RTLD_NEXT, "pthread_self");

    if(dlerror() != NULL)
        exit(1);
    printf("UNLOCK --- %p\n", m);
    pid = selfp();
    printf("LOCK --- %p, %p\n", m, pid);
    
    int fd = open(".ddtrace", O_WRONLY | O_SYNC);
    for(int i =0; i < sizeof(unlock);)
        i += write(fd, ((char*)&unlock) + i, sizeof(unlock)-i);

    for(int i =0; i < sizeof(pid);)
        i += write(fd, ((char*)&pid) + i, sizeof(pid) -i);
        
    for(int i =0; i < sizeof(m);)
        i += write(fd, ((char*)&m) + i, sizeof(m) -i);


    unlockp(m);
}