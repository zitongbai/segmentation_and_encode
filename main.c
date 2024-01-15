//
// Created by ziton on 2024/1/13.
//
#include "include/dip.h"
#include "include/huffman.h"
#include <stdlib.h>



int main() {

    int height = 560;
    int width = 752;
    int channel = 3;

    // 申请内存
    unsigned char * img_buf = (unsigned char *)malloc(height * width * channel * sizeof(unsigned char));
    unsigned char * gray_img_buf = (unsigned char *)malloc(height * width * sizeof(unsigned char));
    unsigned char * Guassian_img_buf = (unsigned char *)malloc(height * width * sizeof(unsigned char));
    unsigned char * edge_img_buf = (unsigned char *)malloc(height * width * sizeof(unsigned char));

    /*****************************************************************************
     * 图像处理部分
     *  本部分主要是从图像中提取感兴趣的区域，主要步骤如下：
     *      1. 读取图像
     *      2. 转换为灰度图
     *      3. 使用高斯核进行低通滤波
     *      4. 使用otsu方法计算全局最优阈值
     *      5. 二值化处理，将感兴趣的区域置为白色，背景置为黑色
     *
     ****************************************************************************/
    // 读取图像
    read_image_raw("figure//002.raw", height, width, channel, img_buf);
    // 转换为灰度图
    rgb2gray(img_buf, height, width, channel, gray_img_buf);
    // 使用高斯核进行低通滤波，高斯核的方差为2.8，该值约为图像短边长度的0.5%
    Guassian_filter(gray_img_buf, Guassian_img_buf, height, width, 2.8); // 高斯核的方差为2.8
    // 使用otsu方法计算全局最优阈值
    int threshold = ostu(Guassian_img_buf, height, width);
    // 二值化处理，将感兴趣的区域置为白色，背景置为黑色
    binary_threshold(gray_img_buf, threshold, height, width);
    save_image_raw("result//002_ostu.raw", height, width, 1, gray_img_buf);

    /*****************************************************************************
     * 哈夫曼编码与解码
     *  本部分主要是对上面得到的感兴趣的区域进行哈夫曼编码，主要步骤如下：
     *    1. 对图像进行Huffman编码
     *    2. 对图像进行Huffman解码
     *  具体的操作详见相关函数
     ****************************************************************************/
    encode_image("result//002.txt", img_buf, gray_img_buf, height, width, channel);
    unsigned char * decode_img_buf = decode_image("result//002.txt", &height, &width, &channel);
    save_image_raw("result//002_decode.raw", height, width, channel, decode_img_buf);


    free(img_buf);
    free(gray_img_buf);
    free(edge_img_buf);
    free(Guassian_img_buf);
//    free(decode_img_buf);

    return 0;
}
