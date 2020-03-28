//
// Created by 张晓民 on 2020/3/29.
//


#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
  string filename = "Resources/videos/slow_traffic_small.mp4";
  VideoCapture capture(filename);
  if (!capture.isOpened()){
    cout << "打开视频文件失败!- " << filename << endl;
    return EXIT_FAILURE;
  }

  // 创建随机颜色
  vector<Scalar> colors;
  RNG rng;
  for (int kI = 0; kI < 100; ++kI) {
    int r = rng.uniform(0, 256);
    int g = rng.uniform(0, 256);
    int b = rng.uniform(0, 256);
    colors.emplace_back(b, g, r);
  }

  Mat oldFrame, oldGray;
  vector<Point2f> p0, p1;

  // 获取第一帧图像并提取角点
  capture >> oldFrame;
  cvtColor(oldFrame, oldGray, COLOR_BGR2GRAY);
  goodFeaturesToTrack(oldGray, p0, 100, 0.3, 7, Mat(), 7, false, 0.04);

  // 创建一个mask用于绘制追踪
  Mat mask = Mat::zeros(oldFrame.size(), oldFrame.type());

  int fps = capture.get(CAP_PROP_FPS);

  while (true){
    Mat frame, frameGray;
    capture >> frame;
    if (frame.empty()){
      break;
    }

    cvtColor(frame, frameGray, COLOR_BGR2GRAY);

    // 计算光流
    vector<uchar> status;
    vector<float> err;
    // 最大迭代次数不超过10次，迭代精度不小于0.03
    TermCriteria criteria = TermCriteria((TermCriteria::COUNT)+(TermCriteria::EPS), 10, 0.03);
    calcOpticalFlowPyrLK(oldGray, frameGray, p0, p1, status, err, Size(15, 15), 2, criteria);

    vector<Point2f> goodNew;
    for (int kI = 0; kI < p0.size(); ++kI) {
      // 选择good points
      if (status[kI] == 1){
        goodNew.push_back(p1[kI]);
        // 绘制追踪线
        line(mask, p1[kI], p0[kI], colors[kI], 2);
        circle(frame, p1[kI], 5, colors[kI], -1);
      }
    }

    Mat img;
    add(frame, mask, img);

    imshow("Optical", img);

    if(waitKey(1000/fps) == 27){
      break;
    }

    oldGray = frameGray.clone();
    p0 = goodNew;
  }

  return  EXIT_SUCCESS;
}