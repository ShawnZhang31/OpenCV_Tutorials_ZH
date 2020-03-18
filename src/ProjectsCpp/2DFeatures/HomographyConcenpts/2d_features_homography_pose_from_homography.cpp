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
 * 计算棋盘格对象的角点坐标
 * @param boardSize
 * @param squareSize
 * @param corners
 * @param patternType
 */
void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType);

/**
 * 使用共面点估算摄像头的位置
 * @param img
 * @param intrinsicsPath
 * @param patternSize
 * @param squareSize
 */
void poseEstimationfromCoplannerPoints(Mat img, string &intrinsicsPath, const Size &patternSize, const float squareSize);


int main(int, char**)
{
    Mat imChessboard;

    // 棋盘格图片路径
    string chessBoardFilename = "Resources/left04.jpg";
    // 相机畸变参数
    string intrinsicsPath = "Resources/left_intrinsics.yml";
    // 棋盘格格数
    const Size patternSize(9,6);
    // 棋盘格各自大小
    const float squareSize = 0.025f;
    imChessboard = imread(chessBoardFilename, IMREAD_COLOR);

    // 使用棋盘格估计坐标
    poseEstimationfromCoplannerPoints(imChessboard, intrinsicsPath, patternSize, squareSize);

    waitKey();
    return EXIT_SUCCESS;
}

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
 * 使用共面点估算摄像头的位置
 * @param img
 * @param intrinsicsPath
 * @param patternSize
 * @param squareSize
 */
void poseEstimationfromCoplannerPoints(Mat img, string &intrinsicsPath, const Size &patternSize, const float squareSize)
{
    imshow("image original", img);
    // 用于显示棋盘格的角点
    Mat img_corners = img.clone();
    // 用于显示姿态
    Mat img_pose = img.clone();

    // region 检测角点
    Mat imgGray;
    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    // 棋盘格的角点
    vector<Point2f> corners;
    // 计算并绘制棋盘格的角点
    clock_t start = clock();
    bool found = findChessboardCorners(imgGray, patternSize, corners);
    double time = double(clock()-start)/CLOCKS_PER_SEC;
    cout << "检测角点耗时 : " << int(time*1000) << " 微秒"<< endl;

    if (!found){
        cout << "未能检测到棋盘格角点!" << endl;
        return;
    }
    drawChessboardCorners(img_corners, patternSize, corners, found);
    imshow("Chessboard Corners Detection", img_corners);
    //endregion

    //! [计算对象点]
    vector<Point3f> objectPoints;
    calcChessboardCorners(patternSize, squareSize, objectPoints, CHESSBOARD);
    // 进行homography估计的话，必须移除z轴
    vector<Point2f> objectPointsPlanar;
    for (auto & objectPoint : objectPoints) {
        objectPointsPlanar.push_back(Point2f(objectPoint.x, objectPoint.y));
    }
    //! [计算对象点]

    //! [加载摄像机参数]
    FileStorage fs(samples::findFile(intrinsicsPath), FileStorage::READ);
    Mat cameraMatix, distCoeffs;
    fs["camera_matrix"] >> cameraMatix;
    fs["distortion_coefficients"] >> distCoeffs;
    //! [加载摄像机参数]

    //! [计算理想成像平面的点]
    vector<Point2f> imagePoints;
    undistortPoints(corners, imagePoints, cameraMatix, distCoeffs);
    //! [计算理想成像平面的点]

    //! [估算homography变换矩阵]
    Mat H = findHomography(objectPointsPlanar, imagePoints);
    cout << "H:\n" << H << endl;
    //! [估算homography变换矩阵]

    //! [提取姿态矩阵]
    // 进行归一化操作确保||c1||=1
    double norm = sqrt(H.at<double>(0,0) * H.at<double>(0,0) +
                       H.at<double>(1,0) * H.at<double>(1,0) +
                       H.at<double>(2,0) * H.at<double>(2,0));
    H /= norm;
    Mat c1  = H.col(0);
    Mat c2  = H.col(1);
    Mat c3 = c1.cross(c2);

    // 平移向量
    Mat tvec = H.col(2);

    // 旋转矩阵
    Mat R(3, 3, CV_64F);
    for (int i = 0; i < 3; i++)
    {
        R.at<double>(i,0) = c1.at<double>(i,0);
        R.at<double>(i,1) = c2.at<double>(i,0);
        R.at<double>(i,2) = c3.at<double>(i,0);
    }
    //! [提取姿态矩阵]

    //! [旋转矩阵的极分解]
    cout << "R (极分解前):\n" << R << "\ndet(R): " << determinant(R) << endl;
    Mat W, U, Vt;
    SVDecomp(R, W, U, Vt);
    R = U*Vt;
    cout << "R (极分解后):\n" << R << "\ndet(R): " << determinant(R) << endl;
    //! [旋转矩阵的极分解]

    //! [显示姿态坐标轴]
    Mat rvec;
    Rodrigues(R, rvec);
    drawFrameAxes(img_pose, cameraMatix, distCoeffs, rvec, tvec, 2*squareSize);
    imshow("Pose from coplanar points", img_pose);

}