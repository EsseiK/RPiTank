#ifndef _COMMAND_ANAYSIS_H
#define _COMMAND_ANAYSIS_H


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

Command_Info Sock_Command_Analysis(char *recv_data);

#endif
