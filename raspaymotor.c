/*モータ制御*/

/*ソースコードを書いていて思ったこと*/

/*１．モータ構造体にIDで番号を持たせたほうがいいのでは?*/
/*IDで持たせれば、motor_kindは必要なくなり、どこかで左右のモータを判定する必要があってもIDでわかる。*/

/*２．出力PIN情報は、初期化時に持たせておいたほうが良いのではないか?*/
/*(ここよくわかっていない)STOPは、前進及び後進のPINにPWM出力をすることで停止する。(と自分は理解しています。)*/
/*ならば、あらかじめ左右のモータのPIN番号は初期化時に設定しておいて、PWM出力でどちらに出力を行うか、
もしくは、Pin状態を削除して、固定値を入れるようにしたほうが良いのではないか?*/

/*３．上記の問題を勝手にやったもの=raspaymotor2.c及び詳細設計データ構造2.asta*/


#include <wiringPi.h>
#include <stdio.h>

/*.h予定*/
/*右モータPIN*/
#define RIGHTMOTORADVANCEPIN        //前進
#define RIGHTMOTORREVERSEPIN        //後進

/*左モータPIN*/
#define LEFTMOTORADVANCEPIN        //前進
#define LEFTMOTORREVERSEPIN        //後進

#define STOPPIN 0

/*コマンドチェック用*/
#define COMMANDCHECK_MIN 2
#define COMMANDCHECK_MAX 8
#define ROTATIONBORDER 5

/*回転方向*/
#define ADVANCE 0
#define REVERSE 1

/*初期化用*/
#define CHARDATA0 0
#define INTDATA0 0

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
}


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
}

/*モータID*/
typedef enum 
{
    MOTOR_LEFT = 0,
    MOTOR_RIGHT,
    MOTOR_MAX
}


/*モータ状態情報の構造体*/
typedef struct motor_state_info 
{
    unsigned short level;
    unsigned short rotation_direction;
    unsigned short pwm_out_value;
    unsigned short pwm_pin;
}motor_state_info;

/*コマンド情報の構造体*/
typedef struct command_info
{
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
}command_info;
/*.hここまで*/

int Motor_main(command_info* command);
int Motor_CommandCheck(command_info* command);
int Motor_Prepare(command_info command, motor_state_info motor_state , char motor_kind)
int Motor_CommandTrans(char command_level,motor_state_info* motor_state)
int Motor_OutCalc(motor_state_info* motor_state)



/*モータ制御のメイン処理を実施する。*/
int Motor_main(command_info* command)
{
    int error_check;
    int motor_index;
    motor_state_info motor_state_left;
    motor_state_info motor_state_right;

    error_check = NONEERROR;
    
    Motor_CommandCheck(command);
    
    if (error_check = NONEERROR)
    {
        Motor_Prepare(command,motor_state_left,MOTOR_LEFT);
    }

    if (error_check = NONEERROR)
    {
        Motor_Prepare(command,motor_state_right,MOTOR_RIGHT);
    }
    
    return 0;
    
}


/*左のコマンド値、右のコマンド値が2～8の値で収まっていることをチェックする。*/
/*不備がある場合はエラー値を返す、ない場合は0を返す。*/
int Motor_CommandCheck(command)
{
    int error_check;
    error_check = NONEERROR;
    
    if((command->left_command < COMMANDCHECK_MIN)||(COMMANDCHECK_MAX < command->left_command))
    {
        error_check = COMMANDERROR;
    }
    
    if((command->right_command < COMMANDCHECK_MIN)||(COMMANDCHECK_MAX < command->right_command))
    {
        error_check = COMMANDERROR;
    }
    
    return error_check;
    
}


/*モータ出力前の標準となる(コマンド処理&出力値計算の実施)*/
/*左右のモータそれぞれで処理を行う。*/
int Motor_Prepare(command_info command, motor_state_info motor_state,int motor_kind)
{

    char level;
    int errorcheck;

    error_check = NONEERROR;
    level = CHARDATA0;
    
    switch (motor_kind)
    {
        case MOTOR_LEFT:
            level = command -> left_command;
            break;
        case MOTOR_RIGHT:
            level = command -> right_command;
            break;
    default:
            error_check = PREPAREERROR;
            break;
    }

    if(errorcheck = NONEERROR)
    {
        errorcheck = Motor_CommandTrans(level,motor_state);
        if(errorcheck = NONEERROR)
        {
            errorcheck = Motor_OutCalc(motor_state);
        }
    }
    
    return errorcheck;
    
}

/*変換対象のコマンド値をもとにモータ状態レベルと向きを算出し、状態を保持する。*/
int Motor_CommandTrans(char command_level,motor_state_info* motor_state)
{
    int error_check;
    error_check = NONEERROR;
    
    if(command_level < ROTATIONBORDER)
    {
        motor_state -> level = ROTATIONBORDER - command_level;
        motor_state -> rotation_direction = REVERSE;
    }
    else if(ROTATIONBORDER <= command_level)
    {
        motor_state -> level = command_level - ROTATIONBORDER;
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

	
	/*出力するPIN番号の処理をどうすれば良いか検討中*/
   
    return error_check;
    
}


/*検討中*/
int Motor_Output(motor_state_info* motor_state_left,motor_state_info* motor_state_right)
{
    
    
    
    
    
    
    
    
    
    
}
