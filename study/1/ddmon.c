#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <execinfo.h>

#define RED "\033[31m"
#define CYAN "\033[36m"
#define NORM "\033[37m"
#define DEBUG 0
#define CALLDDCHCK 1 //0 for doDdchck

//{ for doDdchck
    static int ddchckCnt=0;

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
    static pGraph g;

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
        #if DEBUG
        printf(CYAN"\tnAddPath(): %p adding: %p\n"NORM, src, dest);
        #endif
        src->paths = (pNode*)realloc(src->paths, sizeof(pNode) * src->pathsCnt);
        src->paths[src->pathsCnt-1] = dest;
    }
    void nDelPath(pNode src, pNode dest){
        for(int i =0; i< src->pathsCnt; i++){
            if(src->paths[i] == dest){
                #if DEBUG
                printf(CYAN"\tnDelPath(): %p deleting: %p\n"NORM, src, src->paths[i]);
                #endif
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
        #if DEBUG
        printf(CYAN"\tcurrThread: %p nodetoadd: %p\n"NORM, t, toAdd);
        #endif
        t->holdsCnt++;
        t->holds = (pNode*)realloc(t->holds, sizeof(pNode)* t->holdsCnt);
        t->holds[t->holdsCnt - 1] = toAdd;
        
        if(t->holdsCnt>1)
            nAddPath(t->holds[t->holdsCnt-2], t->holds[t->holdsCnt -1]);

    }
    void tDelNode(pThread t, pNode toDel){
        #if DEBUG
        printf(CYAN"\tcurrThread: %p, freeing: %p\n"NORM, t, toDel);
        #endif
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

    pGraph createGraph(){
        pGraph g = (pGraph)malloc(sizeof(Graph));
        g->nodesCnt = 0;
        g->nodes = NULL;
        g->threadsCnt = 0;
        g->threads = NULL;
        return g;
    }
    void gAddNode(pGraph g, pNode toAdd){
        g->nodesCnt++;
        g->nodes = (pNode*)realloc(g->nodes, sizeof(pNode) * g->nodesCnt);
        g->nodes[g->nodesCnt - 1] = toAdd;
    }
    void gAddThread(pGraph g, pThread toAdd){
        g->threadsCnt++;
        g->threads = (pThread*)realloc(g->threads, sizeof(pThread) * g->threadsCnt);
        g->threads[g->threadsCnt - 1] = toAdd;
    }
    pNode gChkNodeExist(pGraph g, pthread_mutex_t* m){
        for(int i =0; i < g->nodesCnt; i++){
            if(g->nodes[i]->mutex == m) 
                return g->nodes[i];
        }
        return NULL;
    }
    pThread gChkThreadExist(pGraph g, pthread_t* pid){
        for(int i =0; i < g->threadsCnt; i++){
            if(g->threads[i]->pid == pid) 
                return g->threads[i];
        }
        return NULL;
    }
    void gPrintNodes(pGraph g){
        printf(CYAN"printNodes():\n"NORM);
        for(int i =0; i< g->nodesCnt; i++){
            printf(CYAN"\tnode: %p:\n\t\t"NORM, g->nodes[i]);
            for(int j =0; j < g->nodes[i]->pathsCnt; j++){
                printf(CYAN"->%p, "NORM,g->nodes[i]->paths[j]);
            }
            printf(CYAN"\n"NORM);
        }
    }
    void gPrintThread(pGraph g){
        printf(CYAN"printThread():\n"NORM);
        for(int i =0; i< g->threadsCnt; i++){
            printf(CYAN"\tthread: %p:\n\t\t"NORM, g->threads[i]);
            for(int j =0; j < g->threads[i]->holdsCnt; j++){
                printf(CYAN"->%p, "NORM, g->threads[i]->holds[j]);
            }
            printf(CYAN"\n"NORM);
        }
    }
    void freeGraph(pGraph g){
        for(int i =0; i< g->nodesCnt; i++){
            free(g->nodes[i]->paths);
            free(g->nodes[i]);
        }
        for(int i =0; i< g->threadsCnt; i++){
            free(g->threads[i]->holds);
            free(g->threads[i]);
        }
        free(g->nodes);
        free(g->threads);
        free(g);
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
    int chkCycle(pGraph g, char* fileName, int addr){
        for(int i =0; i< g->nodesCnt; i++)
            g->nodes[i]->chker = 0;

        for(int i =0; i< g->nodesCnt; i++){
            if(chkCycleRecur(g->nodes[i])){ 
                char addr2line[50];
                char line[1024];
                printf(RED"DEADLOCK DETECTED ON: \n"NORM);

                sprintf(addr2line, "addr2line -e %s %x", fileName, addr);
                FILE *fp =NULL;
                if((fp = popen(addr2line, "r")) == NULL){
                    printf("popen error\n");
                }else{
                    fgets(line, 1024, fp);
                    printf("\t%s\n", line);
                }
                pclose(fp);
                return 1;
            }
        }
        return 0;
    }
//}
int get_trace(char* fileName){
    void *array[10];
    char addr[30];
    char **strings;
    int size, i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);
    if (strings != NULL){
        if(fileName != NULL){
            int i =0; 
            while(strings[3][i] != '(')
                fileName[i++] = strings[3][i];
            fileName[i] = '\0';
        }

        for(int i = 0; i< strlen(strings[3]); i++){
            if(strings[3][i]=='('){
                int j =0;
                i++;
                while(strings[3][++i] != ')')
                    addr[j++] = strings[3][i];
                addr[j] = '\0';
            }
        }
    }
    free (strings);
    return (int)strtol(addr, NULL, 16) - 5;
}

void doDdchck(int protocol, pthread_t* pid, pthread_mutex_t* m){
    if(ddchckCnt == 0){
        g = createGraph();
        ddchckCnt = 1;
    }
    char fileName[32];
    int addr = get_trace(fileName);

    if(protocol == 1){
            #if DEBUG
            printf(CYAN"LOCKED\n --- pid: %p m: %p \n"NORM, pid, m);
            #endif
        pNode nodeToAdd = NULL;
        pThread currThread = NULL;
        
        if ((nodeToAdd = gChkNodeExist(g, m)) == NULL){
            nodeToAdd = createNode(m);
            gAddNode(g, nodeToAdd);
        }
        if((currThread = gChkThreadExist(g, pid)) == NULL){
            currThread = createThread(pid);
            gAddThread(g, currThread);
        }
        
        tAddNode(currThread, nodeToAdd);
            #if DEBUG
            gPrintNodes(g);
            gPrintThread(g);
            printf("\n");
            #endif
        if(chkCycle(g, fileName, addr)){
            freeGraph(g);
            exit(0);
        }
    }
    else if(protocol == 0){
            #if DEBUG
            printf(CYAN"UNLOCKED\n --- pid: %p m: %p \n"NORM, pid, m);
            #endif
        pThread currThread = gChkThreadExist(g, pid);
        pNode target = gChkNodeExist(g, m);
        
        tDelNode(currThread, target);
            #if DEBUG
            gPrintNodes(g);
            gPrintThread(g);
            printf("\n");
            #endif
    }
}

void callDdchck(int protocol, pthread_t* pid, pthread_mutex_t* m){
    int fd = open(".ddtrace", O_WRONLY | O_SYNC);
    char msg[24];
    int addr = get_trace(NULL);
    memcpy(msg, (char*)&protocol, 4);
    memcpy(msg+4, (char*)&pid, 8);
    memcpy(msg+12, (char*)&m, 8);
    memcpy(msg+20, (char*)&addr, 4);
    for(int i =0; i < 20;) 
        i += write(fd, msg + i, 24-i);
}

void* pthread_mutex_lock(pthread_mutex_t * m){
    int lock = 1;
    pthread_t* pid;
    void* (*lockp)(pthread_mutex_t * m);
    void* (*selfp)();
    lockp = dlsym(RTLD_NEXT, "pthread_mutex_lock");
    selfp = dlsym(RTLD_NEXT, "pthread_self");
    if(dlerror() != NULL) exit(1);

    pid = selfp();
        #if DEBUG
        printf(RED"LOCK --- %p, %p\n"NORM, pid, m);
        #endif
    #if CALLDDCHCK
    callDdchck(lock, pid, m); 
    #else
    doDdchck(lock, pid, m);
    #endif
    lockp(m);
}

void* pthread_mutex_unlock(pthread_mutex_t * m){
    int unlock = 0;
    pthread_t* pid;
    void* (*unlockp)(pthread_mutex_t * m);
    void* (*selfp)();
    unlockp = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
    selfp = dlsym(RTLD_NEXT, "pthread_self");
    if(dlerror() != NULL) exit(1);

    pid = selfp();
        #if DEBUG
        printf(RED"UNLK --- %p, %p\n"NORM, pid, m);
        #endif
    #if CALLDDCHCK
    callDdchck(unlock, pid, m);
    #else
    doDdchck(unlock, pid, m);
    #endif
    unlockp(m);
}