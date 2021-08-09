#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

typedef struct Node{
    Node* pathsTo;

}Node;

void main(){
    if (mkfifo(".ddtrace", 0666)) {
		if (errno != EEXIST) {
			perror("fail to open fifo: ") ;
			exit(1) ;
		}
	}
    int fd = open(".ddtrace", O_RDONLY | O_SYNC);

    while(1){
        pthread_mutex_t* m=NULL;
        pthread_t *pid = NULL;
        int len =0, protocol=0; //1 lock, 0 unlock
        printf("====\n");
        if((len = read(fd, &protocol, 4)) == -1) break;
        else if(len > 0){
            printf("=%d, %d\n", len, protocol);
            len = 0;
        }

        if((len = read(fd, &pid, 8)) == -1) break;
        else if(len > 0){
            printf("-%d, %p\n", len, pid);
            len = 0;
        }

        if((len = read(fd, &m, 8)) == -1) break;
        else if(len > 0){
            printf("-%d, %p\n", len, m);
            len = 0;
        }
    }

    close(fd);
}