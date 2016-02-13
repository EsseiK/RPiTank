#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command_anaysis.h"

Command_Info Sock_Command_Analysis(char *recv_data) {
  int i;
  Command_Info command;
  printf("--- command annaysis ---\n");

  printf("recv : %s\n", recv_data);

  switch(recv_data[7]) {
  case 'M':
    printf("Motor\n");
  printf("??? : %s\n", recv_data);
    command.large_type     = recv_data[7];
    command.small_type     = recv_data[8];
    command.spare3         = recv_data[9];
    command.left_command   = recv_data[10];
    command.right_command  = recv_data[11];
    command.OP3            = recv_data[12];
    command.OP4            = recv_data[13];
    command.spare4         = recv_data[14];
    command.error_code1    = recv_data[15];
    command.error_code2    = recv_data[16];
  printf("??? : %s\n", recv_data);
    break;
  case 'C':
    printf("Camera\n");
    break;
  default:
    printf("ERROR\n");
    break;
  }
  printf("bef ret : %s\n", recv_data);
  return command;
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
