#include <stdio.h>
#include <unistd.h>
#include "thread.h"
#include "./server/command_analysis.h"
/* #include "./camera/camera.hpp" */

//#include <string.h>

void Thread_Arg_Init(Thread_Arg *thread_arg);

#define THREAD 3

int main () {
  pthread_t thread[2]; // thread ID
  Thread_Arg thread_arg;
  Command_Info info;
  char tmp[12]="M0055000000";
  int i = 0;
  int ret =0;
  int cfin = 1;
  char c;

  printf("--- main ---\n");

  // thread arg init
  Thread_Arg_Init(&thread_arg);

  for ( i = 0; i < THREAD; i++ ) {
    make_new_thread(i, &thread[i], &thread_arg);
  }

  // waite thread end
  for(i = 0; i < THREAD; i++) {
    pthread_join(thread[i], NULL);
  }
  /*
  printf("=== Result ===\n");
  printf("recive data\n");
  printf("recive command\n");
  printf("large_type    : %c\n", thread_arg.command.large_type);
  printf("left_command  : %c\n", thread_arg.command.left_command);
  printf("right_command : %c\n", thread_arg.command.right_command);
  printf("OP3           : %c\n", thread_arg.command.OP3);
  printf("OP4           : %c\n", thread_arg.command.OP4);
  */
  usleep(1000); /* モータ終了待ち */
  printf("### Main END ###\n");
  /* scanf("%c", &c); */

  return 0;
}

void Thread_Arg_Init(Thread_Arg *thread_arg){
  int i = 0;
  thread_arg->recv_flag = 0;
  thread_arg->end_flag = 0;
  thread_arg->command.large_type     = '0';
  thread_arg->command.small_type     = '0';
  thread_arg->command.spare3         = '0';
  thread_arg->command.left_command   = '0';
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
