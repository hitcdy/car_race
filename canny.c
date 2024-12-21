#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <canny.h>

// 高斯滤波器（5x5）
float gaussianKernel[KERNEL_SIZE][KERNEL_SIZE] = {
    {2, 4, 5, 4, 2},
    {4, 9, 12, 9, 4},
    {5, 12, 15, 12, 5},
    {4, 9, 12, 9, 4},
    {2, 4, 5, 4, 2}
};

// 高斯滤波
void gaussianBlur(unsigned char input[HEIGHT][WIDTH], unsigned char output[HEIGHT][WIDTH]) {
    int offset = KERNEL_SIZE / 2;
    
    for (int i = offset; i < HEIGHT - offset; i++) {
        for (int j = offset; j < WIDTH - offset; j++) {
            float sum = 0.0;
            for (int ki = -offset; ki <= offset; ki++) {
                for (int kj = -offset; kj <= offset; kj++) {
                    sum += input[i + ki][j + kj] * gaussianKernel[ki + offset][kj + offset];
                }
            }
            output[i][j] = (unsigned char)(sum / gaussianSum);
        }
    }
}

// Sobel 算子计算梯度
void sobelOperator(unsigned char input[HEIGHT][WIDTH], unsigned char gradMag[HEIGHT][WIDTH], unsigned char gradDir[HEIGHT][WIDTH]) {
    int Gx, Gy;
    int offset = 1;
    
    // Sobel算子
    int GxKernel[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int GyKernel[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    for (int i = 1; i < HEIGHT - 1; i++) {
        for (int j = 1; j < WIDTH - 1; j++) {
            Gx = 0;
            Gy = 0;
            // 计算Gx和Gy
            for (int ki = -offset; ki <= offset; ki++) {
                for (int kj = -offset; kj <= offset; kj++) {
                    Gx += input[i + ki][j + kj] * GxKernel[ki + offset][kj + offset];
                    Gy += input[i + ki][j + kj] * GyKernel[ki + offset][kj + offset];
                }
            }
            gradMag[i][j] = (unsigned char)sqrt(Gx * Gx + Gy * Gy);  // 梯度幅值
            gradDir[i][j] = (unsigned char)(atan2(Gy, Gx) * 180 / M_PI);  // 梯度方向
        }
    }
}

// 非最大值抑制
void nonMaxSuppression(unsigned char gradMag[HEIGHT][WIDTH], unsigned char gradDir[HEIGHT][WIDTH], unsigned char output[HEIGHT][WIDTH]) {
    int angle;
    for (int i = 1; i < HEIGHT - 1; i++) {
        for (int j = 1; j < WIDTH - 1; j++) {
            angle = gradDir[i][j] < 0 ? gradDir[i][j] + 180 : gradDir[i][j];  // 确保角度为正
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle < 180)) {  // 水平边缘
                if (gradMag[i][j] < gradMag[i][j-1] || gradMag[i][j] < gradMag[i][j+1]) {
                    output[i][j] = 0;
                } else {
                    output[i][j] = gradMag[i][j];
                }
            } else if (angle >= 22.5 && angle < 67.5) {  // 45度边缘
                if (gradMag[i][j] < gradMag[i-1][j+1] || gradMag[i][j] < gradMag[i+1][j-1]) {
                    output[i][j] = 0;
                } else {
                    output[i][j] = gradMag[i][j];
                }
            } else if (angle >= 67.5 && angle < 112.5) {  // 垂直边缘
                if (gradMag[i][j] < gradMag[i-1][j] || gradMag[i][j] < gradMag[i+1][j]) {
                    output[i][j] = 0;
                } else {
                    output[i][j] = gradMag[i][j];
                }
            } else {  // 135度边缘
                if (gradMag[i][j] < gradMag[i-1][j-1] || gradMag[i][j] < gradMag[i+1][j+1]) {
                    output[i][j] = 0;
                } else {
                    output[i][j] = gradMag[i][j];
                }
            }
        }
    }
}

// 双阈值处理
void doubleThreshold(unsigned char input[HEIGHT][WIDTH], unsigned char output[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (input[i][j] >= THRESHOLD_HIGH) {
                output[i][j] = 255;  // 强边缘
            } else if (input[i][j] >= THRESHOLD_LOW) {
                output[i][j] = 127;  // 弱边缘
            } else {
                output[i][j] = 0;    // 非边缘
            }
        }
    }
}

// 边缘连接
void edgeTrackingByHysteresis(unsigned char input[HEIGHT][WIDTH], unsigned char output[HEIGHT][WIDTH]) {
    for (int i = 1; i < HEIGHT - 1; i++) {
        for (int j = 1; j < WIDTH - 1; j++) {
            if (input[i][j] == 127) {
                if (input[i-1][j-1] == 255 || input[i-1][j] == 255 || input[i-1][j+1] == 255 ||
                    input[i][j-1] == 255 || input[i][j+1] == 255 ||
                    input[i+1][j-1] == 255 || input[i+1][j] == 255 || input[i+1][j+1] == 255) {
                    output[i][j] = 255;  // 连接到强边缘
                } else {
                    output[i][j] = 0;    // 去除弱边缘
                }
            }
        }
    }
}

// Canny 边缘检测主函数
void cannyEdgeDetection(unsigned char input[HEIGHT][WIDTH], unsigned char output[HEIGHT][WIDTH]) {
    // 1. 高斯滤波
    gaussianBlur(input, smoothed);
    
    // 2. 计算梯度
    sobelOperator(smoothed, gradientMag, gradientDir);
    
    // 3. 非最大值抑制
    nonMaxSuppression(gradientMag, gradientDir, edges);
    
    // 4. 双阈值处理
    doubleThreshold(edges, edges);
    
    // 5. 边缘连接
    edgeTrackingByHysteresis(edges, output);
}
