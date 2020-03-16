//
// Created by 张晓民 on 2020/3/16.
//

#include <iostream>
#include <string>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const char* source_window = "Source Image";
const char* corner_windows = "Corner Detected";
Mat src, src_gray;
int thresh=200, maxThresh=255;

/**
 *
 */
void cornerHarrisDemo(int, void*);

int main(int, char**)
{
    string filename = "Resources/harris.jpg";
    src = imread(filename);

    // 将图像转换为灰度图像
    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    namedWindow(source_window, WINDOW_AUTOSIZE);
    createTrackbar("Threshold", source_window, &thresh, maxThresh, cornerHarrisDemo);
    cornerHarrisDemo(0,0);

    imshow(source_window, src);

    waitKey();
    destroyAllWindows();

    return EXIT_SUCCESS;
}

/**
 * 调整阈值
 */
void cornerHarrisDemo(int, void*)
{
    cout << "thresh: " << thresh << endl;
    int blockSize = 2;
    int apertureSize = 3;
    double k =0.04;

    Mat dst = Mat::zeros(src.size(), CV_32FC1);

    cornerHarris(src_gray, dst, blockSize, apertureSize, k);

    Mat dst_norm, dst_norm_scaled;
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( dst_norm, dst_norm_scaled );
    for( int i = 0; i < dst_norm.rows ; i++ )
    {
        for( int j = 0; j < dst_norm.cols; j++ )
        {
            if( (int) dst_norm.at<float>(i,j) > thresh )
            {
                circle( dst_norm_scaled, Point(j,i), 5,  Scalar(0), 2, 8, 0 );
            }
        }
    }
    namedWindow( corner_windows);
    imshow( corner_windows, dst_norm_scaled );
}