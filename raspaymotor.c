/*モータ制御*/

//#include <wiringPi.h>

/*
#include "stdafx.h"
#ifndef dprintf(s,...)
 #define dprintf(s,...) printf("[%s] " s, __func__,__VA_ARGS__)
#else
#endif
*/
#include <stdio.h>
#include "raspi.h"

/*右モータPIN*/
#define RIGHTMOTORADVANCEPIN  11      //前進 /*値は適当*/
#define RIGHTMOTORREVERSEPIN  21      //後進 /*値は適当*/

/*左モータPIN*/
#define LEFTMOTORADVANCEPIN  10      //前進 /*値は適当*/
#define LEFTMOTORREVERSEPIN  20      //後進 /*値は適当*/





/*モータ情報の初期化は全体のメイン処理の中で*/
/*左モータ情報の初期化*/
int Motor_Init_Left(motor_state_info* motor_state_left )
{
	motor_state_left -> id = MOTOR_LEFT;
	motor_state_left -> pwm_advance_pin = LEFTMOTORADVANCEPIN;
	motor_state_left -> pwm_reverse_pin = LEFTMOTORREVERSEPIN;
	
	return 0;
}

/*右モータ情報の初期化*/
int Motor_Init_Right(motor_state_info* motor_state_right)
{
	motor_state_right -> id = MOTOR_RIGHT;
	motor_state_right -> pwm_advance_pin = RIGHTMOTORADVANCEPIN;
	motor_state_right -> pwm_reverse_pin = RIGHTMOTORREVERSEPIN;

	return 0;
}
/*ここまで*/



/*左のコマンド値、右のコマンド値が2～8の値で収まっていることをチェックする。*/
/*不備がある場合はエラー値を返す、ない場合は0を返す。*/
int Motor_CommandCheck(command_info* command)
{
    int error_check;
    error_check = NONEERROR;
    
    if((command->left_command < COMMANDCHECK_MIN)
     ||(COMMANDCHECK_MAX < command->left_command))
    {
        error_check = COMMANDERROR;
    }
    
    if((command->right_command < COMMANDCHECK_MIN)||(COMMANDCHECK_MAX < command->right_command))
    {
        error_check = COMMANDERROR;
    }
    
    return error_check;
    
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
    
//    {
//        error_check = PWMOUTCALCERROR;
//    }
    
    return error_check;
    
}

/*モータ出力前の標準となる(コマンド処理&出力値計算の実施)*/
/*左右のモータそれぞれで処理を行う。*/
int Motor_Prepare(command_info *command, motor_state_info *motor_state)
{

    char level;
    int error_check;

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

    if(error_check == NONEERROR)
    {
        error_check = Motor_CommandTrans(level,motor_state);
        if(error_check == NONEERROR)
        {
            error_check = Motor_OutCalc(motor_state);
        }
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
    		//pwmWrite( motor_state_left -> pwm_out_value, motor_state_left -> pwm_advance_pin);
    		//pwmWrite( PWMOUT0, motor_state_left -> pwm_reverse_pin);
		}
		else if ((motor_state_left -> rotation_direction) == REVERSE)
		{
    	//	pwmWrite( PWMOUT0, motor_state_left -> pwm_advance_pin);
    	//	pwmWrite( motor_state_left -> pwm_out_value, motor_state_left -> pwm_reverse_pin);
		}
	}
	else
	{
//    	pwmWrite( PWMOUT60, motor_state_left -> pwm_advance_pin);
  //  	pwmWrite( PWMOUT60, motor_state_left -> pwm_reverse_pin);
	}
	
	
	if (PWMOUT0 < (motor_state_right -> pwm_out_value))
	{
		if ((motor_state_right -> rotation_direction) == ADVANCE)
		{
    //		pwmWrite( motor_state_right -> pwm_out_value, motor_state_right -> pwm_advance_pin);
    //		pwmWrite( PWMOUT0, motor_state_right -> pwm_reverse_pin);
		}
		else if ((motor_state_right -> rotation_direction) == REVERSE)
		{
    //		pwmWrite( PWMOUT0, motor_state_right -> pwm_advance_pin);
    //		pwmWrite( motor_state_right -> pwm_out_value, motor_state_right -> pwm_reverse_pin);
		}
	}
	else
	{
    //	pwmWrite( PWMOUT60, motor_state_right -> pwm_advance_pin);
    //	pwmWrite( PWMOUT60, motor_state_right -> pwm_reverse_pin);
	}
	
	return 0;
}


/*モータ制御のメイン処理を実施する。*/
int Motor_main(command_info* command)
{
    int error_check;
    int motor_index;
    motor_state_info motor_state_left;
    motor_state_info motor_state_right;
	
    motor_state_left.id = MOTOR_LEFT;
    motor_state_right.id = MOTOR_RIGHT;
    error_check = NONEERROR;
    
	// left motor inti
	error_check = Motor_Init_Left(&motor_state_right);
	
	if(error_check<0){
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
	error_check =    Motor_CommandCheck(command);
    
	
	if (error_check != NONEERROR)
    {
    	return -3;
    	
    }else{
    error_check = Motor_Prepare(command,&motor_state_left);

    }
	printf("Motor_Prepare left do\r\n");	

    if (error_check != NONEERROR)
    {
    	return -4;

    }else{
    	error_check = Motor_Prepare(command,&motor_state_right);

    }
	printf("Motor_Prepare right do\r\n");	
	
	
	//motor control do

	error_check = Motor_Output(&motor_state_left, &motor_state_right);
    
	if(	error_check <0){
		printf("Motor_Output ret = %d\r\n",error_check);
	}else{
		printf("Motor_Output do\r\n");	
	}
    return 0;
    
}
void main(){
	
	int ret = 0;
	command_info info;
	
	char tmp[11]="M001234000\0";
	
	int i=0;
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
	
	printf("%c\r\n",info.large_type);	
	printf("%c\r\n",info.spare4);	
	ret = Motor_main(&info);
	
	printf("Motor_main do ret =%d\r\n",ret);
}
