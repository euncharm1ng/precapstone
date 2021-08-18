/* On Mac OS (aka OS X) the ucontext.h functions are deprecated and requires the
   following define.
*/
#define _XOPEN_SOURCE 700

/* On Mac OS when compiling with gcc (clang) the -Wno-deprecated-declarations
   flag must also be used to suppress compiler warnings.
*/

#include <signal.h>   /* SIGSTKSZ (default stack size), MINDIGSTKSZ (minimal
                         stack size) */
#include <stdio.h>    /* puts(), printf(), fprintf(), perror(), setvbuf(), _IOLBF,
                         stdout, stderr */
#include <stdlib.h>   /* exit(), EXIT_SUCCESS, EXIT_FAILURE, malloc(), free() */
#include <ucontext.h> /* ucontext_t, getcontext(), makecontext(),
                         setcontext(), swapcontext() */
#include <stdbool.h>  /* true, false */
#include <sys/time.h> // ITIMER_REAL, ITIMER_VIRTUAL, ITIMER_PROF, struct itimerval, setitimer()
#include <string.h>
#include "sthreads.h"

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ*100
#define TIMEOUT    3          // ms

/*******************************************************************************
                             Global data structures

                Add data structures to manage the threads here.
********************************************************************************/

thread_t* head, *tail;
tid_t tidProducer = 1000;

/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/
int createCTX(ucontext_t* ctx, ucontext_t* next){
  void *stack = malloc(STACK_SIZE);
  if (stack == NULL) {
    perror("Allocating stack");
    return -1;
    exit(EXIT_FAILURE);
  }
  if (getcontext(ctx) < 0) {
    perror("getcontext");
    return -2;
    exit(EXIT_FAILURE);
  }
  ctx->uc_link = next;
  ctx->uc_stack.ss_sp    = stack;
  ctx->uc_stack.ss_size  = STACK_SIZE;
  ctx->uc_stack.ss_flags = 0;
  return 1;
}

void setCTXLink(ucontext_t* ctx, ucontext_t* next){
  ctx->uc_link = next;
}

/* Set a timer and a handler for the timer.

   Arguments

   type: type of timer, one of ITIMER_REAL, ITIMER_VIRTUAL, or ITIMER_PROF.

   handler: timer signal handler.

   ms: time in ms for the timer. 

 */
void set_timer(void (*handler) (int), int ms) {
  struct itimerval timer;
  struct sigaction sa;
  /* Install signal handler for the timer. */
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler =  handler;
  sigaction (SIGALRM, &sa, NULL);
  /* Configure the timer to expire after ms msec... */
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = ms*1000;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  if (setitimer (ITIMER_REAL, &timer, NULL) < 0) {
    perror("Setting timer");
    exit(EXIT_FAILURE);
  };
}

/* Timer signal handler. */
void timer_handler (int signum) {
  set_timer(timer_handler, TIMEOUT);
  yield();
}

/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/
int  init(){
  set_timer(timer_handler, TIMEOUT);

  tail = head = (thread_t*)malloc(sizeof(thread_t));
  if(createCTX(&(head->ctx), NULL)!= 1) return -1;
  head->state = running;
  head->next = NULL;
  head->tid = tidProducer++;
  return 1;
}

tid_t spawn(void (*start)()){
  thread_t* newT = (thread_t*)malloc(sizeof(thread_t));

  if(newT == NULL) return -1;
  if(createCTX(&(newT->ctx), &(head->ctx))!= 1) return -2;
  newT->state = ready;
  newT->next = NULL;
  newT->tid = tidProducer++;
  
  setCTXLink(&(tail->ctx), &(newT->ctx));
  
  tail = tail->next = newT;
  makecontext(&(newT->ctx), start, 0);

  return newT->tid;
}

void yield(){
  if(head->state == running) head->state = ready;

  thread_t *toRun = head->next;
  while(toRun != NULL && toRun->state == terminated){
    thread_t* terminatedThread = toRun;
    toRun = toRun->next;
    free(terminatedThread);
  }

  if(toRun == NULL){
    tail = head;
    head->next = NULL;
    head->state = ready;
    return;
  }
  tail = tail->next = head;
  tail->next = NULL;
  head = toRun;
  if(head->state == ready) head->state = running;

  swapcontext(&(tail->ctx), &(head->ctx));
}

void done(){
  head->state = terminated;
  thread_t *curr = head;
  while(curr!=NULL){
    if(curr->state == waiting) curr->state = ready;
    curr = curr->next;
  }
  yield();
}

tid_t join() {
  head->state = waiting;
  while(head->state == waiting)
    yield();
  thread_t *curr = head;
  while(curr!= NULL){
    if(curr->state == terminated){ 
      return curr->tid;
    }
    curr = curr->next;
  }
  return -1;
}
