// TCPServer.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

//Windows VC++ での　TCP/IP サンプルプログラム（ここからサーバー）
//クライアントから送られてきた文字列を大文字に変換して送り返す
//サーバープログラムを実行してからクライアントプログラムを実行して下さい

#include <stdio.h>
#include <string.h>
#include <sys/socket.h> // inet_addr()
#include <netinet/in.h> // inet_addr()
#include <netinet/tcp.h> // TCP_NODELAY
#include <arpa/inet.h> // inet_addr()
#pragma comment(lib, "ws2_32.lib")//pragma must set before define

#include "server.h"

#define PORT 9876 //クライアントプログラムとポート番号を合わせてください


int Socket_Recv_Sock(char *recv_data) {
  int i;
  // ポート番号，ソケット
  int srcSocket;  // 自分
  int dstSocket;  // 相手

  // sockaddr_in 構造体
  struct sockaddr_in srcAddr;
  struct sockaddr_in dstAddr;
  int dstAddrSize = sizeof(dstAddr);
  int status;
  // 各種パラメータ
  int numrcv;
  char buffer[1024];
  char pt[2];

  printf("--- server ---\n");
  // sockaddr_in 構造体のセット
  memset(&srcAddr, 0, sizeof(srcAddr));
  srcAddr.sin_port = htons(PORT);
  srcAddr.sin_family = AF_INET;
  srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  printf("--- test ---\n");
  // ソケットの生成（ストリーム型）
  srcSocket = socket(AF_INET, SOCK_STREAM, 0);
  // ソケットのバインド
  bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));
  // 接続の許可
  listen(srcSocket, 1);

  //while (1) { //ループで回すことによって何度でもクライアントからつなぐことができる

  // 接続の受付け
  printf("接続を待っています\nクライアントプログラムを動かしてください\n");
  dstSocket = accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
  printf("%s から接続を受けました\n", inet_ntoa(dstAddr.sin_addr));

  //while (1) {
  //パケットの受信
  numrcv = recv(dstSocket, recv_data, sizeof(char) * 19, 0);
  if (numrcv == 0 || numrcv == -1) {
    status = close(dstSocket);
    //break;
  }
  printf("recv: %s, size: %lu, len:%lu\n", recv_data, sizeof(recv_data), /*strlen(recv_data)*/numrcv);
  if(recv_data[0] == 0x02 && recv_data[17] == 0x03) {
    if(recv_data[1] == 'p' && recv_data[2]  == 't'){
      printf("=== Recive OK ===\n");
    }
  } else {
    printf("# STX or EDX ERROR\n");
    //break;
  }
  /*
  printf("### char ###\n");
  for(i = 0; i < 27; i++) {
    printf("i:%d, char:%c, x: %x\n", i, recv_data[i], recv_data[i]);
  }
  printf("###########\n");
  */

  if(strlen(recv_data) < 18) {
    printf("コマンドが短すぎます\n");
  }
  // パケットの送信
  send(dstSocket, recv_data, sizeof(char) * 19, 0);
  //if(strcmp(&recv_data[0], "0") == 0) break;
  //}
  //if(recv_data[1] == '0') break;
  //}
  // Windows での終了設定
  //WSACleanup();
  close(srcSocket);
  return(0);
}

/*
  int main () {
  char recv_data[19];
  Socket_Recv_Sock(recv_data);
  printf("main\n");
  printf("recv: %s\n", recv_data);
  return 0;
  }
*/
