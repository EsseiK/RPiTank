/*
  コンパイル用コマンド
  gcc -o camera_test test.c -I/usr/local/include/opencv -lpthread -ldl -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann -lm
 */

#include <stdio.h>
#include <time.h>
#include <cv.h>
#include <highgui.h>

int main (int argc, char **argv)
{
    double w = 640, h = 480;
    int key;
    time_t t;

    char file_name[128];
    IplImage *frame;
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

        key = cvWaitKey(1);
	/* リモートとローカルでキーコードが変わったので対応 */
	if(key > 1000000) {
	  key -= 0x100000;
	}
	/* printf("%d\n", key); */
	if(key==113){
	  break;
	} else if(key == 112){
	  t = time(NULL);
	  strftime(file_name, sizeof(file_name), "./png/%Y_%m_%d_%H_%M_%S.png", localtime(&t));
	  cvSaveImage(file_name, frame, 0); /* take pic */
	  printf("%s\n", file_name);
	}
    }
    cvReleaseCapture(&capture);
    cvDestroyWindow(windowName);
    return 0;
}
