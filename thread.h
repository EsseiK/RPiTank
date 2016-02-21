#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>




/*
#include "./server/server.h"
*/
#include "./server/command_analysis.h"

#define THREAD 2
#define DELTA_T 0.02 /* [s] */

#define NSEC_PER_SEC  1000000000 // = 1 [sec]
#define INTERVAL  10000000 // [ns]

typedef struct{
  pthread_mutex_t mutex;
  int recv_flag;
  int end_flag;
  Command_Info command;
  char recv_data[19];
}Thread_Arg;

int make_new_thread(int thNum, pthread_t *thread, Thread_Arg *thread_arg);
void *Thread_Server(void *_thread_arg);
void *Thread_Motor(void *_thread_arg);

#endif
