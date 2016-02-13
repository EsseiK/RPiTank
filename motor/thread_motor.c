#include <stdio.h>
#include <time.h>// needed for getrusage
#include "../thread.h"
#include "motor.h"

extern int clock_nanosleep(clockid_t __clock_id, int __flags,
                           __const struct timespec *__req,
                           struct timespec *__rem);
extern int clock_gettime(clockid_t clk_id, struct timespec *res);

/* the struct timespec consists of nanoseconds
 * and seconds. if the nanoseconds are getting
 * bigger than 1000000000 (= 1 second) the
 * variable containing seconds has to be
 * incremented and the nanoseconds decremented
 * by 1000000000.
 */
/* static inline void tsnorm(struct timespec *ts) */
/* { */
/*   while (ts->tv_nsec >= NSEC_PER_SEC) { */
/*     ts->tv_nsec -= NSEC_PER_SEC; */
/*     ts->tv_sec++; */
/*   } */
/*   printf("tv_sec: %d\ntv_nsec: %ld\n", ts->tv_sec, ts->tv_nsec); */
/* } */

void Command_Copy(Command_Info org, Command_Info *copy);


void *Thread_Motor(void *_thread_arg) {
  Thread_Arg *thread_arg = (Thread_Arg*)_thread_arg;
  Command_Info command;

  int ret = 0;
  char c;

  struct timespec interT;
  int interval = DELTA_T * 1000000000; //[ns]

  clock_gettime(0, &interT);

  //  while(1){

  /* while(1){ */
  /*   usleep(100); */
  /*     if(thread_arg->recv_flag == 1) break; */
  /*     if(thread_arg->end_flag == 1) return; */
  /* } */
    printf("--- thread motor ---\n");

    /* pthread_mutex_lock(&thread_arg->mutex); */
    /* { */
    /*   /\* 受信したらコピー *\/ */
    /*   printf("flag: %d\n", thread_arg->flag); */
    /*   if(thread_arg->flag == 1){ */
    /* 	/\* command copy *\/ */
    /* 	Command_Copy(thread_arg->command, &command); */
    /* 	thread_arg->flag = 0; */
    /* 	printf("=== Get New Command ===\n"); */
    /*   } */
    /* } */
    /* pthread_mutex_unlock(&thread_arg->mutex); */

    ret = Motor_main(&command, thread_arg);
    // ret = Motor_main_old(&command);

    printf("--- thread motor loop end ---\n\n");
    pthread_mutex_lock(&thread_arg->mutex);
    {
      thread_arg->end_flag = 1;
    }
    pthread_mutex_unlock(&thread_arg->mutex);
    /* wait */
    /* interT.tv_nsec += interval; /\*  *\/ */
    /* printf("interval:%d\n", interval); */
    /* printf("interT.tv_nsec:%ld\n", interT.tv_nsec); */
    /* tsnorm(&interT); */
    /* clock_nanosleep(0, TIMER_ABSTIME, &interT, NULL); /\* 渡した時間まで待つ *\/ */

    //scanf("%c", &c);
    //  };
}
/*
void Command_Copy(Command_Info org, Command_Info *copy){
  copy->large_type     = org.large_type;
  copy->small_type     = org.small_type;
  copy->spare3         = org.spare3;
  copy->left_command   = org.left_command;
  copy->right_command  = org.right_command;
  copy->OP3            = org.OP3;
  copy->OP4            = org.OP4;
  copy->spare4         = org.spare4;
  copy->error_code1    = org.error_code1;
  copy->error_code2    = org.error_code2;
}
*/
