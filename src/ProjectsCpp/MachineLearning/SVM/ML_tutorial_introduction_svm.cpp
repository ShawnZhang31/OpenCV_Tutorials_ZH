//
// Created by 张晓民 on 2020/3/23.
//

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include <iostream>

using namespace cv;
using namespace cv::ml;
using namespace std;

int main(int, char**)
{
  // 设置训练数据
  int labels[4] = {1, -1, -1, -1};
  float trainingData[4][2] = {{501,10}, {255, 10}, {501, 255}, {10, 501}};
  Mat trainingDataMat(4, 2, CV_32F, trainingData);
  Mat labelsMat(4, 1, CV_32SC1, labels);

  //训练SVM
  Ptr<SVM> svm = SVM::create();
  svm->setType(SVM::C_SVC);
  svm->setKernel(SVM::LINEAR);
  svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
  svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);

  //数据可视化
  int width = 512, height = 512;
  Mat image = Mat::zeros(height, width, CV_8UC3);

  // 显示SVM给出的确定区域
  Vec3b green(0, 255, 0), blue(255, 0, 0);
  for (int i = 0; i < image.rows; i++)
  {
    for (int j = 0; j < image.cols; j++)
    {
      Mat sampleMat = (Mat_<float>(1,2) << j,i);
      float response = svm->predict(sampleMat);
      if (response == 1)
        image.at<Vec3b>(i,j)  = green;
      else if (response == -1)
        image.at<Vec3b>(i,j)  = blue;
    }
  }

  // 显示训练数据
  int thickness = -1;
  circle( image, Point(501,  10), 5, Scalar(  0,   0,   0), thickness );
  circle( image, Point(255,  10), 5, Scalar(255, 255, 255), thickness );
  circle( image, Point(501, 255), 5, Scalar(255, 255, 255), thickness );
  circle( image, Point( 10, 501), 5, Scalar(255, 255, 255), thickness );

  // 显示支持向量
  thickness = 2;
  Mat sv = svm->getUncompressedSupportVectors();
  cout << "support vectors: " << endl << sv << endl;
  for (int i = 0; i < sv.rows; i++)
  {
    const float* v = sv.ptr<float>(i);
    circle(image,  Point( (int) v[0], (int) v[1]), 6, Scalar(128, 128, 128), thickness);
  }
  imwrite("result.png", image);        // save the image
  imshow("SVM Simple Example", image); // show it to the user
  waitKey();
  return EXIT_SUCCESS;
}

