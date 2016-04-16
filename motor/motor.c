/*モータ制御*/
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include "motor.h"

/*右モータPIN*/
#define RIGHTMOTORADVANCEPIN  1      //前進
#define RIGHTMOTORREVERSEPIN  26      //後進

/*左モータPIN*/
#define LEFTMOTORADVANCEPIN  24      //前進
#define LEFTMOTORREVERSEPIN  23      //後進

#define CLOCK 400
#define RANGE 1024
#define PWM_RANGE 1024

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
static inline void tsnorm(struct timespec *ts)
{
  while (ts->tv_nsec >= NSEC_PER_SEC) {
    ts->tv_nsec -= NSEC_PER_SEC;
    ts->tv_sec++;
  }
  /* printf("tv_sec: %d\ntv_nsec: %ld\n", ts->tv_sec, ts->tv_nsec); */
}


/*モータ情報の初期化は全体のメイン処理の中で*/
/*左モータ情報の初期化*/
int Motor_Init_Left(motor_state_info* motor_state_left )
{
  motor_state_left -> id = MOTOR_LEFT;
  motor_state_left -> pwm_advance_pin = LEFTMOTORADVANCEPIN;
  motor_state_left -> pwm_reverse_pin = LEFTMOTORREVERSEPIN;
  printf("Motor_Init_Left pwm_advance_pin = %u\r\n",motor_state_left -> pwm_advance_pin);
  printf("Motor_Init_Left pwm_reverse_pin = %u\r\n",motor_state_left -> pwm_reverse_pin);

  pinMode(RIGHTMOTORADVANCEPIN, PWM_OUTPUT);
  pwmSetMode (PWM_MODE_MS );// not Balanced mode but Mark:Space mode
  pwmSetClock(CLOCK);
  pwmSetRange(RANGE);
  // setting SW PWM
  softPwmCreate(LEFTMOTORADVANCEPIN,0,PWM_RANGE); //advance
  softPwmCreate(LEFTMOTORREVERSEPIN,0,PWM_RANGE);//reverse
  //softPwmCreate(LEFTMOTORREVERSEPIN,0,RANGE);//reverse
  return 0;
}

/*右モータ情報の初期化*/
int Motor_Init_Right(motor_state_info* motor_state_right)
{
  motor_state_right -> id = MOTOR_RIGHT;
  motor_state_right -> pwm_advance_pin = RIGHTMOTORADVANCEPIN;
  motor_state_right -> pwm_reverse_pin = RIGHTMOTORREVERSEPIN;

  // setting HW PWM for compare;


  softPwmCreate(RIGHTMOTORADVANCEPIN,0,PWM_RANGE);//advance
  softPwmCreate(RIGHTMOTORREVERSEPIN,0,PWM_RANGE);//reverse
  printf("Motor_Init_Right pwm_advance_pin = %u\r\n",motor_state_right -> pwm_advance_pin);
  printf("Motor_Init_Right pwm_reverse_pin = %u\r\n",motor_state_right -> pwm_reverse_pin);

  return 0;
}


/*左のコマンド値、右のコマンド値が2～8の値で収まっていることをチェックする。*/
/*不備がある場合はエラー値を返す、ない場合は0を返す。*/
int Motor_CommandCheck(Command_Info* command)
{
  int error_check;
  error_check = NONEERROR;
  char tmp[2];
  int tmpi = 0;
  tmp[0] = command->left_command;
  tmp[1] = 0x00;
  tmpi =(int)atoi(tmp);
  if(  (tmpi < COMMANDCHECK_MIN) ||  (COMMANDCHECK_MAX < tmpi)  )
    {
      error_check = COMMANDERROR;
      printf("Motor_CommandCheck left_command err %d\r\n",tmpi);
    }
  tmp[0] = command->right_command;
  tmp[1] = 0x00;
  tmpi =(int)atoi(tmp);
  if(  (tmpi < COMMANDCHECK_MIN) ||  (COMMANDCHECK_MAX < tmpi)  )
    {
      printf("Motor_CommandCheck right_command err %d\r\n",tmpi);
    }

  return error_check;
}


/*変換対象のコマンド値をもとにモータ状態レベルと向きを算出し、状態を保持する。*/
int Motor_CommandTrans(char command_level,motor_state_info* motor_state)
{
  int error_check;
  error_check = NONEERROR;
  char tmp[2];
  int tmpi = 0;
  tmp[0] = command_level;
  tmp[1] = 0x00;
  tmpi =(int)atoi(tmp);
  printf("Motor_CommandTrans command_level = %d \r\n",tmpi);
  if(tmpi < ROTATIONBORDER)
    {
      motor_state -> level = (unsigned short)(ROTATIONBORDER - tmpi);
      motor_state -> rotation_direction = REVERSE;
    }
  else if(ROTATIONBORDER <= tmpi)
    {
      motor_state -> level = (unsigned short)(tmpi - ROTATIONBORDER);
      motor_state -> rotation_direction = ADVANCE;
    }
  else
    {
      error_check = COMMANDTRANSERROR;
    }

  return error_check;
}

/*モータ出力レベルを元にPWM出力値(0～100％)を選出する。*/
/*出力するPINを設定する。*/
int Motor_OutCalc(motor_state_info* motor_state)
{
  int error_check;
  error_check = NONEERROR;

  switch(motor_state -> level)
    {
    case OUTLEVEL0:
      motor_state -> pwm_out_value = PWMOUT0;
      break;
    case OUTLEVEL1:
      motor_state -> pwm_out_value = PWMOUT30;
      break;
    case OUTLEVEL2:
      motor_state -> pwm_out_value = PWMOUT60;
      break;
    case OUTLEVEL3:
      motor_state -> pwm_out_value = PWMOUT90;
      break;
    default:
      error_check = PWMOUTERROR;
      break;
    }
  //    {
  //        error_check = PWMOUTCALCERROR;
  //    }

  return error_check;
}

/*モータ出力前の標準となる(コマンド処理&出力値計算の実施)*/
/*左右のモータそれぞれで処理を行う。*/
int Motor_Prepare(Command_Info *command, motor_state_info *motor_state)
{

  char level;
  int error_check;

  error_check = NONEERROR;
  level = CHARDATA0;
  printf("Motor_Prepare id  %u\r\n",motor_state -> id);
  switch (motor_state -> id)
    {
    case MOTOR_LEFT:
      level = command -> left_command;
      break;
    case MOTOR_RIGHT:
      level = command -> right_command;
      break;
    default:
      error_check = PREPAREERROR;
      printf("Motor_Prepare motor_state -> id illegal %u\r\n",motor_state -> id);
      break;
    }

  if(error_check == NONEERROR)
    {
      error_check = Motor_CommandTrans(level,motor_state);
      printf("Motor_CommandTrans do %d\r\n",error_check);
      if(error_check == NONEERROR)
        {
	  error_check = Motor_OutCalc(motor_state);
	  printf("Motor_OutCalc do %d\r\n",error_check);
        }
    }
  return error_check;
}


int Motor_Output(motor_state_info* motor_state_left,motor_state_info* motor_state_right)
{
  //前進、後進のPIN番号をモータ状態に保持させている
  if (PWMOUT0 < ( (int)(motor_state_left -> pwm_out_value) ) )
    {
      //pwm output is over 0
      printf("Motor_Output [left] PWMOUT0<pwm_out_value= %d\r\n",( (int)(motor_state_left -> pwm_out_value) ));
      if (  ( (int)(motor_state_left -> rotation_direction) ) == ADVANCE)
	{
	  softPwmWrite( motor_state_left -> pwm_advance_pin, motor_state_left -> pwm_out_value);
	  softPwmWrite(  motor_state_left -> pwm_reverse_pin, PWMOUT0);
	  printf("Motor_Output [left] ADVANCE %u\r\n",motor_state_left -> rotation_direction);
	}
      else if ( ( (int)(motor_state_left -> rotation_direction) ) == REVERSE)
	{
	  softPwmWrite( motor_state_left -> pwm_advance_pin, PWMOUT0);
	  softPwmWrite( motor_state_left -> pwm_reverse_pin, motor_state_left -> pwm_out_value);
	  printf("Motor_Output [left] REVERSE %u level %u \r\n",motor_state_left -> rotation_direction,motor_state_left -> level);
	}
    }
  else
    {
      //pwm output equals 0
      softPwmWrite( motor_state_left -> pwm_advance_pin, PWMOUT0);
      softPwmWrite( motor_state_left -> pwm_reverse_pin, PWMOUT0);
      printf("Motor_Output [left] PWMOUT0==pwm_out_value= %d\r\n",( (int)(motor_state_left -> pwm_out_value) ));

    }

  if (PWMOUT0 < (motor_state_right -> pwm_out_value))
    {
      printf("Motor_Output [right] PWMOUT0<pwm_out_value= %d\r\n",( (int)(motor_state_right -> pwm_out_value) ));
      if ((motor_state_right -> rotation_direction) == ADVANCE)
	{
	  //pwm output is over 0
	  //pwmWrite( motor_state_right -> pwm_advance_pin, motor_state_right -> pwm_out_value);//hard pwm
	  softPwmWrite( motor_state_right -> pwm_advance_pin, motor_state_right -> pwm_out_value);
	  softPwmWrite( motor_state_right -> pwm_reverse_pin, PWMOUT0);//soft pwm
	  printf("Motor_Output [right] ADVANCE %u level %u \r\n",motor_state_right -> rotation_direction,motor_state_right -> level);
	}
      else if ((motor_state_right -> rotation_direction) == REVERSE)
	{
	  //pwmWrite( motor_state_right -> pwm_advance_pin, PWMOUT0);
	  softPwmWrite( motor_state_right -> pwm_advance_pin, PWMOUT0);
	  softPwmWrite( motor_state_right -> pwm_reverse_pin, motor_state_right -> pwm_out_value);
	  printf("Motor_Output [right] REVERSE %u\r\n",motor_state_right -> rotation_direction);
	}
    }
  else
    {
      //pwm output equals 0
      //pwmWrite( motor_state_right -> pwm_advance_pin, PWMOUT0);
      softPwmWrite( motor_state_right -> pwm_advance_pin, PWMOUT0);
      softPwmWrite( motor_state_right -> pwm_reverse_pin, PWMOUT0);
      printf("Motor_Output [right] PWMOUT0==pwm_out_value= %d\r\n",( (int)(motor_state_right -> pwm_out_value) ));

    }
  return 0;
}

/* add hiraoka */
int Motor_Stop() {
  /* モータ出力停止 */
  softPwmWrite(RIGHTMOTORADVANCEPIN, PWMOUT0);
  softPwmWrite(RIGHTMOTORREVERSEPIN, PWMOUT0);
  softPwmWrite(LEFTMOTORADVANCEPIN, PWMOUT0);
  softPwmWrite(LEFTMOTORREVERSEPIN, PWMOUT0);
  printf("Motor_STOP\n");

  return 0;
}

/*モータ制御のメイン処理を実施する。*/
int Motor_main(Command_Info *command, Thread_Arg *thread_arg)
{
  int error_check;
  int motor_index;
  int new_command;
  motor_state_info motor_state_left;
  motor_state_info motor_state_right;

  struct timespec interT;
  int interval = DELTA_T * 1000000000; //[ns]

  clock_gettime(0, &interT);


  /* init */
  motor_state_left.id = MOTOR_LEFT;
  motor_state_right.id = MOTOR_RIGHT;
  error_check = NONEERROR;

  // left motor inti
  error_check = Motor_Init_Left(&motor_state_left);

  if(error_check<0){
    printf("Motor_Init_Left err %d\r\n",error_check);
    return -1;
  }else{
    printf("Motor_Init_Left do\r\n");
  }

  error_check = NONEERROR;

  // rught motor inti
  error_check = Motor_Init_Right(&motor_state_right);
  if(error_check<0){
    return -2;
  }else{
    printf("Motor_Init_Right do\r\n");
  }

  while(1){
    if(thread_arg->end_flag == 1){
      break;
    }

    pthread_mutex_lock(&thread_arg->mutex);
    {
      /* 受信したらコピー */
      /* printf("flag: %d\n", thread_arg->recv_flag); */
      if(thread_arg->recv_flag == 1){
	/* command copy */
	Command_Copy(thread_arg->command, command);
	if(command->large_type == 'M'){
	  new_command = 1;
	  thread_arg->recv_flag = 0;
	  printf("\n=== Get New Motor Command ===\n");
	} else {
	  new_command = 0;
	}
      }
    }
    pthread_mutex_unlock(&thread_arg->mutex);

    if(new_command == 1){
      printf("test: %c\n", command->large_type);

      error_check =    Motor_CommandCheck(command);

      if (error_check != NONEERROR)
	{
	  printf("Motor_CommandCheck err %d\r\n",error_check);
	  return -3;

	}else{
	error_check = Motor_Prepare(command,&motor_state_left);

      }
      printf("Motor_Prepare left do\r\n");

      if (error_check != NONEERROR)
	{
	  printf("Motor_Prepare err %d\r\n",error_check);
	  return -4;
	}

      error_check = Motor_Prepare(command,&motor_state_right);

      printf("Motor_Prepare right do\r\n");

      if (error_check != NONEERROR){
	printf("Motor_Prepare err %d\r\n",error_check);
	return -5;
      }

      //motor control do
      pthread_mutex_lock(&thread_arg->mutex);
      {

	printf("---------Motor_Output pre info----------\r\n");
	printf("[motor_state_left] id = %u\r\n",motor_state_left.id);
	printf("[motor_state_left] level = %u\r\n",motor_state_left.level);
	printf("[motor_state_left] rotation_direction = %u\r\n",motor_state_left.rotation_direction);
	printf("[motor_state_left] pwm_out_value = %u\r\n",motor_state_left.pwm_out_value);
	printf("[motor_state_left] pwm_advance_pin = %u\r\n",motor_state_left.pwm_advance_pin);
	printf("[motor_state_left] pwm_reverse_pin = %u\r\n",motor_state_left.pwm_reverse_pin);

	printf("[motor_state_right] id = %u\r\n",motor_state_right.id);
	printf("[motor_state_right] level = %u\r\n",motor_state_right.level);
	printf("[motor_state_right] rotation_direction = %u\r\n",motor_state_right.rotation_direction);
	printf("[motor_state_right] pwm_out_value = %u\r\n",motor_state_right.pwm_out_value);
	printf("[motor_state_right] pwm_advance_pin = %u\r\n",motor_state_right.pwm_advance_pin);
	printf("[motor_state_right] pwm_reverse_pin = %u\r\n",motor_state_right.pwm_reverse_pin);
	printf("---------Motor_Output pre info----------\r\n");
	thread_arg->recv_flag = 0;
      }
      pthread_mutex_unlock(&thread_arg->mutex);

      error_check = Motor_Output(&motor_state_left, &motor_state_right);

      if(error_check <0){
	printf("Motor_Output ret = %d\r\n",error_check);
      }else{
	printf("Motor_Output OK\r\n");
      }
    }
    new_command = 0;

    /* 制御周期まで待機 */
    interT.tv_nsec += interval; /*  */
    /* printf("interval:%d\n", interval); */
    /* printf("interT.tv_nsec:%ld\n", interT.tv_nsec); */
    tsnorm(&interT);
    clock_nanosleep(0, TIMER_ABSTIME, &interT, NULL); /* 渡した時間まで待つ */

  }
  /* Motor_Stop(); */
  return 0;
}

