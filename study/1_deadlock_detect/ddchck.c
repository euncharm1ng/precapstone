#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <execinfo.h>

#define RED "\033[31m"
#define CYAN "\033[36m"
#define NORM "\033[37m"
#define PRINTCALL 0
#define PRINTDEBUG 1

char* fileName;
int cyclePredicted = 1;

typedef struct Edge{
    int traveledCnt, lockAddr, threadIDCnt;
    struct Thread** threadID; //pThread
    struct Node** traveled; //pNode*
    struct Node* dest; //pNode
}Edge;
typedef Edge* pEdge;

typedef struct Node{
    int pathsCnt, cycleChker, edgeCnt, cyclePredCnt;
    long *mutex;
    struct Node** paths;
    pEdge* edges;
    struct Thread** cyclePred;
}Node;
typedef Node* pNode;

typedef struct Thread{
    int holdsCnt;
    pthread_t* pid;
    pNode* holds;
} Thread;
typedef Thread* pThread;

typedef struct Graph{
    int nodesCnt, threadsCnt, startingEdgesCnt;
    pEdge* startingEdges;
    pNode* nodes;
    pThread* threads;
}Graph;
typedef Graph* pGraph;


pEdge createEdge(pThread t, int addr){
    pEdge e = (pEdge)malloc(sizeof(Edge));
    e->threadIDCnt = 1;
    e->traveledCnt =t->holdsCnt -1;
    e->threadID = (pThread*)malloc(sizeof(pThread) * e->threadIDCnt);
    e->threadID[0] = t;
    e->traveled = (pNode*)malloc(sizeof(pNode)*e->traveledCnt);
    for(int i =0; i< e->traveledCnt; i++){
        e->traveled[i] = t->holds[i];
    }
    e->dest = t->holds[t->holdsCnt-1];
    e->lockAddr = addr;
    return e;
}
void freeEdge(pEdge e){
    free(e->traveled);
    free(e);
}

pNode createNode(long* m){
    pNode n = (pNode)malloc(sizeof(Node));
    n->cyclePredCnt = 0;
    n->mutex = m;
    n->paths = NULL;
    n->pathsCnt = 0;
    n->cycleChker = 0;
    n->edgeCnt = 0;
    n->edges = NULL;
    n->cyclePred = NULL;
    return n;
}
void nAddPath(pNode src, pNode dest){
        #if PRINTDEBUG
        printf("\tnAddPath(): %p adding: %p\n", src, dest);
        #endif
    src->pathsCnt++;
    src->paths = (pNode*)realloc(src->paths, sizeof(pNode) * src->pathsCnt);
    src->paths[src->pathsCnt-1] = dest;
}
void nDelPath(pNode src, pNode dest){
        #if PRINTDEBUG
        printf("\tnDelPath(): %p deleting: %p\n", src, dest);
        #endif
    for(int i =0; i< src->pathsCnt; i++){
        if(src->paths[i] == dest){        
            for(int j = i+1; j<src->pathsCnt; j++){
                src->paths[j-1] = src->paths[j];
            }
            src->pathsCnt--;
            src->paths = (pNode*)realloc(src->paths, sizeof(pNode) * src->pathsCnt);
            break;
        }
    }
}
void nAddEdge(pNode n, pEdge toAdd){
    for(int i = 0; i< n->edgeCnt; i++){
        if(n->edges[i]->threadID == toAdd->threadID && n->edges[i]->dest == toAdd->dest 
                                        && n->edges[i]->traveledCnt == toAdd->traveledCnt){
            for(int j =0; j< toAdd->traveledCnt; j++){
                if(n->edges[i]->traveled[j] == toAdd->traveled[j]){ 
                    freeEdge(toAdd);
                    return;
                }
            }
        }
    }
    n->edgeCnt++;
    n->edges = (pEdge*)realloc(n->edges, sizeof(pEdge) * n->edgeCnt);
    n->edges[n->edgeCnt-1] = toAdd;
}

pThread createThread(pthread_t* pid){
    pThread t = (pThread)malloc(sizeof(Thread));
    t->pid = pid;
    t->holds = NULL;
    t->holdsCnt =0;
    return t;
}
void tAddNode(pThread t, pNode toAdd, pGraph g, int addr){
        #if PRINTDEBUG
        printf("\tcurrThread: %p nodetoadd: %p\n", t, toAdd);
        #endif
    t->holdsCnt++;
    t->holds = (pNode*)realloc(t->holds, sizeof(pNode)* t->holdsCnt);
    t->holds[t->holdsCnt - 1] = toAdd;
    
    if(t->holdsCnt>1){
        nAddPath(t->holds[t->holdsCnt-2], t->holds[t->holdsCnt-1]);
        pEdge newEdge = createEdge(t, addr);
        nAddEdge(t->holds[t->holdsCnt-2], newEdge);
    }
}
void tDelNode(pThread t, pNode toDel){
        #if PRINTDEBUG
        printf("\tcurrThread: %p, freeing: %p\n", t, toDel);
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
void gAddEdge(pGraph g, pThread t, int addr){
    if(t->holdsCnt == 1){
        g->startingEdgesCnt++;
        g->startingEdges = (pEdge*)realloc(g->startingEdges, sizeof(pEdge) * g->startingEdgesCnt);
        g->startingEdges[g->startingEdgesCnt-1] = createEdge(t, addr);
    }
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
pNode gChkNodeExist(pGraph g, long* m){
    for(int i =0; i < g->nodesCnt; i++){
        if(g->nodes[i]->mutex == m) return g->nodes[i];
    }
    return NULL;
}
pThread gChkThreadExist(pGraph g, pthread_t* pid){
    for(int i =0; i < g->threadsCnt; i++){
        if(g->threads[i]->pid == pid) return g->threads[i];
    }
    return NULL;
}
void gPrintEdge(pGraph g){
    printf("gPrintEdge: \n");
    printf("\tgraph:\n");
    for(int i =0; i< g->startingEdgesCnt; i++){
        printf("\t\ttid: ");
        for(int j =0; j < g->startingEdges[i]->threadIDCnt; j++){
            printf(" %p, ", g->startingEdges[i]->threadID[j]);
        }
        printf("\n\t\t\t dest: %p\n", g->startingEdges[i]->dest);
    }
    for(int i=0; i< g->nodesCnt; i++){
        printf("\tnode %p:\n", g->nodes[i]);
        for(int j =0; j < g->nodes[i]->edgeCnt; j++){
            printf("\t\ttid: ");
            for(int k =0; k < g->nodes[i]->edges[j]->threadIDCnt; k++){
                printf(" %p, ", g->nodes[i]->edges[j]->threadID[k]);
            }
            printf("\n\t\t\tdest: %p, traveled: ", g->nodes[i]->edges[j]->dest);
            for(int k =0; k<g->nodes[i]->edges[j]->traveledCnt; k++){
                printf("%p -> ", g->nodes[i]->edges[j]->traveled[k]);
            }
            printf("\n");
        }
    }
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
void gAddToGraph(pGraph g, long* m, long* pid, int addr){
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
    
    tAddNode(currThread, nodeToAdd, g, addr);
    gAddEdge(g, currThread, addr); //must called after tAddNode
}
void gDelFromGraph(pGraph g, long* m, pthread_t* pid){
    pThread currThread = gChkThreadExist(g, pid);
    pNode target = gChkNodeExist(g, m);
    
    tDelNode(currThread, target);
}
void freeGraph(pGraph g){
    for(int i =0; i< g->nodesCnt; i++){
        for(int j =0; j< g->nodes[i]->edgeCnt; j++){
            freeEdge(g->nodes[i]->edges[j]);
        }
        free(g->nodes[i]->edges);
        free(g->nodes[i]->paths);
        free(g->nodes[i]);
    }
    for(int i =0; i< g->threadsCnt; i++){
        free(g->threads[i]->holds);
        free(g->threads[i]);
    }
    for(int i =0; i< g->startingEdgesCnt; i++){
        freeEdge(g->startingEdges[i]);
    }
    free(g->startingEdges);
    free(g->nodes);
    free(g->threads);
    free(g);
}

void printAddr2line(int addr){
    char addr2line[64];
    char line[1024];

    sprintf(addr2line, "addr2line -e %s %x", fileName, addr);
    FILE *fp =NULL;
    if((fp = popen(addr2line, "r")) == NULL){
        printf("popen error\n");
    }else{
        fgets(line, 1024, fp);
        printf("\t%s", line);
    }
    pclose(fp);
}

int chkCycleRecur(pNode currNode){
    if(currNode->cycleChker == 1) return 1; //found cycle
    currNode->cycleChker = 1;
    for(int i =0; i< currNode->pathsCnt; i++){
        if(chkCycleRecur(currNode->paths[i])) return 1;
    }
    currNode->cycleChker = 0;
    return 0; //no cycle
}
int chkCycle(pGraph g, int addr){
    for(int i =0; i< g->nodesCnt; i++)
        g->nodes[i]->cycleChker = 0;

    for(int i =0; i< g->nodesCnt; i++){
        if(chkCycleRecur(g->nodes[i])){ 
            printf(RED"DEADLOCK DETECTED ON:\n"NORM);
            printAddr2line(addr);
            return 1;
        }
    }
    return 0;
}

int chkTraveledOverlap(pEdge prevEdge, pEdge currEdge){
    for(int i =0; i< prevEdge->traveledCnt; i++){
        for(int j =0; j< currEdge->traveledCnt; j++){
            if(prevEdge->traveled[i] == currEdge->traveled[j])
                return 0; //overlaps, no need to go further
        }
    }
    return 1;
}
int tidExistInEdge(pNode currNode, pEdge prevEdge){
    printf(RED"-32-2-2--332-tidExistInEdge\n"NORM);
    for(int i =0; i<prevEdge->threadIDCnt; i++){
        for(int j =0; j<currNode->cyclePredCnt; j++){
            if(currNode->cyclePred[j] == prevEdge->threadID[i]) return 1;
        }
    }
    return 0;
}
int tidCmp(pThread* currNode, pThread* prevEdge, int currNodeCnt, int prevEdgeCnt){
    for(int i =0; i<prevEdgeCnt; i++){
        for(int j =0; j<currNodeCnt; j++){
            if(currNode[j] == prevEdge[i]) return 1;
        }
    }
    return 0;
}
void predictRecur(pNode currNode, pEdge prevEdge){
    if(currNode->cyclePred == NULL);
    else if(tidCmp(currNode->cyclePred, prevEdge->threadID, currNode->cyclePredCnt, prevEdge->threadIDCnt))//tidExistInEdge(currNode, prevEdge))//currNode->cyclePred == prevEdge->threadID)
        return;
    else{
        if(cyclePredicted){ 
            printf(RED "DEADLOCK IS PREDICTED ON:\n" NORM);
            cyclePredicted = 0;
        }
        printAddr2line(prevEdge->lockAddr);
        return;
    }
    
    // currNode->cyclePred = prevEdge->threadID;
    for(int i =0; i< currNode->edgeCnt; i++){
        // if(currNode->edges[i]->threadID == prevEdge->threadID || chkTraveledOverlap(prevEdge, currNode->edges[i])){
        if(tidCmp(currNode->edges[i]->threadID, prevEdge->threadID, currNode->edges[i]->threadIDCnt, prevEdge->threadIDCnt) 
                                                    || chkTraveledOverlap(prevEdge, currNode->edges[i])){
            //currNode->cyclePred = currNode->edges[i]->threadID;
            currNode->cyclePredCnt = currNode->edges[i]->threadIDCnt;
            currNode->cyclePred = (pThread*)malloc(sizeof(pThread) * currNode->cyclePredCnt);
            for(int j =0; j<currNode->cyclePredCnt; j++){
                currNode->cyclePred[j] = currNode->edges[i]->threadID[j];
            }
            predictRecur(currNode->edges[i]->dest, currNode->edges[i]);
        }
    }
    currNode->cyclePred = NULL;
}
void predict(pGraph g){
    for(int i =0; i<g->startingEdgesCnt; i++){
        predictRecur(g->startingEdges[i]->dest, g->startingEdges[i]);
    }
    if(cyclePredicted)
        printf(CYAN"NO DEADLOCK IS PREDICTED\n"NORM);
}

void main(int argc, char* argv[]){
    if (mkfifo(".ddtrace", 0666)) {
		if (errno != EEXIST) {
			perror("fail to open fifo: ") ;
			exit(1) ;
		}
	}
    int fd = open(".ddtrace", O_RDONLY | O_SYNC);
    pGraph g = createGraph();
    int loopCnt =0, len =0, protocol=0, addr=0; //protocol:1 lock, 0 unlock, 2 create, 3 join
    //pthread_mutex_t* m=NULL;
    //pthread_t *pid = NULL;
    long *m = NULL, *pid = NULL;
    fileName = argv[1];

    while(1){
        if((len = read(fd, &protocol, 4)) == -1) break;
        if((len = read(fd, &pid, 8)) == -1) break;
        if((len = read(fd, &m, 8)) == -1) break;
        if((len = read(fd, &addr, 4)) == -1) break;
        if(len == 0){
                #if PRINTDEBUG
                gPrintEdge(g);
                #endif
            predict(g);
            freeGraph(g);
            close(fd);
            exit(0);
        }
        else if(len>0){
            if(protocol == 1){
                    #if PRINTDEBUG || PRINTCALL
                    printf(CYAN"LOCKED\n%d --- pid: %p m: %p \n"NORM, loopCnt, pid, m);
                    #endif
                gAddToGraph(g, m, pid, addr);
                    #if PRINTDEBUG 
                    gPrintNodes(g);
                    gPrintThread(g);
                    #endif
                if(chkCycle(g, addr)){
                    freeGraph(g);
                    close(fd);
                    exit(0);
                }
            }
            else if(protocol == 0){
                    #if PRINTDEBUG || PRINTCALL
                    printf(CYAN"UNLOCKED\n%d --- pid: %p m: %p \n"NORM, loopCnt, pid, m);
                    #endif
                gDelFromGraph(g, m, pid);
                    #if PRINTDEBUG 
                    gPrintNodes(g);
                    gPrintThread(g);
                    #endif
            }
            else if(protocol == 2){
                printf(CYAN"CREATED\n%d --- pid: %p m: %p \n"NORM, loopCnt, pid, m);
                pThread childT = NULL, parentT = NULL;
                if((childT = gChkThreadExist(g, m)) == NULL){
                    childT = createThread(m);
                    gAddThread(g, childT);
                }

                if((parentT = gChkThreadExist(g, pid)) != NULL){
                    // if(parentT->holdsCnt == 0) ;
                    // else{
                    //     for(int i =0; i< g->startingEdgesCnt; i++){
                    //         if(g->startingEdges[i]->dest == parentT->holds[0]){
                    //             pEdge currE = g->startingEdges[i];
                    //             currE->threadID = (pThread*)realloc(currE->threadID, sizeof(pThread) * currE->threadIDCnt++);
                    //             currE->threadID[currE->threadIDCnt-1] = childT;
                    //         }
                    //     }
                    //     for(int i = 0; i < parentT->holdsCnt - 1; i++){
                    //         parentT->holds[i]->edges;
                    //         for(int j = 0; j < parentT->holds[i]->edgeCnt; j++){
                    //             if(parentT->holds[i]->edges[j]->dest == parentT->holds[i+1]){
                    //                 pEdge currE = parentT->holds[i]->edges[j];
                    //                 currE->threadID = (pThread*)realloc(currE->threadID, sizeof(pThread) * currE->threadIDCnt++);
                    //                 currE->threadID[currE->threadIDCnt-1] = childT;
                    //                 break;
                    //             }
                    //         }
                    //     }
                    // }
                    for(int i =0; i< g->startingEdgesCnt; i++){
                        pEdge currE = g->startingEdges[i];
                        while(tidCmp(currE->threadID, &parentT, currE->threadIDCnt, 1)){
                            //TODO
                        }
                    }
                }
                    #if PRINTDEBUG 
                    gPrintNodes(g);
                    gPrintThread(g);
                    #endif
            }
            else{
                printf("PROTOCOL ERROR!\n");
                exit(1);
            }
                #if PRINTDEBUG 
                printf("\n==================================\n");
                #endif
                #if PRINTCALL || PRINTDEBUG
                printf("\n");
                #endif
            loopCnt++;
        }
    }
}
