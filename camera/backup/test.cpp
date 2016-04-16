#include <stdio.h>
#include "camera.hpp"

void Thread_Arg_Init(Thread_Arg *thread_arg);

int main(){
  Thread_Arg thread_arg;
  Command_Info command;

  printf("test\n");

  // thread arg init
  Thread_Arg_Init(&thread_arg);
  // mutex init
  pthread_mutex_init(&thread_arg.mutex, NULL);

  Camera_main(&command, &thread_arg);
}

void Thread_Arg_Init(Thread_Arg *thread_arg){
  int i = 0;
  thread_arg->recv_flag = 1;
  thread_arg->command.large_type     = 'C';
  thread_arg->command.small_type     = 'P';
  thread_arg->command.spare3         = '0';
  thread_arg->command.left_command   = '1';
  thread_arg->command.right_command  = '0';
  thread_arg->command.OP3            = '0';
  thread_arg->command.OP4            = '0';
  thread_arg->command.spare4         = '0';
  thread_arg->command.error_code1    = '0';
  thread_arg->command.error_code2    = '0';

  for(i = 0; i < 19; i++){
    thread_arg->recv_data[i] = '0';
  }
}
