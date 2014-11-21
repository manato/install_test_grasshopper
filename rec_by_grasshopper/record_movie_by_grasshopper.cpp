//
// record_movie_by_grasshopper.cpp
//
// OpenCVを用いてカメラ画像を取得し、動画として保存するプログラム
//
// ViewPLUS Inc.
//
/* ソース参考：
   http://opencv.jp/sample/video_io.html
   /home/hirabayashi/install_grasshopper/install_test/flycapture/src/FlyCapture2Test_C/FlyCapture2Test_C.c
*/

#include <stdio.h>

#include <C/FlyCapture2_C.h>

// OpenCV関連のヘッダー
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

/* header file to get date */
#include <time.h>

#define MY_FC2_CHECK(error, text) {                             \
    if ((error) != FC2_ERROR_OK) {                              \
      fprintf(stderr, "error detected at \"%s\"\n", (text));    \
      exit(0);                                                  \
    }                                                           \
  }

void PrintCameraInfo( fc2Context context )
{
    fc2Error error;
    fc2CameraInfo camInfo;
    error = fc2GetCameraInfo( context, &camInfo );
    MY_FC2_CHECK(error, "fc2GetCameraInfo()");

    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        camInfo.serialNumber,
        camInfo.modelName,
        camInfo.vendorName,
        camInfo.sensorInfo,
        camInfo.sensorResolution,
        camInfo.firmwareVersion,
        camInfo.firmwareBuildTime );
}

void makeFileName(char *filename)
{
  time_t timer;
  struct tm *local;

  /* get date */
  timer = time(NULL);

  /* convert local time */
  local = localtime(&timer);

  /* create file name(YYYY_MM_DD_hh_mm.avi) */
  sprintf(filename, "%4d_%02d_%02d_%02d_%02d.avi", local->tm_year+1900, local->tm_mon+1, local->tm_mday, local->tm_hour, local->tm_min);

  printf("video is recorded in \"%s\"\n", filename);
}

int main(int argc, char* argv[])
{
  fc2Context flycapture;
  fc2Error   error;
  fc2PGRGuid guid;              // カメラを一意に識別するために用いられるディスクリプタ
  char window_name[] = "Camera image";

  //
  // 初期化処理
  //
  cvNamedWindow( window_name, CV_WINDOW_AUTOSIZE); // OpenCVのウインドウを作成する。ウインドウへのアクセスにはwindow_nameで指定した名前を使います
  
  fc2Image  raw_image, bgr_image; // 画像保持用の構造体
  //  IplImage *cv_image, *vga_image;    // OpenCVの画像
  IplImage *cv_image;    // OpenCVの画像
  
  error = fc2CreateContext( &flycapture); // コンテキストの作成
  MY_FC2_CHECK(error, "fc2CreateContext()");
  
  unsigned int numCameras = 0;
  error = fc2GetNumOfCameras(flycapture, &numCameras);
  MY_FC2_CHECK(error, "fc2GetNumOfCameras()"); 

  if (numCameras == 0) {        // No cameras detected
    fprintf(stderr, "No cameras detected.\n");
    exit(0);
  }
  
  /* get the 0th camera */
  error = fc2GetCameraFromIndex(flycapture, 0, &guid);
  MY_FC2_CHECK(error, "fc2GetCameraFromIndex()");
  
  error = fc2Connect(flycapture, &guid);
  MY_FC2_CHECK(error, "fc2Connect()");

  PrintCameraInfo(flycapture);

  // configure to resize image by hardware
  // read sample or talk abraham to teach 

  
  error = fc2StartCapture(flycapture); // カメラから画像の転送を開始する
  MY_FC2_CHECK(error, "fc2StartCapture()");
  
  /* 画像格納用構造体の生成 */
  error = fc2CreateImage(&raw_image);
  MY_FC2_CHECK(error, "fc2CreateImage(raw_image)");

  error = fc2CreateImage(&bgr_image);
  MY_FC2_CHECK(error, "fc2CreateImage(rgb_image)");
  bgr_image.format = FC2_PIXEL_FORMAT_BGR; // RGB画像のフォーマットを標準のBGR形式にセット

  /* 画像を1回キャプチャ */
  error = fc2RetrieveBuffer(flycapture, &raw_image);
  MY_FC2_CHECK(error, "fc2RetieveBuffer() first");

  //  unsigned char *bgr_buffer = new unsigned char[ camera_image.rows * camera_image.cols * 3]; // キャプチャした画像のサイズと同じサイズのRGB画像を保持するバッファの確保
  
  cv_image = cvCreateImage( cvSize(raw_image.cols, raw_image.rows), IPL_DEPTH_8U, 3); // OpenCV画像の作成(8ビット3チャネル）
  //  vga_image = cvCreateImage( cvSize(640, 480), IPL_DEPTH_8U, 3); // VGAサイズの画像を作成
  
  /* descriptors to save captured image as movie */
  CvVideoWriter *vw;
  double w = raw_image.cols, h = raw_image.rows;

  /* create video writer structure */
  char filename[256] = {0};
  makeFileName(filename);
  //  int fps = 15;
    int fps = 20;
  vw = cvCreateVideoWriter(filename, CV_FOURCC('X', 'V', 'I', 'D'), fps, cvSize((int)w, (int)h));

  //
  // カメラから画像を取得し、表示するループ
  //
  while (cvWaitKey(2) == -1) { // キー入力があるまで繰り返す
    
    /* 画像をカメラから取得 */
    error = fc2RetrieveBuffer(flycapture, &raw_image);
    MY_FC2_CHECK(error, "fc2RetrieveBuffer() in loop");
    
    // bgr_image.pData = bgr_buffer; // バッファを画像保持する構造体にセット
    // bgr_image.format = FC2_PIXEL_FORMAT_BGR; // RGB画像のフォーマットを標準のBGR形式にセット
      
    /* 生データをBGR形式の画像に変換 */
    error = fc2ConvertImageTo(FC2_PIXEL_FORMAT_BGR, &raw_image, &bgr_image);
    MY_FC2_CHECK(error, "fc2ConvertImageTo()");

    memcpy(cv_image->imageData, bgr_image.pData, bgr_image.rows * bgr_image.stride); // OpenCVの画像バッファにRGBバッファをコピー
    
    
    /* write captured frame to video */
    cvWriteFrame(vw, cv_image);

    // /* resize image to VGA size */
    // cvResize(cv_image, vga_image, CV_INTER_LINEAR);

    cvShowImage( window_name, cv_image); // ウインドウを描画
    //    cvShowImage( window_name, vga_image); // ウインドウを描画
  }
  
  //
  // 終了処理
  //
  error = fc2StopCapture(flycapture); // カメラからの画像転送を停止
  MY_FC2_CHECK(error, "fc2StopCapture()");
  
  
  /* 画像バッファの解放 */
  error = fc2DestroyImage(&raw_image);
  MY_FC2_CHECK(error, "fc2DestroyImage(raw_image)");
  
  error = fc2DestroyImage(&bgr_image);
  MY_FC2_CHECK(error, "fc2DestroyImage(bgr_image)");


  error = fc2DestroyContext( flycapture); // コンテキストを破棄
  MY_FC2_CHECK(error, "fc2DestroyContext()");
  
  cvReleaseImage( &cv_image); // OpenCV画像の解放
  //  cvReleaseImage( &vga_image); // OpenCV画像の解放
  cvDestroyWindow(window_name); // OpenCVウインドウの解放
  
  //  delete []bgr_buffer; // バッファの解放
  
  /* stop writing and release structure */
  cvReleaseVideoWriter(&vw);

  return 0;
}
