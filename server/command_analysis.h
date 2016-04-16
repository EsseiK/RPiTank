#ifndef _COMMAND_ANAYSIS_H
#define _COMMAND_ANAYSIS_H
#include "../thread.h"

// Command_Info Sock_Command_Analysis(char *recv_data);
void Sock_Command_Analysis(char *recv_data, Command_Info *command);
void Command_Copy(Command_Info org, Command_Info *copy);
#endif
