//
// Created by shawn on 2020/3/11.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main(int, char**)
{
    string filename = "imageTextN.png";
//    string filename = "imageTextR.png";
    Mat image = imread(filename, IMREAD_GRAYSCALE);
    if (image.empty()){
        cout << "文件加载失败！-"<< filename << endl;
        return EXIT_FAILURE;
    }

    Mat imageCopy = image.clone();


    // 将输入图像扩大到最佳尺寸
    Mat padded;
    int m = getOptimalDFTSize(image.rows);
    int n = getOptimalDFTSize(image.cols);
    copyMakeBorder(image, padded, 0, m-image.rows, 0, n-image.cols, BORDER_CONSTANT,
            Scalar::all(0));

    Mat planes[]={Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat complexImage;
    // 吧一个zeros通道和扩展通道合并在一起
    merge(planes, 2, complexImage);

    // dft是一个in-place操作
    dft(complexImage, complexImage);

    // 将DFT实数和虚数部分变换成幅值
    split(complexImage, planes);// planes[0]为DFT变换结果的实部，planes[1]为虚部
    magnitude(planes[0], planes[1], planes[0]);// 如此planes[0]存储的幅值大小

    Mat magImage = planes[0];

    // 将幅值转换到对数空间
    magImage += Scalar::all(1);
    log(magImage, magImage);

    // 如果图像的尺寸是奇数的话对图像进行裁剪并重新排列
    magImage = magImage(Rect(0, 0, magImage.cols & -2, magImage.rows & -2));

    // 重新排列Fourier图像的象限，使得图像的中心在象限的原点
    int cx = magImage.cols/2;
    int cy = magImage.rows/2;

    Mat q0(magImage, Rect(0,0,cx,cy));  // 左上
    Mat q1(magImage, Rect(cx, 0, cx, cy));  // 右上
    Mat q2(magImage, Rect(0, cy, cx, cy));  // 左下
    Mat q3(magImage, Rect(cx, cy, cx, cy));     // 右下

    Mat tmp;    //交换象限 (左上与右下交换)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    //交换象限 (右上与左下)
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);


    // 将幅度矩阵转化为0~1之间的浮点型数据方便显示
    normalize(magImage, magImage, 0, 1, NORM_MINMAX);

    imshow("input image", image);
    imshow("spectrum magnitude", magImage);
    waitKey();
    return EXIT_SUCCESS;
}
