//
// Created by ziton on 2024/1/13.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/dip.h"

#define PI 3.14159265358979323846


/**
 * @brief 读取raw图像
 *      raw图像相隔存储通道，共3个通道，即RGBRGB...，每个通道的数据为8bit
 * @param filename
 * @param height
 * @param width
 * @param channel
 * @param data
 * @return
 */
void read_image_raw(char * filename, int height, int width, int channel, unsigned char * img_buf) {
    FILE * fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("[ERROR] Cannot open file %s\n", filename);
        exit(0);
    }
    fread(img_buf, sizeof(unsigned char), height * width * channel, fp);
    fclose(fp);
}

/**
 * @brief 保存raw图像
 * @param filename
 * @param height
 * @param width
 * @param channel
 * @param data
 * @return
 */
void save_image_raw(char * filename, int height, int width, int channel, unsigned char * img_buf) {
    FILE * fp = fopen(filename, "wb");
    fwrite(img_buf, sizeof(unsigned char), height * width * channel, fp);
    fclose(fp);
}

/**
 * @brief 将图像转换为灰度图
 *      raw图像相隔存储通道，共3个通道，即RGBRGB...，每个通道的数据为8bit
 *      我们取各个通道的平均值作为灰度值
 * @return
 */
void rgb2gray(const unsigned char * rgb_img_buf, int height, int width, int channel,
              unsigned char * gray_img_buf) {
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            gray_img_buf[i*width+j] = (unsigned char)((rgb_img_buf[i*width*channel+j*channel] +
                                                       rgb_img_buf[i*width*channel+j*channel+1] + rgb_img_buf[i*width*channel+j*channel+2])/3.0);
        }
    }
}


/**
 * @breif 对灰度图像进行二值化处理
 *      将感兴趣的区域置为白色，背景置为黑色
 * @param gray_img_buf 灰度图像
 * @param threshold 阈值
 * @param height 图像的高度
 * @param width 图像的宽度
 */
void binary_threshold(unsigned char * gray_img_buf, int threshold, int height, int width){
    for (int i = 0; i < height * width; i++) {
        gray_img_buf[i] = gray_img_buf[i] < threshold ? 255 : 0;
    }
}

/**
 * @brief 生成高斯核
 * @param kernel 用于存放生成的高斯核
 * @param radius 核半径
 * @param sigma 高斯分布的标准差
 */
void generate_Guassian_kernel(double* kernel, int radius, double sigma) {
    double sum = 0.0;
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            int index = (y + radius) * (radius * 2 + 1) + (x + radius);
            kernel[index] = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * PI * sigma * sigma);
            sum += kernel[index];
        }
    }

    // 归一化核
    for (int i = 0; i < (radius * 2 + 1) * (radius * 2 + 1); i++) {
        kernel[i] /= sum;
    }
}

/**
 * @brief 使用高斯核进行低通滤波
 * @param src 滤波前图像
 * @param dst 滤波后图像
 * @param width 图像的宽度
 * @param height 图像的高度
 * @param sigma 高斯分布的标准差
 */
void Guassian_filter(const unsigned char* src, unsigned char* dst, int height, int width, double sigma) {
    int radius = (int)(sigma * 3); // 核半径
    double* kernel = (double*)malloc((radius * 2 + 1) * (radius * 2 + 1) * sizeof(double));
    generate_Guassian_kernel(kernel, radius, sigma);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sum = 0.0;
            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    int ix = x + kx;
                    int iy = y + ky;
                    // 边界检查
                    ix = ix < 0 ? 0 : ix;
                    ix = ix >= width ? width - 1 : ix;
                    iy = iy < 0 ? 0 : iy;
                    iy = iy >= height ? height - 1 : iy;
                    // 卷积
                    sum += src[iy * width + ix] * kernel[(ky + radius) * (radius * 2 + 1) + (kx + radius)];
                }
            }
            dst[y * width + x] = (unsigned char)sum; // 将卷积结果写入目标图像
        }
    }
    free(kernel);
}

/**
 * @brief 使用Otsu方法的全局最优阈值处理
 * @param gray_img 灰度图像
 * @param width 图像的宽度
 * @param height 图像的高度
 * @return
 */
int ostu(const unsigned char* gray_img, int height, int width) {
    // 计算直方图
    int histogram[256] = {0};
    for (int i = 0; i < width * height; ++i) {
        histogram[gray_img[i]]++;
    }

    // 计算总像素数和总灰度和
    int total = width * height;
    int sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += i * histogram[i];
    }

    int sumB = 0, wB = 0, wF = 0;
    float maxVar = 0.0;
    int threshold = 0;

    // 遍历所有可能的阈值，寻找最大类间方差
    for (int i = 0; i < 256; ++i) {
        wB += histogram[i];               // 背景部分的像素数
        if (wB == 0) continue;
        wF = total - wB;                  // 前景部分的像素数
        if (wF == 0) break;

        sumB += i * histogram[i];
        float meanB = (float)sumB / wB;    // 背景平均灰度
        float meanF = (float)(sum - sumB) / wF; // 前景平均灰度

        // 计算类间方差
        float varBetween = (float)wB * (float)wF * (meanB - meanF) * (meanB - meanF);

        // 更新最大类间方差及对应的阈值
        if (varBetween > maxVar) {
            maxVar = varBetween;
            threshold = i;
        }
    }

    return threshold;
}