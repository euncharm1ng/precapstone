#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <execinfo.h>
#define RED "\033[31m"
#define NORM "\033[37m"

void get_trace (char* addr){
    void *array[10];
    char **strings;
    int size, i;

    size = backtrace (array, 10);
    strings = backtrace_symbols (array, size);
    if (strings != NULL)
    {
        // printf ("Obtained %d stack frames.\n", size);
        // printf("3333====%ld --  %s\n",strlen(strings[3]), strings[3]);
            // for (i = 0; i < size; i++)
            //   printf ("%s\n", strings[i]);
          
        for(int i =0; i< strlen(strings[3]); i++){
            if(strings[3][i]==')'){
                addr[i] = '\0';
                break;
            }
            addr[i] = strings[3][i];
        }
    }
    free (strings);
}

void callDdchck(int protocol, pthread_t* pid, pthread_mutex_t* m){
    int fd = open(".ddtrace", O_WRONLY | O_SYNC);
    char msg[30];
    char addr[30];
    get_trace(addr);
    // printf("-- %ld -- %d -- %s --\n",sizeof((int)strlen(addr)), (int)strlen(addr), addr);
    int addrLen = (int)strlen(addr) +1;
    memcpy(msg, (char*)&protocol, 4);
    memcpy(msg+4, (char*)&pid, 8);
    memcpy(msg+12, (char*)&m, 8);
    memcpy(msg+20, (char*)&addrLen, 4);
    memcpy(msg+24, (char*)&addr, addrLen);
    for(int i =0; i < 20;) i += write(fd, msg + i, 24+addrLen-i);
}

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
    printf(RED"LOCK --- %p, %p\n"NORM, pid, m);
    callDdchck(lock, pid, m);
    
    // int fd = open(".ddtrace", O_WRONLY | O_SYNC);
    
    // char msg[30];
    // char addr[20];
    // print_trace(addr);
    // printf("-- %ld -- %d -- %s --\n",sizeof((int)strlen(addr)), (int)strlen(addr), addr);
    // int addrLen = (int)strlen(addr) +1;
    // memcpy(msg, (char*)&lock, 4);
    // memcpy(msg+4, (char*)&pid, 8);
    // memcpy(msg+12, (char*)&m, 8);
    // memcpy(msg+20, (char*)&addrLen, 4);
    // memcpy(msg+24, (char*)&addr, addrLen);
    // for(int i =0; i < 20;) i += write(fd, msg + i, 24+addrLen-i);

    
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
    printf(RED"UNLK --- %p, %p\n"NORM, pid, m);

    callDdchck(unlock, pid, m);

    // int fd = open(".ddtrace", O_WRONLY | O_SYNC);

    // char msg[20];
    // char addr[20];
    // print_trace(addr);
    // printf("-- %ld -- %d -- %s --\n",sizeof((int)strlen(addr)), (int)strlen(addr), addr);
    // int addrLen = (int)strlen(addr) +1;
    // memcpy(msg, (char*)&unlock, 4);
    // memcpy(msg+4, (char*)&pid, 8);
    // memcpy(msg+12, (char*)&m, 8);
    // memcpy(msg+20, (char*)&addrLen, 4);
    // memcpy(msg+24, (char*)&addr, addrLen);
    // for(int i =0; i < 20;) i += write(fd, msg + i, 20-i);

    unlockp(m);
}