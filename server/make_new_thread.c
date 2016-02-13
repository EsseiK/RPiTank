#include <stdio.h>
#include "./thread.h"

extern void *Thread_Server(void *_thread_arg);

int make_new_thread(int thNum, pthread_t *thread, Thread_Arg *thread_arg) {
  pthread_attr_t attr; // set attribute of thread
  size_t stack_size = 0;
  int i = 0;

  printf("--- make new thread ---\n");

  // mutex init
  pthread_mutex_init(&thread_arg->mutex, NULL);
  printf("--- test1 ---\n");

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
    printf("--- test2 ---\n");
    break;
  case 1:
    break;
  default:
    puts("error of make thread");
    return -1;
    break;
  }
  printf("--- test3 ---\n");
    return 0;
}

int error(int at)
{
  /* Just exit on error */
  fprintf(stderr, "Some error occured at %d", at);
  return 0;
}
