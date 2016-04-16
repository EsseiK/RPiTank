#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command_analysis.h"

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

// Command_Info Sock_Command_Analysis(char *recv_data, Command_Info *command) {
void Sock_Command_Analysis(char *recv_data, Command_Info *command) {
  int i;
  //Command_Info command;
  printf("--- command annaysis ---\n");

  // printf("recv : %s\n", recv_data);
  printf("recv: ");
  for(i = 0; i < 18; i++) {
    printf("%c", recv_data[i]);
  }
  printf("\n");

  switch(recv_data[7]) {
  case 'M':
    printf("Motor\n");
    command->large_type     = recv_data[7];
    command->small_type     = recv_data[8];
    command->spare3         = recv_data[9];
    command->left_command   = recv_data[10];
    command->right_command  = recv_data[11];
    command->OP3            = recv_data[12];
    command->OP4            = recv_data[13];
    command->spare4         = recv_data[14];
    command->error_code1    = recv_data[15];
    command->error_code2    = recv_data[16];
    break;
  case 'C':
    printf("Camera\n");
    command->large_type     = recv_data[7];
    command->small_type     = recv_data[8];
    command->spare3         = recv_data[9];
    command->left_command   = recv_data[10];
    command->right_command  = recv_data[11];
    command->OP3            = recv_data[12];
    command->OP4            = recv_data[13];
    break;
  case 'S':
    printf("System\n");
    command->large_type     = recv_data[7];
    command->small_type     = recv_data[8];
    command->spare3         = recv_data[9];
    command->left_command   = recv_data[10];
    command->right_command  = recv_data[11];
    command->OP3            = recv_data[12];
    command->OP4            = recv_data[13];
  default:
    printf("ERROR\n");
    break;
  }
  printf("bef ret : %s\n", recv_data);
  // return command;
}
/*
int main() {
  char recv_data[19] = "0pt4567M9012345678";
  Command_Info command;

  command = Sock_Command_Analysis(recv_data);
  printf("recive command\n");
  printf("large_type    : %c\n", command.large_type);
  printf("left_command  : %c\n", command.left_command);
  printf("right_command : %c\n", command.right_command);
  printf("OP3           : %c\n", command.OP3);
  printf("OP4           : %c\n", command.OP4);
  return 0;
}
*/
