#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>
#include "server.h"
#include "command_anaysis.h"

#define THREAD 1

typedef struct{
  pthread_mutex_t mutex;
  int flag;
  Command_Info command;
  char recv_data[19]; // 一番下にしないと余計なデータが入る
}Thread_Arg;

int make_new_thread(int thNum, pthread_t *thread, Thread_Arg *thread_arg);
void *Thread_Server(void *_thread_arg);

#endif
