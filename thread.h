#ifndef _THREAD_H
#define _THREAD_H

#include <time.h>// needed for getrusage
#include <pthread.h>
#include <netinet/in.h> // inet_addr()
/*
#include "./server/server.h"
#include "./server/command_analysis.h"
*/

#define DELTA_T 0.02 /* [s] */

#define NSEC_PER_SEC  1000000000 // = 1 [sec]
#define INTERVAL  10000000 // [ns]


/*コマンド情報の構造体*/
typedef struct{
    char large_type;
    char small_type;
    char spare3;
    char left_command;
    char right_command;
    char OP3;
    char OP4;
    char spare4;
    char error_code1;
    char error_code2;
}Command_Info;

typedef struct{
  pthread_mutex_t mutex;
  int recv_flag;
  int end_flag;
  Command_Info command;
  char recv_data[19];
  struct sockaddr_in addr;
}Thread_Arg;

int make_new_thread(int thNum, pthread_t *thread, Thread_Arg *thread_arg);
/*
extern void *Thread_Server(void *_thread_arg);
extern void *Thread_Motor(void *_thread_arg);
extern void *Thread_Camera(void *_thread_arg);
*/
// extern void Command_Copy(Command_Info org, Command_Info *copy);

/*
extern "C"{
  extern void *Thread_Camera(void *_thread_arg);
  extern void Command_Copy(Command_Info org, Command_Info *copy);
}
*/
#endif

