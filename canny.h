#ifndef CANNY.H
#define CANNY.H
// 图像的宽和高
#define WIDTH 188
#define HEIGHT 120

// 高斯核大小
#define KERNEL_SIZE 5
#define THRESHOLD_LOW 50
#define THRESHOLD_HIGH 150

// 高斯滤波器（5x5）
float gaussianKernel[KERNEL_SIZE][KERNEL_SIZE];
float gaussianSum = 159.0;  // 高斯核的归一化常数
// 图像结构
unsigned char image[HEIGHT][WIDTH];         // 输入图像
unsigned char smoothed[HEIGHT][WIDTH];      // 高斯滤波后的图像
unsigned char gradientMag[HEIGHT][WIDTH];   // 梯度幅值
unsigned char gradientDir[HEIGHT][WIDTH];   // 梯度方向
unsigned char edges[HEIGHT][WIDTH];         // 最终的边缘图

#endif
