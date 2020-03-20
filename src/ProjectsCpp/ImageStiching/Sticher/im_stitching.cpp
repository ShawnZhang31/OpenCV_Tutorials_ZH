//
// Created by 张晓民 on 2020/3/20.
//

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/stitching.hpp>

#include <iostream>

using namespace std;
using namespace cv;

bool divide_images = false;
Stitcher::Mode mode = Stitcher::PANORAMA;
vector<Mat> imgs;

string result_name = "result.jpg";

int main(int, char**)
{
    VideoCapture cap("boats/boat%1d.jpg");
    Mat frame;
    while (true){
        cap >> frame;
        if (frame.empty()){
            break;
        }
        // 为了防止图片不足，多切割点图片
        Rect roi(0,0, frame.cols/2, frame.rows);
        imgs.push_back(frame(roi).clone());
        roi.x = frame.cols / 3;
        imgs.push_back(frame(roi).clone());
        roi.x = frame.cols/2;
        imgs.push_back(frame(roi).clone());
    }
    cap.release();

    Mat pano;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(imgs, pano);

    if (status != Stitcher::OK){
        cout << "不能拼接图像，错误代码= " << int(status) << endl;
        return EXIT_FAILURE;
    }

    imwrite(result_name, pano);

    return EXIT_SUCCESS;
}