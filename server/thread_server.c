#include <stdio.h>
#include "../thread.h"

void *Thread_Server(void *_thread_arg) {
  Thread_Arg *thread_arg = (Thread_Arg*)_thread_arg;

  printf("--- thread server ---\n");

  // データ受信
  Socket_Recv_Sock(thread_arg);
  thread_arg->end_flag = 1;
  printf("--- thread server end ---\n\n");
}
