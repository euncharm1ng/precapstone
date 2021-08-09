#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
//1 lock, 0 unlock

void main(){

    if (mkfifo(".ddtrace", 0666)) {
		if (errno != EEXIST) {
			perror("fail to open fifo: ") ;
			exit(1) ;
		}
	}
    int fd = open(".ddtrace", O_RDONLY | O_SYNC);

    while(1){
        pthread_mutex_t* m;
        pthread_t *pid;
        int len =0, prochk;
        printf("====\n");
        if((len = read(fd, &prochk, 4)) == -1) break;
        else if(len > 0)
            printf("=%d\n", prochk);

        if((len = read(fd, &pid, 8)) == -1) break;
        else if(len > 0)
            printf("-%p\n", pid);

        if((len = read(fd, &m, 8)) == -1) break;
        else if(len > 0)
            printf("-%p\n", m);
    }

    close(fd);
}