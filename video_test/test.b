/*
  コンパイル用コマンド
  gcc -o video_test test.c -I/usr/local/include/opencv -lpthread -ldl -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lm
*/

#include <stdio.h>
#include <time.h>
#include <cv.h>
#include <highgui.h>

/* fpps合わせ */
#include <pthread.h>
#define NSEC_PER_SEC  1000000000 // = 1 [sec]
#define INTERVAL  10000000 // [ns]
#define DELTA_T 0.02

extern int clock_nanosleep(clockid_t __clock_id, int __flags,
                           __const struct timespec *__req,
                           struct timespec *__rem);
extern int clock_gettime(clockid_t clk_id, struct timespec *res);

int main (int argc, char **argv)
{
  struct timespec interT;
  int interval = DELTA_T * 1000000000; //[ns]
  clock_gettime(0, &interT);

  double w = 640, h = 360;
  // double w = 1280, h = 720;
  int key;
  int video_run;
  time_t t;
  double fps;
  char file_name[128];
  IplImage *frame;
  CvVideoWriter *vw; /* ビデオライター */
  CvCapture *capture = cvCreateCameraCapture(-1);
  if (capture==NULL) {
    puts("Camera not found.\n");
    return 0;
  }

  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, w);
  cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, h);


  // ウィンドウを作成する
  char *windowName = "camera";
  cvNamedWindow(windowName, CV_WINDOW_AUTOSIZE);

  while (1) {
    frame = cvQueryFrame(capture);
    cvShowImage(windowName, frame);
    //  fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    // printf("%lf\n", fps);
    // printf("%d\n", (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FPS));

    key = cvWaitKey(1);
    /* リモートとローカルでキーコードが変わったので対応 */
    if(key > 1000000) {
      key -= 0x100000;
    }
    /* printf("%d\n", key); */
    if(key==113){
      break;
    } else if(key == 118) {
      if(video_run == 0) {
	t = time(NULL);
	strftime(file_name, sizeof(file_name), "./avi/%Y_%m_%d_%H_%M_%S.avi", localtime(&t));
	/* ビデオライター設定 */
	//  vw = cvCreateVideoWriter(/*file_name*/"./avi/test.avi", CV_FOURCC ('X', 'V', 'I', 'D'), 30, cvSize ((int) w, (int) h), 1);
	vw = cvCreateVideoWriter(/*file_name*/"./avi/test.avi", CV_FOURCC ('M', 'J', 'P', 'G'), 5, cvSize ((int) w, (int) h), 1);
	printf("Video START: %s\n", file_name);
	video_run = 1;
      } else {
	cvReleaseVideoWriter(&vw);
	printf("Video STOP: %s\n", file_name);
	video_run = 0;
      }
    }

    if(video_run == 1) {
      /* fps分のフレームが取得できていないことがある */
      /* フレーム足りない->動画早い */
      cvWriteFrame(vw, frame);
    }
    /* 制御周期まで待機 */
    interT.tv_nsec += interval; /*  */
    /* printf("interval:%d\n", interval); */
    /* printf("interT.tv_nsec:%ld\n", interT.tv_nsec); */
    tsnorm(&interT);
    clock_nanosleep(0, TIMER_ABSTIME, &interT, NULL); /* 渡した時間まで待つ */

  }
  /* 解放 */
  if(video_run == 1) {
    cvReleaseVideoWriter(&vw);
    video_run = 0;
  }
  cvReleaseCapture(&capture);
  cvDestroyWindow(windowName);
  return 0;
}
