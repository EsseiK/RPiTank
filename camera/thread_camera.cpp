#include <stdio.h>
#include "../thread.h"
#include "camera.hpp"

void *Thread_Camera(void *_thread_arg) {
  Thread_Arg *thread_arg = (Thread_Arg*)_thread_arg;
  Command_Info command;
  int ret = 0;

    printf("--- thread camera ---\n");

    ret = Camera_main(&command, thread_arg);

    printf("--- thread camera loop end ---\n\n");
    pthread_mutex_lock(&thread_arg->mutex);
    {
      thread_arg->end_flag = 1;
    }
    pthread_mutex_unlock(&thread_arg->mutex);
}
