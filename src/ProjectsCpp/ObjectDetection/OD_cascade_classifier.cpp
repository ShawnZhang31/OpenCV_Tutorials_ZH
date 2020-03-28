//
// Created by 张晓民 on 2020/3/27.
//
#include <iostream>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

/**
 * 识别并显示图像
 * @param frame 进行识别的图像帧
 */
void detectAnsDisplay(Mat frame)
{
  Mat frameGray;
  cvtColor(frame, frameGray, COLOR_BGR2GRAY);
  equalizeHist(frameGray, frameGray);

  // 检测面部
  vector<Rect> faces;
  clock_t startTime = clock();
  face_cascade.detectMultiScale(frameGray, faces);
  double timeCost = double(clock() - startTime)/CLOCKS_PER_SEC;
  cout << "face detect 耗时: " << timeCost << " 秒" << endl;
  for (const Rect face:faces) {
    Mat faceROI = frameGray(face);
    // 绘制脸部
    rectangle(frame, face, Scalar(255, 0, 0), 2);

    // 检测眼部
    vector<Rect> eyes;
    eyes_cascade.detectMultiScale(faceROI, eyes);
    for (const Rect eye:eyes) {
      // 绘制眼部
      Size radius(eye.width/2, eye.height/2);
      Point center(face.x+eye.x+eye.width/2, face.y+eye.y+eye.height/2);
      ellipse(frame, center, radius, 0, 0, 360, Scalar(0, 255, 0), 2);
    }
  }

  imshow("Capture-Face Detection", frame);
}

int main(int, char**)
{
  string face_cascade_name = "Resources/haarcascade_frontalface_default.xml";
  string eyes_cascade_name = "Resources/haarcascade_eye_tree_eyeglasses.xml";

  // 1. 加载级联
  if (!face_cascade.load(face_cascade_name))
  {
    cout << "--(!)加载面部识别模型失败! " << face_cascade_name << endl;
    return  EXIT_FAILURE;
  }
  if (!eyes_cascade.load(eyes_cascade_name))
  {
    cout << "--(!)加载眼睛识别模型失败! " << face_cascade_name << endl;
    return  EXIT_FAILURE;
  }

  int cameraDevice = 0;
  VideoCapture capture;
  // 2. 读取视频流
  capture.open(0);
  if (!capture.isOpened()){
    cout << "--(!)打开摄像头出错!" << endl;
    return EXIT_FAILURE;
  }
  int fps = capture.get(CAP_PROP_FPS);
  Mat frame;

  while(capture.read(frame)){
    if (frame.empty()){
      cout << "--(!) 获取不到图像 -- 退出循环!\n";
      break;
    }

    // 进行识别
    detectAnsDisplay(frame);

    if (waitKey(1000/fps) == 27){
      break;
    }
  }

  return EXIT_SUCCESS;
}
