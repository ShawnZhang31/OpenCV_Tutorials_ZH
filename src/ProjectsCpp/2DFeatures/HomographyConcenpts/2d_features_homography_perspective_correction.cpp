//
// Created by 张晓民 on 2020/3/18.
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

enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

/**
 * 生成随机颜色
 * @param rng 随机子
 * @return
 */
Scalar randomColor()
{
    return Scalar( rand()%255, rand()%255, rand()%255 );
}

int main(int, char**)
{
    Mat img1 = imread("Resources/left01.jpg", IMREAD_GRAYSCALE);
    Mat img2 = imread("Resources/left02.jpg", IMREAD_GRAYSCALE);
    imshow("image1", img1);
    imshow("image2", img2);

    // 检测角点
    vector<Point2f> corners1, corners2;
    Size patternSize(9,6);
    bool found1 = findChessboardCorners(img1, patternSize, corners1);
    bool found2 = findChessboardCorners(img2, patternSize, corners2);
    //绘制角点
    Mat image1Color, image2Color;
    cvtColor(img1, image1Color, COLOR_GRAY2BGR);
    cvtColor(img2, image2Color, COLOR_GRAY2BGR);
    drawChessboardCorners(image1Color, patternSize, corners1, found1);
    drawChessboardCorners(image2Color, patternSize, corners2, found2);
    imshow("image1 corners", image1Color);
    imshow("image2 corners", image2Color);

    // 绘制图像角点的匹配情况
    Mat img_draw_matches;
    cvtColor(img1, image1Color, COLOR_GRAY2BGR);
    cvtColor(img2, image2Color, COLOR_GRAY2BGR);
    hconcat(image1Color, image2Color, img_draw_matches);
    for (int i = 0; i < corners1.size(); ++i) {
        Point start = Point(corners1[i].x, corners1[i].y);
        Point end = Point(img1.size().width+corners2[i].x, corners2[i].y);
        line(img_draw_matches, start, end, randomColor(), 2);
    }
    imshow("Draw matches", img_draw_matches);

    // 计算homography变换矩阵
    Mat H = findHomography(corners2, corners1);
    cout << "H:\n" << H << endl;

    Mat img2_warp;
    warpPerspective(image2Color, img2_warp, H, image1Color.size());
    imshow("image2 warped", img2_warp);

    waitKey();
    return EXIT_SUCCESS;
}