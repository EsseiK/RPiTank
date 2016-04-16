#include <stdio.h>
#include "./thread.h"

extern void *Thread_Server(void *_thread_arg);
extern void *Thread_Motor(void *_thread_arg);
// extern "C" void *Thread_Camera(void *_thread_arg);
extern void *Thread_Camera(void *_thread_arg);

int error(int at);

int make_new_thread(int thNum, pthread_t *thread, Thread_Arg *thread_arg) {
  pthread_attr_t attr; // set attribute of thread
  size_t stack_size = 0;
  int i = 0;

  printf("--- make new thread ---\n");

  // mutex init
  pthread_mutex_init(&thread_arg->mutex, NULL);

  // init to default values
  if(pthread_attr_init(&attr)) {
    error(1);
  }
  // Set the requested stacksize  for thread
  pthread_attr_getstacksize(&attr, &stack_size);
  if(pthread_attr_setstacksize(&attr, stack_size)) {
    error(2);
  }

  // thread create
  switch(thNum) {
  case 0:
    pthread_create(thread, &attr, Thread_Server, (void*) thread_arg);
    printf("--- make server thread ---\n");
    break;
  case 1:
    pthread_create(thread, &attr, Thread_Motor, (void*) thread_arg);
    printf("--- make motor thread ---\n");
    break;
  case 2:
    pthread_create(thread, &attr, Thread_Camera, (void*) thread_arg);
    printf("--- make camera thread ---\n");
    break;
  default:
    puts("error of make thread");
    return -1;
    break;
  }
    return 0;
}

int error(int at)
{
  /* Just exit on error */
  fprintf(stderr, "Some error occured at %d", at);
  return 0;
}
