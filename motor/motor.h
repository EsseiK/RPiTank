/*.h予定*/
#include <wiringPi.h>
#include "../thread.h"
#include "../server/command_analysis.h"

/*コマンドチェック用*/
#define COMMANDCHECK_MIN 2
#define COMMANDCHECK_MAX 8
#define ROTATIONBORDER 5

/*回転方向*/
#define ADVANCE 1
#define REVERSE 2

/*初期化用*/
#define CHARDATA0 0
#define INTDATA0 0
#define SHORT0 0

/*出力PWM*/
#define PWMOUT0 0
#define PWMOUT30 300
#define PWMOUT60 600
#define PWMOUT90 900

/*出力レベル*/
typedef enum
  {
    OUTLEVEL0 = 0,
    OUTLEVEL1,
    OUTLEVEL2,
    OUTLEVEL3,
  }LEVEL;

/*エラーの種類*/
typedef enum
  {
    NONEERROR = 0,
    COMMANDERROR,
    PREPAREERROR,
    COMMANDTRANSERROR,
    PWMOUTCALCERROR,
    PWMOUTERROR,
    MAXERROR
  }ERR;

/*モータID*/
typedef enum 
  {
    MOTOR_LEFT = 0,
    MOTOR_RIGHT,
    MOTOR_MAX
  }MID;

/*モータ状態情報の構造体*/
typedef struct
{
  unsigned short id;					//tt
  unsigned short level;
  unsigned short rotation_direction;
  unsigned short pwm_out_value;
  unsigned short pwm_advance_pin;		//tt
  unsigned short pwm_reverse_pin;		//tt
}motor_state_info;

/*コマンド情報の構造体*/
/*
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
*/
// int Motor_main(Command_Info* command);
int Motor_main(Command_Info *command, Thread_Arg *thread_arg);
int Motor_main_old(Command_Info* command);
int Motor_Stop();

/*.hここまで*/
