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
tid_t tid_producer = 1000;

/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/
int 
create_ctx (ucontext_t* ctx, ucontext_t* next)
{
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

void 
set_ctx_link (ucontext_t* ctx, ucontext_t* next)
{
  ctx->uc_link = next;
}

void 
set_timer (void (*handler) (int), int ms) 
{
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

void 
reset_timer (void (*handler) (int), int ms) 
{
  struct itimerval timer;
  struct sigaction sa;
  /* Install signal handler for the timer. */
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler =  handler;
  sigaction (SIGALRM, &sa, NULL);
  /* Configure the timer to expire after ms msec... */
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  if (setitimer (ITIMER_REAL, &timer, NULL) < 0) {
    perror("Setting timer");
    exit(EXIT_FAILURE);
  };
}

void 
timer_handler (int signum) 
{
  yield();
}

/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/
int 
init ()
{
  set_timer(timer_handler, TIMEOUT);

  tail = head = (thread_t*)malloc(sizeof(thread_t));
  if(create_ctx(&(head->ctx), NULL)!= 1) return -1;
  head->state = running;
  head->next = NULL;
  head->tid = tid_producer++;
  return 1;
}

tid_t 
spawn (void (*start)())
{
  thread_t* new_thread = (thread_t*)malloc(sizeof(thread_t));

  if(new_thread == NULL) return -1;
  if(create_ctx(&(new_thread->ctx), &(head->ctx))!= 1) return -2;
  new_thread->state = ready;
  new_thread->next = NULL;
  new_thread->tid = tid_producer++;
  
  set_ctx_link(&(tail->ctx), &(new_thread->ctx));
  
  tail = tail->next = new_thread;
  makecontext(&(new_thread->ctx), start, 0);

  return new_thread->tid;
}

void 
yield ()
{
  reset_timer(timer_handler, TIMEOUT);
  if(head->state == running) head->state = ready;

  thread_t *to_run = head->next;
  while(to_run != NULL && to_run->state == terminated){
    thread_t* terminated_thread = to_run;
    to_run = to_run->next;
    free(terminated_thread->ctx.uc_stack.ss_sp);
    free(terminated_thread);
  }

  if(to_run == NULL){
    tail = head;
    head->next = NULL;
    head->state = ready;
    return;
  }

  tail = tail->next = head;
  tail->next = NULL;
  head = to_run;
  if(head->state == ready) head->state = running;
  
  set_timer(timer_handler, TIMEOUT);
  swapcontext(&(tail->ctx), &(head->ctx));
}

void 
done () {
  head->state = terminated;
  thread_t *curr = head;
  while(curr!=NULL){
    if(curr->state == waiting) curr->state = ready;
    curr = curr->next;
  }
  yield();
}

tid_t 
join () 
{
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
