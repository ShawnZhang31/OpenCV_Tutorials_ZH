# Sticher类使用
OpenCV官方教程[原文](https://docs.opencv.org/4.1.2/d8/d19/tutorial_stitcher.html)  

## 内容
- 使用[`cv::Sticher`](https://docs.opencv.org/4.1.2/d2/d8d/classcv_1_1Stitcher.html)进行图像对齐
- 了解如何使用预配置的Stitcher配置以使用不同的相机型号来拼接图像

## 代码
[import](../../src/ProjectsCpp/ImageStiching/Sticher/im_stitching.cpp)

输入的图片为：  

<div style="text-align: center">
    <ul style="display: table">
        <li style="display: table-cell; list-style: none;">
            <img src=".Sticher_images/boats/boat1.jpg" width="99%">
            <p>boat1</p>
        </li>
        <li style="display: table-cell; list-style: none;">
            <img src=".Sticher_images/boats/boat2.jpg" width="99%">
            <p>boat2</p>
        </li>
        <li style="display: table-cell; list-style: none;">
            <img src=".Sticher_images/boats/boat3.jpg" width="99%">
            <p>boat3</p>
        </li>
    </ul>
    <ul style="display: table">
        <li style="display: table-cell; list-style: none;">
            <img src=".Sticher_images/boats/boat4.jpg" width="99%">
            <p>boat4</p>
        </li>
        <li style="display: table-cell; list-style: none;">
            <img src=".Sticher_images/boats/boat5.jpg" width="99%">
            <p>boat5</p>
        </li>
        <li style="display: table-cell; list-style: none;">
            <img src=".Sticher_images/boats/boat6.jpg" width="99%">
            <p>boat6</p>
        </li>
    </ul>
</div>  

输出图像为：

<div style="text-align: center">
    <img src=".Sticher_images/result.jpg" width="100%" alt=""/>
    <h6>拼接输出</h6>
</div>  



## 代码说明
最重要的代码部分是：  

```c++
    Mat pano;
    Ptr<Stitcher> stitcher = Stitcher::create(mode);
    Stitcher::Status status = stitcher->stitch(imgs, pano);

    if (status != Stitcher::OK){
        cout << "不能拼接图像，错误代码= " << int(status) << endl;
        return EXIT_FAILURE;
    }
```

Stitcher类实例的stitch方法会帮我们完成图像拼接的所有的计算和操作。  
[`cv::Stitcher::create`](https://docs.opencv.org/4.1.2/d2/d8d/classcv_1_1Stitcher.html#a308a47865a1f381e4429c8ec5e99549f)使用[`cv::Stitcher::Mode`](https://docs.opencv.org/4.1.2/d2/d8d/classcv_1_1Stitcher.html#a114713924ec05a0309f4df7e918c0324)参数创建一个示例。这些配置将设置多个Stitcher属性以在一种预定义的方案中运行. 在其中一种预定义配置中创建拼接器后，可以通过设置任何拼接器属性来调整拼接.  

如果你的OpenCV支持CUDA，则cv::Stitcher将在GPU上计算，速度将大大提高。

拼接有可能会失败，可以通过[`cv::Stitcher::Status`](https://docs.opencv.org/4.1.2/d2/d8d/classcv_1_1Stitcher.html#a507409ce9435dd89857469d12ec06b45)查看失败原件。

## Camera Models
OpenCV当前提供两个摄像机模型：
- Homography执行透视变换
    - 主要还是用于摄像机拍摄的图片
- Affine模型执行仿射变换
    - 主要用于扫描仪扫描的图片

**注意:**
> 虽然OpenCV的Stitcher类给出了许多详细的设置，但是实际开发中并没有太大的意义。只要牢记一天Homography模式和Affine模式不要混合使用，他们是不同的变换。

