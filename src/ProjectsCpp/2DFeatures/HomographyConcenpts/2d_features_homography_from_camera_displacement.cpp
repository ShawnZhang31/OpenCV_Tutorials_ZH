//
// Created by 张晓民 on 2020/3/19.
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
 * 计算棋盘格对象的角点坐标
 * @param boardSize
 * @param squareSize
 * @param corners
 * @param patternType
 */
void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType = CHESSBOARD)
{
    corners.resize(0);

    switch (patternType)
    {
        case CHESSBOARD:
        case CIRCLES_GRID:
            //! [计算棋盘格对象的点]
            for (int i = 0; i < boardSize.height; ++i) {
                for (int j = 0; j < boardSize.width; ++j) {
                    corners.push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
                }
            }
            //! [计算棋盘格对象的点]
            break;
        case ASYMMETRIC_CIRCLES_GRID:
            for( int i = 0; i < boardSize.height; i++ )
                for( int j = 0; j < boardSize.width; j++ )
                    corners.push_back(Point3f(float((2*j + i % 2)*squareSize),
                                              float(i*squareSize), 0));
            break;
        default:
            CV_Error(Error::StsBadArg, "位置的标定类型!\n");
    }
}

/**
 * 计算从相机姿态2到姿态1的变换
 * @param R1
 * @param tvec1
 * @param R2
 * @param tvec2
 * @param R_1to2
 * @param tvec_1to2
 */
void computeC2MC1(const Mat &R1, const Mat &tvec1, const Mat &R2, const Mat &tvec2, Mat &R_1to2, Mat &tvec_1to2)
{
    R_1to2 = R2 * R1.t();
    tvec_1to2 = R2 * (-R1.t() * tvec1) + tvec2;
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

    // 棋盘格各自大小
    const float squareSize = 0.025f;
    vector<Point3f> objectPoints;
    calcChessboardCorners(patternSize, squareSize, objectPoints, CHESSBOARD);

    //! [加载摄像机参数]
    // 相机参数
    string intrinsicsPath = "Resources/left_intrinsics.yml";
    FileStorage fs(samples::findFile(intrinsicsPath), FileStorage::READ);
    Mat cameraMatix, distCoeffs;
    fs["camera_matrix"] >> cameraMatix;
    fs["distortion_coefficients"] >> distCoeffs;
    //! [加载摄像机参数]

    Mat rvec1, tvec1; // image1相机的旋转向量和平移向量
    solvePnP(objectPoints, corners1, cameraMatix, distCoeffs, rvec1, tvec1);
    cout << "rvec1:\n"<<rvec1<<endl;
    cout << "tvec1:\n"<<tvec1<<endl;
    Mat rvec2, tvec2; // image2相机的旋转向量和平移向量
    solvePnP(objectPoints, corners2, cameraMatix, distCoeffs, rvec2, tvec2);
    cout << "rvec2:\n"<<rvec2<<endl;
    cout << "tvec2:\n"<<tvec2<<endl;

    // 绘制坐标轴
    Mat image1Color, image2Color;
    cvtColor(img1, image1Color, COLOR_GRAY2BGR);
    cvtColor(img2, image2Color, COLOR_GRAY2BGR);
    drawFrameAxes(image1Color, cameraMatix, distCoeffs, rvec1, tvec1, 2*squareSize);
    drawFrameAxes(image2Color, cameraMatix, distCoeffs, rvec2, tvec2, 2*squareSize);
    imshow("image1 axies", image1Color);
    imshow("image2 axies", image2Color);



    waitKey();

    return EXIT_SUCCESS;
}