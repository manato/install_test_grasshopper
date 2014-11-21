//
// opencv_viewer.cpp
//
// OpenCVを用いてカメラ画像を表示するプログラム
//
// ViewPLUS Inc.
//
/* ソース参考：
   http://www.viewplus.co.jp/tutorial/flycapture/cpp/opencv_viewer.cpp 
   /home/hirabayashi/install_grasshopper/install_test/flycapture/src/FlyCapture2Test_C/FlyCapture2Test_C.c
*/

#include <stdio.h>

#include <C/FlyCapture2_C.h>

// OpenCV関連のヘッダー
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>


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

int main(int argc, char* argv[])
{
  fc2Context flycapture;
  fc2Context flycapture2;
  fc2Error   error;
  fc2PGRGuid guid;              // カメラを一意に識別するために用いられるディスクリプタ
  fc2PGRGuid guid2;              // カメラを一意に識別するために用いられるディスクリプタ
  char window_name[] = "Camera image";
  char window_name2[] = "Camera image2";

  //
  // 初期化処理
  //
  cvNamedWindow( window_name, CV_WINDOW_AUTOSIZE); // OpenCVのウインドウを作成する。ウインドウへのアクセスにはwindow_nameで指定した名前を使います
  cvNamedWindow( window_name2, CV_WINDOW_AUTOSIZE); // OpenCVのウインドウを作成する。ウインドウへのアクセスにはwindow_nameで指定した名前を使います
  
  fc2Image  raw_image, bgr_image; // 画像保持用の構造体
  IplImage *cv_image, *vga_image;    // OpenCVの画像
  fc2Image  raw_image2, bgr_image2; // 画像保持用の構造体
  IplImage *cv_image2, *vga_image2;    // OpenCVの画像
  
  error = fc2CreateContext( &flycapture); // コンテキストの作成
  MY_FC2_CHECK(error, "fc2CreateContext()");
  error = fc2CreateContext( &flycapture2); // コンテキストの作成
  MY_FC2_CHECK(error, "fc2CreateContext()");
  
  unsigned int numCameras = 0;
  error = fc2GetNumOfCameras(flycapture, &numCameras);
  error = fc2GetNumOfCameras(flycapture2, &numCameras);
  MY_FC2_CHECK(error, "fc2GetNumOfCameras()"); 

  if (numCameras == 0) {        // No cameras detected
    fprintf(stderr, "No cameras detected.\n");
    exit(0);
  }
  
  // /* get the 0th camera */
  error = fc2GetCameraFromIndex(flycapture, 0, &guid);
  MY_FC2_CHECK(error, "fc2GetCameraFromIndex()");

  /* get the 1th camera */
  error = fc2GetCameraFromIndex(flycapture2, 1, &guid2);
  MY_FC2_CHECK(error, "fc2GetCameraFromIndex()");
  
  error = fc2Connect(flycapture, &guid);
  error = fc2Connect(flycapture2, &guid2);
  MY_FC2_CHECK(error, "fc2Connect()");

  PrintCameraInfo(flycapture);
  PrintCameraInfo(flycapture2);
 
   error = fc2StartCapture(flycapture); // カメラから画像の転送を開始する
  error = fc2StartCapture(flycapture2); // カメラから画像の転送を開始する
  MY_FC2_CHECK(error, "fc2StartCapture()");
  
  /* 画像格納用構造体の生成 */
  error = fc2CreateImage(&raw_image);
  error = fc2CreateImage(&raw_image2);
  MY_FC2_CHECK(error, "fc2CreateImage(raw_image)");

  error = fc2CreateImage(&bgr_image);
  error = fc2CreateImage(&bgr_image2);
  MY_FC2_CHECK(error, "fc2CreateImage(rgb_image)");
  bgr_image.format = FC2_PIXEL_FORMAT_BGR; // RGB画像のフォーマットを標準のBGR形式にセット
  bgr_image2.format = FC2_PIXEL_FORMAT_BGR; // RGB画像のフォーマットを標準のBGR形式にセット

  /* 画像を1回キャプチャ */
  error = fc2RetrieveBuffer(flycapture, &raw_image);
  error = fc2RetrieveBuffer(flycapture2, &raw_image2);
  MY_FC2_CHECK(error, "fc2RetieveBuffer() first");

  //  unsigned char *bgr_buffer = new unsigned char[ camera_image.rows * camera_image.cols * 3]; // キャプチャした画像のサイズと同じサイズのRGB画像を保持するバッファの確保
  
  cv_image = cvCreateImage( cvSize(raw_image.cols, raw_image.rows), IPL_DEPTH_8U, 3); // OpenCV画像の作成(8ビット3チャネル）
  cv_image2 = cvCreateImage( cvSize(raw_image2.cols, raw_image2.rows), IPL_DEPTH_8U, 3); // OpenCV画像の作成(8ビット3チャネル）
  vga_image = cvCreateImage( cvSize(640, 480), IPL_DEPTH_8U, 3); // VGAサイズの画像を作成
  vga_image2 = cvCreateImage( cvSize(640, 480), IPL_DEPTH_8U, 3); // VGAサイズの画像を作成
  
  //
  // カメラから画像を取得し、表示するループ
  //
  while (cvWaitKey(2) == -1) { // キー入力があるまで繰り返す
    
    /* 画像をカメラから取得 */
    error = fc2RetrieveBuffer(flycapture, &raw_image);
    error = fc2RetrieveBuffer(flycapture2, &raw_image2);
    MY_FC2_CHECK(error, "fc2RetrieveBuffer() in loop");
    
    // bgr_image.pData = bgr_buffer; // バッファを画像保持する構造体にセット
    // bgr_image.format = FC2_PIXEL_FORMAT_BGR; // RGB画像のフォーマットを標準のBGR形式にセット
      
    /* 生データをBGR形式の画像に変換 */
    error = fc2ConvertImageTo(FC2_PIXEL_FORMAT_BGR, &raw_image, &bgr_image);
    error = fc2ConvertImageTo(FC2_PIXEL_FORMAT_BGR, &raw_image2, &bgr_image2);
    MY_FC2_CHECK(error, "fc2ConvertImageTo()");

    memcpy(cv_image->imageData, bgr_image.pData, bgr_image.rows * bgr_image.stride); // OpenCVの画像バッファにRGBバッファをコピー
    memcpy(cv_image2->imageData, bgr_image2.pData, bgr_image2.rows * bgr_image2.stride); // OpenCVの画像バッファにRGBバッファをコピー
    
    
    /* resize image to VGA size */
    cvResize(cv_image, vga_image, CV_INTER_LINEAR);
    cvResize(cv_image2, vga_image2, CV_INTER_LINEAR);

    //    cvShowImage( window_name, cv_image); // ウインドウを描画
    cvShowImage( window_name, vga_image); // ウインドウを描画
    cvShowImage( window_name2, vga_image2); // ウインドウを描画
  }
  
  //
  // 終了処理
  //
  error = fc2StopCapture(flycapture); // カメラからの画像転送を停止
  error = fc2StopCapture(flycapture2); // カメラからの画像転送を停止
  MY_FC2_CHECK(error, "fc2StopCapture()");
  
  
  /* 画像バッファの解放 */
  error = fc2DestroyImage(&raw_image);
  error = fc2DestroyImage(&raw_image2);
  MY_FC2_CHECK(error, "fc2DestroyImage(raw_image)");
  
  error = fc2DestroyImage(&bgr_image);
  error = fc2DestroyImage(&bgr_image2);

  MY_FC2_CHECK(error, "fc2DestroyImage(bgr_image)");


  error = fc2DestroyContext( flycapture); // コンテキストを破棄
  error = fc2DestroyContext( flycapture2); // コンテキストを破棄
  MY_FC2_CHECK(error, "fc2DestroyContext()");
  
  cvReleaseImage( &cv_image); // OpenCV画像の解放
  cvReleaseImage( &vga_image); // OpenCV画像の解放
  cvDestroyWindow(window_name); // OpenCVウインドウの解放
  cvReleaseImage( &cv_image2); // OpenCV画像の解放
  cvReleaseImage( &vga_image2); // OpenCV画像の解放
  cvDestroyWindow(window_name2); // OpenCVウインドウの解放

  
  //  delete []bgr_buffer; // バッファの解放
  
  return 0;
}
