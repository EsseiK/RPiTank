#include <stdio.h>
#include "thread.h"
#include "./server/command_analysis.h"
#include "./motor/motor.h"

//#include <string.h>

void Thread_Arg_Init(Thread_Arg *thread_arg);

int main () {
  pthread_t thread[2]; // thread ID
  Thread_Arg thread_arg;
  Command_Info info;
  char tmp[11]="M005500000\0";
  int i = 0;
  int ret =0;
  int cfin = 1;

  printf("--- main ---\n");
  if(wiringPiSetupGpio() == -1){
    printf("---------wiringPiSetupGpio missing----------\r\n");
    return;
  }

  // thread arg init
  Thread_Arg_Init(&thread_arg);

  for ( i = 0; i < THREAD; i++ ) {
    make_new_thread(i, &thread[i], &thread_arg);
  }

  // waite thread end
  for(i = 0; i < THREAD; i++) {
    pthread_join(thread[i], NULL);
  }
  printf("=== Result ===\n");
  printf("recive data\n");
  printf("recive command\n");
  printf("large_type    : %c\n", thread_arg.command.large_type);
  printf("left_command  : %c\n", thread_arg.command.left_command);
  printf("right_command : %c\n", thread_arg.command.right_command);
  printf("OP3           : %c\n", thread_arg.command.OP3);
  printf("OP4           : %c\n", thread_arg.command.OP4);

  //ret = Motor_main(&thread_arg.command);

  /* while(cfin){ */
  /*   printf("end input '0'?: "); */
  /*   scanf("%d",&cfin); */
  /*   printf("end: 0, in: %d\n", cfin); */
  /* }; */

  /*  i = 0;
  info.large_type	=	tmp[i];
  i++;
  info.small_type	=	tmp[i];
  i++;
  info.spare3	=	tmp[i];
  i++;
  info.left_command	=	tmp[i];
  i++;
  info.right_command	=	tmp[i];
  i++;
  info.OP3	=	tmp[i];
  i++;
  info.OP4	=	tmp[i];
  i++;
  info.spare4	=	tmp[i];
  i++;
  info.error_code1	=	tmp[i];
  i++;
  info.error_code2	=	tmp[i];
  printf("Motor_STOP\n");

  thread_arg.command.left_command = '5';
  thread_arg.command.right_command = '5';
  ret = Motor_main(&info);
  */

  Motor_Stop();
  if(wiringPiSetupGpio() == -1){
    printf("---------wiringPiSetupGpio missing----------\r\n");
    return;
  }

  printf("### Main END ###\n");

  return 0;
}


void Thread_Arg_Init(Thread_Arg *thread_arg){
  int i = 0;
  thread_arg->recv_flag = 0;
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
