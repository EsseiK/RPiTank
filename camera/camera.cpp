/*
  コンパイル用コマンド
  g++ -o send send.cpp -I/usr/local/include/opencv -lpthread -ldl -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lm -lsocket -lnsl
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "camera.hpp"

using namespace std;
using namespace cv;

// #define DISPLAY /* デバッグ用window表示 */
#define FPS 5


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


/* fps合わせ */
/* the struct timespec consists of nanoseconds
 * and seconds. if the nanoseconds are getting
 * bigger than 1000000000 (= 1 second) the
 * variable containing seconds has to be
 * incremented and the nanoseconds decremented
 * by 1000000000.
 */
static inline void tsnorm(struct timespec *ts)
{
  while (ts->tv_nsec >= NSEC_PER_SEC) {
    ts->tv_nsec -= NSEC_PER_SEC;
    ts->tv_sec++;
  }
  /* printf("tv_sec: %d\ntv_nsec: %ld\n", ts->tv_sec, ts->tv_nsec); */
}
extern "C" int clock_nanosleep(clockid_t __clock_id, int __flags,
                           __const struct timespec *__req,
                           struct timespec *__rem);
extern "C" int clock_gettime(clockid_t clk_id, struct timespec *res);

int Camera_main(Command_Info *command, Thread_Arg *thread_arg) {
// int main(int argc, char *argv[]){
  struct timespec interT;
  int interval = CAMERA_DT * 1000000000; //[ns]
  clock_gettime(0, &interT);

  //ソケットの設定
  int sock;
  struct sockaddr_in addr;
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9875);  //ポート番号

  // addr.sin_addr.s_addr = inet_addr("192.168.38.134"); //送信先IPアドレス
  // addr.sin_addr.s_addr = inet_addr("10.1.10.212"); //送信先IPアドレス
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //送信先IPアドレス
  // addr.sin_addr.s_addr = inet_addr("10.0.0.20"); //送信先IPアドレス
  // addr.sin_addr.s_addr = inet_addr("10.1.10.104"); //送信先IPアドレス
  //  addr.sin_addr.s_addr = inet_addr("10.1.10.147"); //送信先IPアドレス
  addr.sin_addr.s_addr = inet_addr("10.1.10.197"); //送信先IPアドレス
  // addr.sin_addr.s_addr = inet_addr("10.1.10.83"); //送信先IPアドレス

  //カメラの設定
  VideoCapture cap(0);
  // size
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  /* カメラが認識できるか判断 */
  if(!cap.isOpened()){
    cout << "Camera not Found!" << endl;
    return -1;
  }

  int key = 0; /* もとはwindow上でのキー入力の受けでだが，そのまま機能分岐に使用 */
  int new_command =0; /* コマンド更新 */
  int end_flag = 0;
  bool video_run = false; /* ビデオ撮影中 */
  bool streaimg  = true; /* ストリーミング中（ずっと真にしてるが，コマンドからにしたい）*/
  double w = 640, h = 360; /* 幅，高さ */
  Mat frame; /* ビデオキャプチャ画像 */
  Mat jpgimg; /* ストリーミング用 */
  cv::VideoWriter vw; /* ビデオ作成 */
  // cv::VideoWriter vw("test.avi", CV_FOURCC_DEFAULT, 10, Size(640, 480), true);
  char file_name[128];
  time_t t;
  static const int sendSize = 65500;
  char buff[sendSize];
  vector<unsigned char> ibuff;
  vector<int> param = vector<int>(2);
  param[0] = CV_IMWRITE_JPEG_QUALITY;
  param[1] = 85;
  cv::Point point(2,28);
#ifdef DISPLAY
  cv::namedWindow("Send", CV_WINDOW_AUTOSIZE); // ウインドウ生成
#endif

  /* calc fps*/
  int cnt, oldcnt;
  int startTime, endTime, diffTime;
  int fps;
  const double f = (1000 /cv::getTickFrequency());
  /* while(cvWaitKey(1) == -1){ */
  startTime = cv::getTickCount();

  cout << "=== Camera Thread Start ===" << endl;
  while(1) {
    clock_t start = clock();    // スタート時間

    if(thread_arg->end_flag == 1){
      cout << "camera.end_flag =1" << endl;
      break;
    }

    /* コマンドコピー */
    pthread_mutex_lock(&thread_arg->mutex);
    {
      /* 受信したらコピー */
      /* printf("flag: %d\n", thread_arg->recv_flag); */
      if(thread_arg->recv_flag == 1){
	/* command copy */
	Command_Copy(thread_arg->command, command);
	// addr.sin_addr.s_addr = thread_arg->addr.sin_addr.s_addr;
	if(command->large_type == 'C'){
	  new_command = 1;
	  thread_arg->recv_flag = 0;
	  printf("\n=== Get New Camera Command ===\n");
	} else {
	  new_command = 0;
	}
      }
    }
    pthread_mutex_unlock(&thread_arg->mutex);


    /* コマンドから変換 */
    if(new_command == 1){
      printf("%c, %c\n", command->small_type, command->left_command);
      if(command->small_type == 'P') {
	// 静止画撮影
	if(command->left_command == '1'){
	  key = 112;
	} else {
	  key = 0;
	}
      }else if(command->small_type == 'V') {
	// 動画撮影
	if(command->left_command == '1') {
	  key = 118;
	} else if(command->left_command == '0') {
	  key = 118;
	} else {
	  key = 0;
	}
      }
      new_command = 0;
      end_flag = 0;
      cout << "key: " << key << endl;
    }

    /* カメラからキャプチャ */
    cap >> frame;

    /* send UDP streaming */
    if(streaimg) {
      // 画像圧縮
      imencode(".jpg", frame, ibuff, param);
      // 送信ように格納
      if(ibuff.size() < sendSize ){
	for(int i=0; i<ibuff.size(); i++) buff[i] = ibuff[i];
	// データ送信
	sendto(sock, buff, sendSize, 0, (struct sockaddr *)&addr, sizeof(addr));
	// カラー画像作成
	jpgimg = imdecode(Mat(ibuff), CV_LOAD_IMAGE_COLOR);
	// cout << " sendto " << endl;
      }
      // バッファクリア
      ibuff.clear();
    }

    /* video output */
    if(video_run) {
      // ビデオライタは生成済みか
      if(vw.isOpened()){
	// ビデオら板にキャプチャフレームの書きみ
	vw << frame;
	// cout << " video write " << endl;
      } else {
	cout << "Video Writer ERROR !!!" << endl;
	vw.release();
	video_run = false;
      }
    }

    /* calc fps  */
    endTime = cv::getTickCount();
    diffTime = (int)((startTime - endTime) * f);
    if (diffTime >= 1000) {
      startTime = endTime;
      fps = cnt - oldcnt;
      oldcnt = cnt;
      // cout << "fps:" << fps << endl;
    }

#ifdef DISPLAY
    // 文字列（FPS）
    std::ostringstream os;
    os << fps;
    std::string number = os.str();
    cv::putText(frame, number, point, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,200), 2, CV_AA);
    // ウインドウの更新
    cv::imshow("Send", frame);
    cv::waitKey(1);
    if(key == -1) {
      key = cv::waitKey(1);
      if(key > 1000000) {
	key -= 0x100000;
      }
      // cout << "key: " << key << endl;
    }
#endif

    switch(key) {
    case 113: /* q */
      end_flag = 1;
      break;

    case 112: /* p */
      // 現在時間からファイル名決定
      t = time(NULL);
      strftime(file_name, sizeof(file_name), "./data/jpg/%Y_%m_%d_%H_%M_%S.jpg", localtime(&t));
      imwrite(file_name, frame);
      printf("%s\n", file_name);
      break;

    case 118: /* v */
      if(video_run == false) {
	t = time(NULL);
	strftime(file_name, sizeof(file_name), "./data/avi/%Y_%m_%d_%H_%M_%S.avi", localtime(&t));
	/* ビデオライター設定 */
	vw.open(file_name, CV_FOURCC_DEFAULT, FPS, Size(640, 480), true);
	video_run = true;
	printf("Video START: %s\n", file_name);
      } else {
	/* cvReleaseVideoWriter(&vw); */
	printf("Video STOP: %s\n", file_name);
	// ビデオライタ解放
	vw.release();
	video_run = false;
      }
      break;

    default:
      break;
    }

    key = 0;

    cnt++;
    if(end_flag) break;

    /* 制御周期まで待機 */
    interT.tv_nsec += interval; /*  */
    // printf("interval:%d\n", interval);
    // printf("interT.tv_nsec:%ld\n", interT.tv_nsec);
    tsnorm(&interT);
    clock_nanosleep(0, TIMER_ABSTIME, &interT, NULL); /* 渡した時間まで待つ */
    clock_t end = clock();     // 終了時間
    // std::cout << "duration = " << (double)(end - start) / CLOCKS_PER_SEC << "sec.\n";
  } /* loop end */
  // ソケット終了
  close(sock);

  cap.release();
  if(video_run) {
    vw.release();
    video_run = false;
  }
  return 0;
}
