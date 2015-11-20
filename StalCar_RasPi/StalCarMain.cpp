#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <string>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "RaspiCamCV.h"
#include "SerialManager.h"

#define CASCADE_FILE "./haarcascade_frontalface_alt.xml"
//#define CASCADE_FILE "./haarcascade_mcs_upperbody.xml"

/* 定数の設定 */
#define DOWNSCALE (4)	//縮小率(標準4)
#define MIN_OBJECT_WIDTH (48) //標準48
#define MIN_OBJECT_HEIGHT (48)//標準48
#define WIDTH 640
#define HEIGHT 480

#define THRESHOLD 120
#define PRE_THRESHOLD 150

#define LEFT 0
#define RIGHT 1

/* 表示モード */
//#define WINDOW_MODE

int detect(IplImage*,IplImage*,int,int,int,CvPoint*);
void sendCommand(int,std::string,int);

/* スタティック変数 */
static CvHaarClassifierCascade *cascade;
static CvMemStorage *storage;
static SerialManager *serial;
static int left_speed = 0,right_speed = 0;

/* メイン */
int main(int argc,char **argv){
  IplImage *image = NULL,*dst = NULL;
  RaspiCamCvCapture *cap = NULL;
  CvPoint point;
 
	
  if((cap = raspiCamCvCreateCameraCapture(0)) == 0){
    printf("no camera\n");
    return -1;
  }
  /* 顔検出用ファイル読み込み */
  cascade=(CvHaarClassifierCascade *)cvLoad(CASCADE_FILE,0,0,0);
  if(cascade==NULL){
    printf("no cascade\n");
    return -1;
  }

  try{
    serial = new SerialManager("/dev/ttyUSB0");
  }catch(const char* exception){
    printf("%s\n",exception);
    return -1;
  }
  
  storage=cvCreateMemStorage(0);
  
  raspiCamCvSetCaptureProperty(cap,CV_CAP_PROP_FRAME_WIDTH,WIDTH);
  raspiCamCvSetCaptureProperty(cap,CV_CAP_PROP_FRAME_HEIGHT,HEIGHT);
  
  /* ウィンドウの作成 */
#ifdef WINDOW_MODE
  cvNamedWindow("sample", CV_WINDOW_AUTOSIZE);
#endif

  
	sendCommand(LEFT,"stop",0);
	sendCommand(RIGHT,"stop",0);
  printf("start\n");

  /* メインループ */
  while(1){
    /* 画像の表示 */
    image = raspiCamCvQueryFrame(cap);
    if(image == NULL)continue;
    //printf("cap\n");
    dst = cvCloneImage(image);
    int size = detect(image,dst,DOWNSCALE,48,48,&point);
    //int size =0;

    //printf("show\n");
#ifdef WINDOW_MODE
    cvShowImage("sample", dst);
#endif
    //printf("x:%d,y:%d\n",point.x,point.y);
    //printf("minus:%d,plus:%d",image->width/2 - THRESHOLD,image->width/2 + THRESHOLD);
    //printf("serial\n");
    /* シリアル通信 */
    std::string param;
    if(size >0){
      std::cout << "x:" << point.x << ",y:" << point.y << std::endl;
      if(point.x < image->width/2 - THRESHOLD){
      	/*
	left_speed =0;
	right_speed = 130;
	serial->writeLine(param = "right,forward,130\n");
	serial->writeLine(param = "left,brake,0\n");
      	*/
      	sendCommand(LEFT,"stop",0);
      	sendCommand(RIGHT,"forward",80);
	usleep(10000);
      }else if(point.x > image->width/2 + THRESHOLD){
      	sendCommand(LEFT,"forward",80);
      	sendCommand(RIGHT,"stop",0);
	usleep(10000);
      }else if(size>WIDTH*0.6){
      	sendCommand(LEFT,"stop",0);
      	sendCommand(RIGHT,"stop",0);
	usleep(10000);
      }else{
      	sendCommand(LEFT,"forward",150);
      	sendCommand(RIGHT,"forward",150);
	usleep(10000);
      }
    }else{
      	sendCommand(LEFT,"stop",0);
      	sendCommand(RIGHT,"stop",0);
      usleep(10000);
    }
    cvReleaseImage(&dst);
    if(cvWaitKey(10) == 'q')break;
  }
#ifdef WINDOW_MODE
  cvDestroyWindow("sample");
#endif

  raspiCamCvReleaseCapture(&cap);
  
  return 0;
}

void sendCommand(int muki,std::string drive, int speed)
{
	std::string param;
	std::ostringstream oss;
	if(muki == LEFT){
		if(speed == left_speed)return;
		left_speed = speed;
		//param = "right," + drive + "," + "\n";
		oss << "left," << drive << "," << speed << "\n";
		param = oss.str();
		serial->writeLine(param);
	}
	else{
		if(speed == right_speed)return;
		right_speed = speed;
		oss << "right," << drive << "," << speed << "\n";
		param = oss.str();
		serial->writeLine(param);
	}
}

int detect(IplImage *currentImage,IplImage *resultImage,int scale,int min_width,int min_height,CvPoint *point)
{
	static CvScalar color ={255,255,255};
	int i;
	CvPoint pt1,pt2;

	/* 顔検出関数用に画像を変換 */
	IplImage *gray=cvCreateImage(cvSize(currentImage->width,currentImage->height),8,1); //白黒画像
	IplImage *small_img=cvCreateImage(cvSize(cvRound(currentImage->width/scale),cvRound(currentImage->height/scale)),8,1); //小さいサイズ

	cvClearMemStorage(storage);

	cvCvtColor(currentImage,gray,CV_BGR2GRAY);
	cvResize(gray,small_img,CV_INTER_LINEAR);
	cvEqualizeHist(small_img,small_img);

	/* 検出処理 */
	CvSeq *faces=cvHaarDetectObjects(small_img,cascade,storage,1.3,2,CV_HAAR_FIND_BIGGEST_OBJECT + CV_HAAR_DO_ROUGH_SEARCH,cvSize(cvRound(min_width/scale),cvRound(min_height/scale)));

	cvCopy(currentImage,resultImage,NULL);

	cvReleaseImage(&gray);
	cvReleaseImage(&small_img);

	if(faces->total == 0)return 0;

	printf("faces\n");
	CvRect *r=(CvRect*)cvGetSeqElem(faces,0);
	pt1.x = r->x*scale;
	pt1.y = r->y*scale;
	pt2.x = (r->x+r->width)*scale;
	pt2.y = (r->y+r->height)*scale;
	cvRectangle(resultImage,pt1,pt2,CV_RGB(0,255,0));
	
	point->x = (r->x + r->width/2.0)*scale;
	point->y = (r->y + r->height/2.0)*scale;
	printf("detect\n");
	return r->width * scale;
}
