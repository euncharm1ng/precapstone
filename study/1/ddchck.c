#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

typedef struct Node{
    pthread_mutex_t *mutex;
    struct Node** paths;
    int pathsCnt, chker;
}Node;
typedef Node* pNode;

typedef struct Thread{
    pthread_t* pid;
    pNode* holds;
    int holdsCnt;
} Thread;
typedef Thread* pThread;

typedef struct Graph{
    pNode* nodes;
    int nodesCnt, threadsCnt;
    pThread* threads;
}Graph;
typedef Graph* pGraph;

pGraph createGraph(){
    pGraph g = (pGraph)malloc(sizeof(Graph));
    g->nodesCnt = 0;
    g->nodes = NULL;
    g->threadsCnt = 0;
    g->threads = NULL;
    return g;
}
void gAddNode(pGraph g, pNode newNode){
    g->nodesCnt++;
    g->nodes = (pNode*)realloc(g->nodes, sizeof(pNode) * g->nodesCnt);
    g->nodes[g->nodesCnt - 1] = newNode;
}
void gAddThread(pGraph g, pThread thread){
    g->threadsCnt++;
    g->threads = (pThread*)realloc(g->threads, sizeof(pThread) * g->threadsCnt);
    g->threads[g->threadsCnt - 1] = thread;
}
pNode gChkNodeExist(pGraph g, pthread_mutex_t* m){
    // printf("\tDEBUG: gChkNodeExist():\n");
    for(int i =0; i < g->nodesCnt; i++){
        if(g->nodes[i]->mutex == m) return g->nodes[i];
    }
    return NULL;
}
pThread gChkThreadExist(pGraph g, pthread_t* pid){
    // printf("\tDEBUG: gChkThreadExist():\n");
    for(int i =0; i < g->threadsCnt; i++){
        if(g->threads[i]->pid == pid) return g->threads[i];
    }
    return NULL;
}
void gPrintNodes(pGraph g){
    printf("printNodes():\n");
    for(int i =0; i< g->nodesCnt; i++){
        printf("\tnode: %p:\n\t\t", g->nodes[i]);
        for(int j =0; j < g->nodes[i]->pathsCnt; j++){
            printf("->%p, ",g->nodes[i]->paths[j]);
        }
        printf("\n");
    }
}

void gPrintThread(pGraph g){
    printf("printThread():\n");
    for(int i =0; i< g->threadsCnt; i++){
        printf("\tthread: %p:\n\t\t", g->threads[i]);
        for(int j =0; j < g->threads[i]->holdsCnt; j++){
            printf("->%p, ", g->threads[i]->holds[j]);
        }
        printf("\n");
    }
}


int chkCycleRecur(pNode currNode){
    if(currNode->chker == 1) return 1; //found cycle
    currNode->chker = 1;
    for(int i =0; i< currNode->pathsCnt; i++){
        if(chkCycleRecur(currNode->paths[i])) return 1;
    }
    currNode->chker = 0;
    return 0; //no cycle
}
int chkCycle(pGraph g){
    for(int i =0; i< g->nodesCnt; i++)
        g->nodes[i]->chker = 0;

    for(int i =0; i< g->nodesCnt; i++){
        if(chkCycleRecur(g->nodes[i])) return 1;
    }
    return 0;
}

pNode createNode(pthread_mutex_t* m){
    pNode n = (pNode)malloc(sizeof(Node));
    n->mutex = m;
    n->paths = NULL;
    n->pathsCnt = 0;
    n->chker = 0;
    return n;
}
void nAddPath(pNode src, pNode dest){
    src->pathsCnt++;
    printf("\tnAddPath(): %p adding: %p\n", src, dest);
    src->paths = (pNode*)realloc(src->paths, sizeof(pNode) * src->pathsCnt);
    src->paths[src->pathsCnt-1] = dest;
}
void nDelPath(pNode src, pNode dest){
    for(int i =0; i< src->pathsCnt; i++){
        if(src->paths[i] == dest){
            printf("\tnDelPath(): %p deleting: %p\n", src, src->paths[i]);
            for(int j = i+1; j<src->pathsCnt; j++){
                src->paths[j-1] = src->paths[j];
            }
            src->pathsCnt--;
            src->paths = (pNode*)realloc(src->paths, sizeof(pNode) * src->pathsCnt);
            break;
        }
    }
}

pThread createThread(pthread_t* pid){
    pThread t = (pThread)malloc(sizeof(Thread));
    t->pid = pid;
    t->holds = NULL;
    t->holdsCnt =0;
    return t;
}
void tAddNode(pThread t, pNode toAdd){
    t->holdsCnt++;
    t->holds = (pNode*)realloc(t->holds, sizeof(pNode)* t->holdsCnt);
    t->holds[t->holdsCnt - 1] = toAdd;
    
    if(t->holdsCnt>1)
        nAddPath(t->holds[t->holdsCnt-2], t->holds[t->holdsCnt -1]);

}
void tDelNode(pThread t, pNode toDel){
    for(int i =0; i< t->holdsCnt; i++){
        if(t->holds[i] == toDel){
            if(i-1 >-1){
                nDelPath(t->holds[i-1], t->holds[i]);
                if(i+1 < t->holdsCnt){
                    nDelPath(t->holds[i], t->holds[i+1]);
                    nAddPath(t->holds[i-1], t->holds[i+1]);
                }
            }
            else if(i+1 < t->holdsCnt){
                nDelPath(t->holds[i], t->holds[i+1]);
            } 
            for(int j = i+1; j < t->holdsCnt; j++){
                t->holds[j-1] = t->holds[j];
            }
            t->holdsCnt--;
            t->holds = (pNode*)realloc(t->holds, sizeof(pNode) * t->holdsCnt);
            break;
        }
    }
}



void main(){
    if (mkfifo(".ddtrace", 0666)) {
		if (errno != EEXIST) {
			perror("fail to open fifo: ") ;
			exit(1) ;
		}
	}
    int fd = open(".ddtrace", O_RDONLY | O_SYNC);
    pGraph g = createGraph();
int i =0;
    while(1){
        pthread_mutex_t* m=NULL;
        pthread_t *pid = NULL;
        int len =0, protocol=0; //1 lock, 0 unlock
        if((len = read(fd, &protocol, 4)) == -1) break;
        if((len = read(fd, &pid, 8)) == -1) break;
        if((len = read(fd, &m, 8)) == -1) break;
        if(len>0){
            if(protocol == 1){
                printf("LOCKED\n");
                pNode nodeToAdd = NULL;
                pThread currThread = NULL;
                printf("%d --- pid: %p m: %p \n", i, pid, m);
                if ((nodeToAdd = gChkNodeExist(g, m)) == NULL){
                    nodeToAdd = createNode(m);
                    gAddNode(g, nodeToAdd);
                }
                if((currThread = gChkThreadExist(g, pid)) == NULL){
                    currThread = createThread(pid);
                    gAddThread(g, currThread);
                }
                printf("\tcurrThread: %p nodetoadd: %p\n", currThread, nodeToAdd);
                tAddNode(currThread, nodeToAdd);
                
                gPrintNodes(g);
                gPrintThread(g);
                if(chkCycle(g)) printf("CYLCE!\n");
                printf("\n");
            }
            else if(protocol == 0){
                printf("UNLOCKED\n");
                printf("%d --- pid: %p m: %p \n", i, pid, m);
                pNode target = NULL;
                pThread currThread = NULL;
                currThread = gChkThreadExist(g, pid);
                target = gChkNodeExist(g, m);
                printf("\tcurrThread: %p, freeing: %p\n", currThread, target);
                tDelNode(currThread, target);
                
                gPrintNodes(g);
                gPrintThread(g);
                if(chkCycle(g)) printf("CYCLE!\n");
                printf("\n");
            }
            else{
                printf("PIPE ERROR!\n");
                exit(1);
            }
            printf("==================================\n\n");
            i++;
        }
    }

    close(fd);
}