//
// Created by ziton on 2024/1/13.
//

#ifndef SEGMENTATION_AND_ENCODE_DIP_H
#define SEGMENTATION_AND_ENCODE_DIP_H

void read_image_raw(char * filename, int height, int width, int channel, unsigned char * img_buf);
void save_image_raw(char * filename, int height, int width, int channel, unsigned char * img_buf);

void rgb2gray(const unsigned char * rgb_img_buf, int height, int width, int channel,
              unsigned char * gray_img_buf);

void binary_threshold(unsigned char * gray_img_buf, int threshold, int height, int width);

void generate_Guassian_kernel(double* kernel, int radius, double sigma);
void Guassian_filter(const unsigned char* src, unsigned char* dst, int height, int width, double sigma);

int ostu(const unsigned char* gray_img, int height, int width);




#endif //SEGMENTATION_AND_ENCODE_DIP_H
