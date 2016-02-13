#include <stdio.h>
#include "thread.h"

void *Thread_Server(void *_thread_arg) {
  Thread_Arg *thread_arg = (Thread_Arg*)_thread_arg;

  printf("--- thread server ---\n");

  // データ受信
  Socket_Recv_Sock(thread_arg->recv_data);
  printf("--- Socket_Recv_Soc ---\n");
  printf("recv : %s\n", thread_arg->recv_data);
  // コマンド解析
  // ここで余計なデータが入る（thread_arg->commandがrecv_dataの後ろに入る）
  thread_arg->command = Sock_Command_Analysis(thread_arg->recv_data);
  printf("--- Sock_Command_Analysis ---\n");
  printf("recv : %s\n", thread_arg->recv_data);

}
