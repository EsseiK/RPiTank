#include <stdio.h>
#include "../thread.h"
#include "motor.h"

void *Thread_Motor(void *_thread_arg) {
  Thread_Arg *thread_arg = (Thread_Arg*)_thread_arg;
  Command_Info command;
  int ret = 0;

    printf("--- thread motor ---\n");
  if(wiringPiSetupGpio() == -1){
    printf("---------wiringPiSetupGpio missing----------\r\n");
    /* return; */
  }

    ret = Motor_main(&command, thread_arg);

    printf("--- thread motor loop end ---\n\n");
    pthread_mutex_lock(&thread_arg->mutex);
    {
      thread_arg->end_flag = 1;
    }
    pthread_mutex_unlock(&thread_arg->mutex);
    Motor_Stop();
}
