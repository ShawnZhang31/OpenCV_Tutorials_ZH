# 图像梯度(Image Gradients)
OpenCV提供三种图像的梯度计算(高通滤波 High-pass filters)：Sobel、Scharr和Laplacian。
## Sobel和Scharr求导
Sobel算子是一种图像模糊(Smoothing)和图像微分(Differentiation)的操作，因此噪声对Sobel的影响要小一点。使用Sobel算子可以通过分别指定yorder和xorder的参数来分别计算图像在x方向和y方向的导数。而且可以通过ksize参数指定kernel的尺寸。如果ksize=-1, 则OpenCV使用3x3尺寸的Scharr滤波，这比使用3x3尺寸的Sobel滤波要精确的多。
## Laplacian求导
Laplacian算子的数学表达为$$\Delta{src}=\frac{\delta^2{src}}{\delta{x^2}}+\frac{\delta^2{src}}{\delta{y^2}}$$,如果ksize=1，则laplacian算子的核可以表示为:  
$$
    Kernel = \begin{bmatrix}
            0 & 1 & 0 \\
            1 & -4 & 1 \\
            0 & 1 & 0
            \end{bmatrix}
$$

