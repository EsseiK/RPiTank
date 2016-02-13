/*モータ制御*/

#include <wiringPi.h>
#include <stdio.h>

/*.h予定*/
/*右モータPIN*/
#define RIGHTMOTORADVANCEPIN        //前進
#define RIGHTMOTORREVERSEPIN        //後進

/*左モータPIN*/
#define LEFTMOTORADVANCEPIN        //前進
#define LEFTMOTORREVERSEPIN        //後進

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
    unsigned short id;					//tt
    unsigned short level;
    unsigned short rotation_direction;
    unsigned short pwm_out_value;
    unsigned short pwm_advance_pin;		//tt
	unsigned short pwm_reverse_pin;		//tt
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

/*モータ情報の初期化は全体のメイン処理の中で*/
/*左モータ情報の初期化*/
int Motor_Init_Left(motor_state_info* motor_state_left )
{
	motor_state_left -> id = MOTOR_LEFT;
	motor_state_left -> pwm_advance_pin = LEFTMOTORADVANCEPIN;
	motor_state_left -> pwm_reverse_pin = LEFTMOTORREVERSEPIN;
}

/*右モータ情報の初期化*/
int Motor_Init_Right(motor_state_info* motor_state_right)
{
	motor_state_right -> id = MOTOR_RIGHT;
	motor_state_right -> pwm_advance_pin = RIGHTMOTORADVANCEPIN;
	motor_state_right -> pwm_reverse_pin = RIGHTMOTORREVERSEPIN;
}
/*ここまで*/


/*モータ制御のメイン処理を実施する。*/
int Motor_main(command_info* command)
{
    int error_check;
    int motor_index;
    motor_state_info motor_state_left;
    motor_state_info motor_state_right;

    motor_state_left -> id = MOTOR_LEFT;
    motor_state_right -> id = MOTOR_RIGHT;
    error_check = NONEERROR;
    
	Motor_Init_Left(MOTOR_LEFT);
	Motor_Init_Right(motor_state_right);
	
    Motor_CommandCheck(command);
    
    if (error_check = NONEERROR)
    {
        Motor_Prepare(command,motor_state_left);
    }

    if (error_check = NONEERROR)
    {
        Motor_Prepare(command,motor_state_right);
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
int Motor_Prepare(command_info command, motor_state_info motor_state)
{

    char level;
    int errorcheck;

    error_check = NONEERROR;
    level = CHARDATA0;
    
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
    else
    {
        error_check = PWMOUTCALCERROR;
    }
    
    return error_check;
    
}

/**/
int Motor_Output(motor_state_info* motor_state_left,motor_state_info* motor_state_right)
{
	//前進、後進のPIN番号をモータ状態に保持させている
	if (PWMOUT0 < (motor_state_left -> pwm_out_value))
	{
		if ((motor_state_left -> rotation_direction) == ADVANCE)
		{
    		pwmWrite( motor_state_left -> pwm_out_value, motor_state_left -> pwm_advance_pin);
    		pwmWrite( PWMOUT0, motor_state_left -> pwm_reverse_pin);
		}
		else if ((motor_state_left -> rotation_direction) == REVERSE)
		{
    		pwmWrite( PWMOUT0, motor_state_left -> pwm_advance_pin);
    		pwmWrite( motor_state_left -> pwm_out_value, motor_state_left -> pwm_reverse_pin);
		}
	}
	else
	{
    	pwmWrite( PWMOUT60, motor_state_left -> pwm_advance_pin);
    	pwmWrite( PWMOUT60, motor_state_left -> pwm_reverse_pin);
	}
	
	
	if (PWMOUT0 < (motor_state_right -> pwm_out_value))
	{
		if ((motor_state_right -> rotation_direction) == ADVANCE)
		{
    		pwmWrite( motor_state_right -> pwm_out_value, motor_state_right -> pwm_advance_pin);
    		pwmWrite( PWMOUT0, motor_state_right -> pwm_reverse_pin);
		}
		else if ((motor_state_right -> rotation_direction) == REVERSE)
		{
    		pwmWrite( PWMOUT0, motor_state_right -> pwm_advance_pin);
    		pwmWrite( motor_state_right -> pwm_out_value, motor_state_right -> pwm_reverse_pin);
		}
	}
	else
	{
    	pwmWrite( PWMOUT60, motor_state_right -> pwm_advance_pin);
    	pwmWrite( PWMOUT60, motor_state_right -> pwm_reverse_pin);
	}
}
