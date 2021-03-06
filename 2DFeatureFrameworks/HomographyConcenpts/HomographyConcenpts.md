# Homography的基本概念
[原文](https://docs.opencv.org/4.1.2/d9/dab/tutorial_homography.html)
## 什么是单应矩阵(homography matrix)？
简而言之，平面单应性涉及两个平面之间的变换（最大比例因子）：
$$
s \begin{bmatrix} x^{'} \\ y^{'} \\ 1 \end{bmatrix} = H \begin{bmatrix} x \\ y \\ 1 \end{bmatrix} = \begin{bmatrix} h_{11} & h_{12} & h_{13} \\ h_{21} & h_{22} & h_{23} \\ h_{31} & h_{32} & h_{33} \end{bmatrix} \begin{bmatrix} x \\ y \\ 1 \end{bmatrix}
$$

单应性矩阵是一个`3x3`矩阵，但只具有8DoF（自由度），因为它估计到一个比例。通常使用$$h_{33} = 1$$或$$h_{11}^2 + h_{12}^2 + h_{13}^2 + h_{21}^2 + h_{22}^2 + h_{23}^2 + h_{31}^2 + h_{32}^2 + h_{33}^2 = 1$$来对矩阵进行泛化。

以下示例显示了不同类型的变换，但都与两个平面之间的变换有关。

- 平面和像平面

    <div style="text-align: center">
        <img src=".HomographyConcenpts_images/48dc316e.png" width="85%" alt=""/>
        <h6>平面和像平面</h6>
    </div>

- 两个摄像机位置视角的平面

    <div style="text-align: center">
        <img src=".HomographyConcenpts_images/280b7c89.png" width="85%" alt=""/>
        <h6>两个摄像机位置视角的平面</h6>
    </div>

- 围绕其投影轴旋转的摄像头，相当于认为这些点位于无限远的平面上

    <div style="text-align: center">
        <img src=".HomographyConcenpts_images/4f787a19.png" width="85%" alt=""/>
        <h6>围绕其投影轴旋转的摄像头</h6>
    </div>

## 单应变换有什么应用？
- 从共面点进行相机姿态估计，例如使用标记增强现实

    <div style="text-align: center">
        <img src=".HomographyConcenpts_images/8b82d117.png" width="85%" alt=""/>
        <h6></h6>
    </div>

- 透视移除及矫正

    <div style="text-align: center">
        <img src=".HomographyConcenpts_images/c7fe074a.png" width="85%" alt=""/>
        <h6></h6>
    </div>

- 全景图拼接

    <div style="text-align: center">
        <img src=".HomographyConcenpts_images/823aa8d2.png" width="85%" alt=""/>
        <h6></h6>
    </div>

## 应用
### 1.使用共面点进行姿态估计
**注意:**
> 从单应性估计相机姿势的代码是一个示例，如果要估计平面或任意对象的相机姿势，则应改用[`cv::solvePnP`](https://docs.opencv.org/4.1.2/d9/d0c/group__calib3d.html#ga549c2075fac14829ff4a58bc931c033d)

可以使用DLT(Direct Linear Transform)算法估计homography变换。关于观察对象是一个平面，所以泛化的摄像机图像帧里面的对象帧上的点和像平面上的投影点是单应性的。**只有当对象是个平面的时候，假定知道了摄像头的内参，才能使用homography变换求解摄像头的姿态。**可以简单的使用棋盘格和[`findChessboardCorners()`](https://docs.opencv.org/4.1.2/d9/d0c/group__calib3d.html#ga93efa9b0aa890de240ca32b11253dd4a)方法获取图像上的角点坐标。

在使用棋盘格进行摄像头的位置姿态估计的时候，我们需要首先获取摄像头的内部参数和畸变参数，以及棋盘格的数据：

OpenCV演示例子提供其棋盘格图片以及棋盘格数据信息和拍摄的摄像头的数据信息:  

![chessboard](../../src/ProjectsCpp/2DFeatures/HomographyConcenpts/Resources/left04.jpg)

[import](../../src/ProjectsCpp/2DFeatures/HomographyConcenpts/Resources/left_intrinsics.yml)

#### 1.1 检测图片的棋盘格角点

```c++
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
```
**PS：OpenCV的`findChessboardCorners()`方法，如果图像中不存在棋盘格的话，十分的慢，但是存在的话几乎是毫秒级的响应**


![检测到的棋盘角点](.HomographyConcenpts_images/86ccf07b.png)

#### 1.2 计算对象的点
在知道棋盘格的尺寸之后，计算对象帧里面表述的对象的点事十分简单的：  

```c++
//! [计算棋盘格对象的点]
for (int i = 0; i < boardSize.height; ++i) {
    for (int j = 0; j < boardSize.width; ++j) {
         orners.push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
    }
}
//! [计算棋盘格对象的点]
```

在进行homography估计的时候一定要移除对象点的`z=0`坐标：  

```c++
    //! [计算对象点]
    vector<Point3f> objectPoints;
    calcChessboardCorners(patternSize, squareSize, objectPoints, CHESSBOARD);
    // 进行homography估计的话，必须移除z轴
    vector<Point2f> objectPointsPlanar;
    for (auto & objectPoint : objectPoints) {
        objectPointsPlanar.push_back(Point2f(objectPoint.x, objectPoint.y));
    }
    //! [计算对象点]
```

#### 1.3 计算泛化摄像机的像平面的角点
使用摄像头的内参和畸变参数，对棋盘角度使用透视变换的逆变换可以计算泛化摄像机的像平面的角点。 

```c++
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
```

#### 1.4 使用对象平面和泛化像平面的单应性关系计算homography变换矩阵

```c++
    //! [估算homography变换矩阵]
    Mat H = findHomography(objectPointsPlanar, imagePoints);
    cout << "H:\n" << H << endl;
    //! [估算homography变换矩阵]
```

#### 1.5 从homography矩阵中提取姿态信息

```c++
    //! [提取姿态矩阵]
    // 进行归一化操作确保||c1||=1
    double norm = sqrt(H.at<double>(0,0) * H.at<double>(0,0) +
                       H.at<double>(1,0) * H.at<double>(1,0) +
                       H.at<double>(2,0) * H.at<double>(2,0));
    H /= norm;
    Mat c1  = H.col(0);
    Mat c2  = H.col(1);
    Mat c3 = c1.cross(c2);
    Mat tvec = H.col(2);
    Mat R(3, 3, CV_64F);
    for (int i = 0; i < 3; i++)
    {
        R.at<double>(i,0) = c1.at<double>(i,0);
        R.at<double>(i,1) = c2.at<double>(i,0);
        R.at<double>(i,2) = c3.at<double>(i,0);
    }
    //! [提取姿态矩阵]
```

使用数学公式表示为：  
$$
    \mathbf{X} = \left( X, Y, 0, 1 \right ) 
$$

$$
    \begin{aligned}
        \mathbf{x} &= \mathbf{P}\mathbf{X} \\
        &= \mathbf{K} \begin{bmatrix} r_1 & r_2 & r_3 & t \end{bmatrix}  \begin{bmatrix} X \\ Y \\ 0 \\ 1 \end{bmatrix} \\
        &= \mathbf{K} \begin{bmatrix} r_1 & r_2 & t \end{bmatrix}  \begin{bmatrix} X \\ Y \\  1 \end{bmatrix} \\
        &= \mathbf{H}  \begin{bmatrix} X \\ Y \\  1 \end{bmatrix}
    \end{aligned}
$$

$$
    \begin{aligned}
        \mathbf{H} &= \lambda \mathbf{K} \begin{bmatrix} r_1 & r_2 & t \end{bmatrix} \\
        \mathbf{K^{-1}} \mathbf{H} &= \lambda  \begin{bmatrix} r_1 & r_2 & t \end{bmatrix} \\
        \mathbf{P} &= \mathbf{K} \begin{bmatrix} r_1 & r_2 & (r_1 \times r_2) & t \end{bmatrix}
    \end{aligned}
$$

**PS:这种简单的处理，并不能确保旋转矩阵是正交的，缩放比例也是非常之粗糙。而使用机型分解可以获得叫个较好的旋转矩阵**  

```c++
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
```

如此即可获取把对象投影到图像中的摄像机的姿态。

![camera pos](.HomographyConcenpts_images/4b6334bb.png)

### 基于共面角点的homography变换的姿态估计的完整代码

[import](../../src/ProjectsCpp/2DFeatures/HomographyConcenpts/2d_features_homography_pose_from_homography.cpp)

### 2. 透视矫正(Perspective correction)

<div style="text-align: center">
    <img src=".HomographyConcenpts_images/05404338.png" width="100%" alt=""/>
    <h6>将左图变换到右图</h6>
</div>  

#### 2.1 检测角点

```c++
    Mat img1 = imread("Resources/left01.jpg", IMREAD_GRAYSCALE);
    Mat img2 = imread("Resources/left02.jpg", IMREAD_GRAYSCALE);
    imshow("image1", img1);
    imshow("image2", img2);

    // 检测角点
    vector<Point2f> corners1, corners2;
    Size patternSize(9,6);
    bool found1 = findChessboardCorners(img1, patternSize, corners1);
    bool found2 = findChessboardCorners(img2, patternSize, corners2);
```
#### 2.2 估算homography

```c++
// 计算homography变换矩阵
    Mat H = findHomography(corners2, corners1);
```

#### 2.3 执行变换

```c++
    Mat img2_warp;
    warpPerspective(image2Color, img2_warp, H, image1Color.size());
    imshow("image2 warped", img2_warp);
```

### 3.使用相机位移计算Homography
Homography设计两个平面的变换，而且可以揭示摄像机从第一个平面视角到第一个平面视角的位移。

在CV中有了相机的内参和畸变参数，使用[`cv::solvePnP`](https://docs.opencv.org/4.1.2/d9/d0c/group__calib3d.html#ga549c2075fac14829ff4a58bc931c033d)可以计算出从3D对象点到图片上的2D投影点的相机的姿态位置。

$$
    \begin{aligned}
        s \begin{bmatrix} u \\ v \\ 1 \end{bmatrix}
        &= \begin{bmatrix} 
                f_x & 0 & c_x \\ 
                0 & f_y & c_y \\ 
                0 & 0 & 1
           \end{bmatrix}
           \begin{bmatrix} 
                r_{11} & r_{12} & r_{13} & t_x \\ 
                r_{21} & r_{22} & r_{23} & t_y \\ 
                r_{31} & r_{32} & r_{33} & t_z
           \end{bmatrix}
           \begin{bmatrix} X_o \\ Y_o \\ Z_o \\ 1 \end{bmatrix} \\
        &= \mathbf{K} {}^c\mathbf{M}_o \begin{bmatrix} X_o \\ Y_o \\ Z_o \\ 1 \end{bmatrix}
    \end{aligned}
$$

其中$$\mathbf{K}$$是相机的内参矩阵， $${}^c\mathbf{M}_o$$是相机的姿态，[`cv::solvePnP`](https://docs.opencv.org/4.1.2/d9/d0c/group__calib3d.html#ga549c2075fac14829ff4a58bc931c033d)输出的结果中`rvec`是Rodrigues旋转向量，`tvec`是移到向量。  

可以使用homography的形式表示$${}^c\mathbf{M}_o$$，$${}^c\mathbf{M}_o$$将对象上的点变换到摄像机换面上的映射点。  

$$
    \begin{aligned}
     \begin{bmatrix} X_c \\ Y_c \\ Z_c \\ 1 \end{bmatrix}
            &= {}^c\mathbf{M}_o \begin{bmatrix} X_o \\ Y_o \\ Z_o \\ 1 \end{bmatrix} \\
            &= \begin{bmatrix} 
                  {}^c R_o & {}^c t_o \\ 
                  0_{1 \times 3} & 1
               \end{bmatrix}
               \begin{bmatrix} X_o \\ Y_o \\ Z_o \\ 1 \end{bmatrix} \\
            &= \begin{bmatrix} 
                 r_{11} & r_{12} & r_{13} & t_x \\ 
                 r_{21} & r_{22} & r_{23} & t_y \\ 
                 r_{31} & r_{32} & r_{33} & t_z \\
                 0 & 0 & 0 & 1 
               \end{bmatrix}
               \begin{bmatrix} X_o \\ Y_o \\ Z_o \\ 1 \end{bmatrix}
     \end{aligned}
$$

如此从图像1到图像2上的点的变换可以通过矩阵相城简单完成：  
- $${}^{c_1}\mathbf{M}_o$$代表相机位1的姿态
- $${}^{c_2}\mathbf{M}_o$$代表相机位2的姿态

那么将相机位1的3D点变换到相机位2的变换可以表示为： 
$$
    ^{c_2}\textrm{M}_{c_1} =  ^{c_2}\textrm{M}_{o} \cdot  ^{o}\textrm{M}_{c_1} =  ^{c_2}\textrm{M}_{o} \cdot  \left( ^{c_1}\textrm{M}_{o} \right )^{-1} = \begin{bmatrix} ^{c_2}\textrm{R}_{o} & ^{c_2}\textrm{t}_{o} \\ 0_{3 \times 1} & 1 \end{bmatrix} \cdot \begin{bmatrix} ^{c_1}\textrm{R}_{o}^T & -  ^{c_1}\textrm{R}_{o}^T \cdot  ^{c_1}\textrm{t}_{o} \\ 0_{1 \times 3} & 1 \end{bmatrix}
$$

#### 3.1计算出相机姿态

```c++
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
    Mat rvec2, tvec2; // image2相机的旋转向量和平移向量
    solvePnP(objectPoints, corners2, cameraMatix, distCoeffs, rvec2, tvec2);

    // 绘制坐标轴
    Mat image1Color, image2Color;
    cvtColor(img1, image1Color, COLOR_GRAY2BGR);
    cvtColor(img2, image2Color, COLOR_GRAY2BGR);
    drawFrameAxes(image1Color, cameraMatix, distCoeffs, rvec1, tvec1, 2*squareSize);
    drawFrameAxes(image2Color, cameraMatix, distCoeffs, rvec2, tvec2, 2*squareSize);
    imshow("image1 axies", image1Color);
    imshow("image2 axies", image2Color);
```

![相机姿态](.HomographyConcenpts_images/23a9e22b.png)

#### 3.2根据上面的公式使用相机的姿态计算相机的位移
```c++
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
```
**从相机位移计算关于某个平面的homography**  

<div style="text-align: center">
    <img src=".HomographyConcenpts_images/6b32b107.png" width="100%" alt=""/>
    <h6>By Homography-transl.svg: Per Rosengren derivative work: Appoose (Homography-transl.svg) [CC BY 3.0 (http://creativecommons.org/licenses/by/3.0)], via Wikimedia Commons</h6>
</div>  

上图中向量`n`是屏幕的法向量，`d`表示相机沿法线到平面的距离，则使用相机位移计算homography的方程是: 
$$
    ^{2}\textrm{H}_{1} = ^{2}\textrm{R}_{1} - \frac{^{2}\textrm{t}_{1} \cdot n^T}{d}
$$

其中$$^{2}\textrm{H}_{1}$$将相位1拍摄的图像中的点映射到相位1拍摄的图像中对应的点的homography变换矩阵，$$^{2}\textrm{R}_{1} = ^{c_2}\textrm{R}_{o} \cdot ^{c_1}\textrm{R}_{o}^{T}$$代表两个相位的旋转变换向量，$$^{2}\textrm{t}_{1} = ^{c_2}\textrm{R}_{o} \cdot \left( - ^{c_1}\textrm{R}_{o}^{T} \cdot  ^{c_1}\textrm{t}_{o} \right ) +  ^{c_2}\textrm{t}_{o}$$表示两个相位的平移向量。 

通常法向量`n`可是使用平面的两个非平行的向量的叉积计算。在本例中我们假设平面是一个水平平面，现在又知道的frame1的相机旋转向量，所以可以直接计算：

```c++
 Mat normal = (Mat_<double>(3,1) << 0, 0, 1);
 Mat normal1 = R1*normal;
```

距离`d`可是使用法向量与一点到平面的一个向量的点积计算，或者可是直接使用平面方程的D系数:  

```c++
    Mat origin(3, 1, CV_64F, Scalar(0));
    Mat origin1 = R1*origin + tvec1;
    double d_inv1 = 1.0 / normal1.dot(origin1);
```

projective homography matrix $$\mathbf{G}$$ 可以使用 Euclidean homography $$\mathbf{H}$$ 结合 intrinsic matrix $$\mathbf{K}$$, 假设两个平面视图的摄像机是相同的:  
$$
\textbf{G} = \gamma \textbf{K} \textbf{H} \textbf{K}^{-1}
$$

```c++
Mat computeHomography(const Mat &R_1to2, const Mat &tvec_1to2, const double d_inv, const Mat &normal)
{
    Mat homography = R_1to2 + d_inv * tvec_1to2*normal.t();
    return homography;
}
```

**未完，使用相机位移计算Homography和使用findHomography都可以，并且很难区分不同，以后完善这一点**

### 4. homography矩阵分解
使用[`cv::decomposeHomographyMat`](https://docs.opencv.org/4.1.2/d9/d0c/group__calib3d.html#ga7f60bdff78833d1e3fd6d9d0fd538d92)方法可以将homography矩阵分解为旋转矩阵、平移矩阵和平面的法向量。  

对homography矩阵分解后获得了相机的位移， 如果知道相机的初始位置，就可以计算相机当前的位置，以及测试属于平面的3D对象点是否投影在相机的前面。

### 5.使用旋转摄像机进行基本的全景拼接
**注意:**
> 此示例用于说明基于相机纯旋转运动进行图像拼接的概念，并且不适合应用于拼接全景图像应用。cv的[拼接模块](https://docs.opencv.org/4.1.2/d1/d46/group__stitching.html)提供了完整的流水线来拼接图像。

单应变换仅适用于平面结构, 但是在旋转相机的情况下（围绕相机投影轴的纯旋转，没有平移），可以考虑是平面结构类型。


