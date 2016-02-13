////////////////////////////////////////////////////////////////////////////////////////
//Windows VC++ での　TCP/IP サンプルプログラム（ここからはクライアント）
//入力されたデータをクライアントに送り，もらったデータを表示する
//サーバープログラムを実行してからクライアントプログラムを実行して下さい

#include <stdio.h>
#include <string.h>
#include <sys/socket.h> // inet_addr()
#include <netinet/in.h> // inet_addr()
#include <netinet/tcp.h> // TCP_NODELAY
#include <arpa/inet.h> // inet_addr()
#pragma comment(lib, "ws2_32.lib")

#define PORT 9876 //サーバープログラムとポート番号を合わせてください
#define RASS_IP "10.1.10.203"

int Socket_Send_Sock() {
	// IP アドレス，ポート番号，ソケット，sockaddr_in 構造体
	char destination[18] = RASS_IP;
	int dstSocket;
	struct sockaddr_in dstAddr;
	char *str;
	int len;
	// 各種パラメータ
	char buffer[19] = "0pt4567M9073345678";
	buffer[0]  = 0x02; // STX
	buffer[17] = 0x03; // EDX
	len = strlen(buffer);
	printf("buf %s\n, size: %lu, lne%d\n", buffer, sizeof(buffer), len);

	// Windows の場合
	//WSADATA data;
	//WSAStartup(MAKEWORD(2, 0), &data);

	// 相手先アドレスの入力と送る文字の入力
	//printf("サーバーマシンのIPは？ r or p:");
	//scanf("%s", str);
	// sockaddr_in 構造体のセット
	memset(&dstAddr, 0, sizeof(dstAddr));
	dstAddr.sin_port = htons(PORT);
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_addr.s_addr = inet_addr(destination);

	// ソケットの生成
	dstSocket = socket(AF_INET, SOCK_STREAM, 0);

	//接続
	if (connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr))) {
		printf("%s　に接続できませんでした\n", destination);
		close(dstSocket);
		return(-1);
	}
	printf("%s に接続しました\n", destination);
	//printf("右出力を入力してください\n");
	//scanf("%s", str);
	/*
	printf("右出力を入力してください\n");
	scanf("%s", str);
	strcpy(&buffer[10],&str[0]);
	printf("左出力を入力してください\n");
	scanf("%s", str);
	buffer[11] = str[0];
	*/
	printf("buf %s\n, size: %lu, lne%lu\n", buffer, sizeof(buffer), strlen(buffer));
	//while (1) {
		scanf("%s", str);
		//パケットの送信
		printf("→ %s\n, lne%lu\n", buffer, strlen(buffer));
		send(dstSocket, buffer, /*18*/len, 0);
		//パケットの受信
		recv(dstSocket, buffer, /*18*/len, 0);
		printf("→ %s\n\n", buffer);
		//	if(strcmp(&buffer[0], "0") == 0) break;
		//}

	// Windows でのソケットの終了
	close(dstSocket);
	return(0);
}

int main(){
  Socket_Send_Sock();
  return 0;
}
