//
// Created by ziton on 2024/1/13.
//

#include "../include/huffman.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * 计算RGB值的频率
 * @return
 */
void calculate_freq(const unsigned char * rgb_img_buf, const unsigned char * binary_img_buf,
                    int * freq, int height, int width, int channel) {
    // 初始化频率数组
    for (int i = 0; i < 256; i++) {
        freq[i] = 0;
    }
    // 计算频数
    for (int i = 0; i < height * width; i++) {
        if (binary_img_buf[i] == 255) {
            freq[rgb_img_buf[i * channel]]++;
            freq[rgb_img_buf[i * channel + 1]]++;
            freq[rgb_img_buf[i * channel + 2]]++;
        }
    }
}

void stack_init(Stack *s) {
    s->index = -1;
}

void stack_clear(Stack *s) {
    s->index = -1;
}

int stack_is_full(Stack *s) {
    return s->index >= MaxSize - 1;
}

int stack_is_empty(Stack *s) {
    return s->index == -1;
}

void stack_push(Stack *s, char d) {
    if (!stack_is_full(s)) {
        s->Data[++s->index] = d;
    }
}

char stack_pop(Stack *s) {
    if (!stack_is_empty(s)) {
        return s->Data[s->index--];
    }
    return '\0';  // 返回一个空字符表示栈为空
}

int stack_size(Stack *s) {
    return s->index + 1;
}


// 检查最小堆是否已满
int minheap_is_full(MinHeap *heap) {
    return heap->Size == MaxSize - 1;
}

// 检查最小堆是否为空
int minheap_is_empty(MinHeap *heap) {
    return heap->Size == 0;
}

// 将元素X插入到最小堆中
// 如果堆已满，则返回0，否则执行插入操作并返回1
int minheap_push(MinHeap *heap, HuffmanNode* X) {
    if (minheap_is_full(heap)) return 0;

    int i = heap->Size++;
    for (; i > 0 && heap->Data[(i - 1) / 2]->weight > X->weight; i = (i - 1) / 2) {
        heap->Data[i] = heap->Data[(i - 1) / 2];
    }
    heap->Data[i] = X;
    return 1;
}

// 从位置p开始下沉元素，以维持最小堆的性质
void minheap_perc_down(MinHeap *heap, int p) {
    HuffmanNode* X = heap->Data[p];
    int parent, child;
    for (parent = p; parent * 2 + 1 < heap->Size; parent = child) {
        child = parent * 2 + 1;
        if (child != heap->Size - 1 && heap->Data[child]->weight > heap->Data[child + 1]->weight) {
            child++;
        }
        if (X->weight <= heap->Data[child]->weight)
            break;
        else
            heap->Data[parent] = heap->Data[child];
    }
    heap->Data[parent] = X;
}

// 构建最小堆
// 对堆中的每个非叶子节点执行下沉操作
void minheap_build_heap(MinHeap *heap) {
    for (int i = heap->Size / 2 - 1; i >= 0; --i) {
        minheap_perc_down(heap, i);
    }
}

// 从最小堆中弹出最小元素（位于堆顶）
// 如果堆为空，则返回NULL
HuffmanNode* minheap_pop(MinHeap *heap) {
    if (minheap_is_empty(heap)) return NULL;

    HuffmanNode* minItem = heap->Data[0];
    HuffmanNode* X = heap->Data[--heap->Size];

    int parent, child;
    for (parent = 0; parent * 2 + 1 < heap->Size; parent = child) {
        child = parent * 2 + 1;
        if (child < heap->Size - 1 && heap->Data[child]->weight > heap->Data[child + 1]->weight) {
            child++;
        }
        if (X->weight <= heap->Data[child]->weight)
            break;
        else
            heap->Data[parent] = heap->Data[child];
    }
    heap->Data[parent] = X;

    return minItem;
}
/***********************************END 数据结构***********************************/

/************************************************************************/
/* 与Huffman编码相关的函数                                                 */
/************************************************************************/

/**
 * @brief 创建Huffman树
 * @param freq 频率数组
 * @param freq_size 频率数组的大小
 * @return
 */
HuffmanNode * create_huffman_tree(const int * freq, int freq_size){
    MinHeap heap;
    heap.Size = 0;  // 初始化堆的大小为0

    for(int i = 0; i < freq_size; i++) {
        if (freq[i] > 0) {  // 只处理非零频率的字符
            HuffmanNode * node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
            node->data = (unsigned char)(i);
            node->weight = freq[i];
            node->llink = NULL;
            node->rlink = NULL;
            minheap_push(&heap, node);  // 将节点加入到最小堆中
        }
    }
    minheap_build_heap(&heap);  // 构建最小堆

    // 依次弹出最小的两个节点，合并为一个新的节点，再将新的节点加入到最小堆中
    while(heap.Size > 1){
        HuffmanNode * node1 = minheap_pop(&heap);
        HuffmanNode * node2 = minheap_pop(&heap);
        HuffmanNode * node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
        node->data = 0;
        node->weight = node1->weight + node2->weight;
        node->llink = node1;
        node->rlink = node2;
        minheap_push(&heap, node);
    }

    return minheap_pop(&heap);
}


/**
 * @brief 生成Huffman编码表
 * @param root Huffman树的根节点
 * @param code_table 编码表，用于存储生成的Huffman编码，确保它的长度大于等于256
 * @param code_len 当前编码的长度（用于递归）
 * @param stack 用于存储编码的栈
 */
void generate_huffman_code(HuffmanNode * root, HuffmanCode * code_table, int code_len, Stack * stack, int * cnt){
    if(root->llink == NULL && root->rlink == NULL){ // 叶子节点
        code_table[root->data].code = (char *)malloc(sizeof(char) * code_len);
        code_table[root->data].len = code_len;
        code_table[root->data].encoded_data = root->data;
        for(int i=0; i<code_len; i++){
            code_table[root->data].code[i] = stack->Data[i];
        }
        (*cnt)++;
        return;
    }
    // 在Huffman编码中，非叶子节点通常不直接存储或表示具体字符。它们的主要作用是作为树的结构部分，帮助指导到达叶子节点（实际表示字符的节点）的路径。
    if(root->llink != NULL){
        stack_push(stack, '0');
        generate_huffman_code(root->llink, code_table, code_len+1, stack, cnt);
        stack_pop(stack);
    }
    if(root->rlink != NULL){
        stack_push(stack, '1');
        generate_huffman_code(root->rlink, code_table, code_len+1, stack, cnt);
        stack_pop(stack);
    }
}

/**
 * @brief 释放Huffman树
 * @param root Huffman树的根节点
 */
void free_huffman_tree(HuffmanNode * root){
    if(root == NULL) return;
    free_huffman_tree(root->llink);
    free_huffman_tree(root->rlink);
    free(root);
}

/**
 * @brief 释放Huffman编码表
 * @param code_table Huffman编码表
 */
void free_huffman_code(HuffmanCode * code_table){
    for(int i=0; i<256; i++){
        if(code_table[i].code != NULL){
            free(code_table[i].code);
        }
    }
}

/**
 * @brief 打印Huffman编码表
 *     用于调试
 * @param code_table Huffman编码表
 */
void print_huffman_code(HuffmanCode * code_table){
    for(int i=0; i<256; i++){
        if(code_table[i].code != NULL){
            printf("%d: ", code_table[i].encoded_data);
            for(int j=0; j<code_table[i].len; j++){
                printf("%c", code_table[i].code[j]);
            }
            printf("\n");
        }
        else{
            printf("%d: NULL\n", i);
        }
    }
}

/********************************END****************************************/

/**
 * @brief 对图像进行Huffman编码
 * @param rgb_img_buf RGB图像
 * @param binary_img_buf 二值化图像
 * @param code_table Huffman编码表
 * @param height 图像的高度
 * @param width 图像的宽度
 * @param channel 图像的通道数
 */
void encode_image(const char * filename,
                  const unsigned char * rgb_img_buf, const unsigned char * binary_img_buf,
                  int height, int width, int channel) {
    /*************************************************
     * 首先根据感兴趣区域的像素值计算RGB值的频率
     * 然后构建Huffman树
     ************************************************/
    // 计算RGB值的频率
    int freq[256];
    calculate_freq(rgb_img_buf, binary_img_buf, freq, height, width, channel);
    // 构建Huffman树
    HuffmanNode *root = create_huffman_tree(freq, 256);
    // 初始化Huffman编码表
    HuffmanCode * code_table = (HuffmanCode *)malloc(sizeof(HuffmanCode) * 256);
    for (int i = 0; i < 256; i++) {
        code_table[i].code = NULL;
        code_table[i].len = 0;
    }
    // 生成Huffman编码表
    Stack stack;    // 用于辅助生成Huffman编码的栈
    stack_init(&stack);
    int huffman_code_cnt = 0;
    generate_huffman_code(root, code_table, 0, &stack, &huffman_code_cnt);

    /*************************************************
     * 将编码表首先写入文件
     ************************************************/
    FILE * fp = fopen(filename, "wb");
    // 写入编码表
    fwrite(&huffman_code_cnt, sizeof(int), 1, fp); // 首先写入编码表的长度
    for (int i = 0; i < 256; i++) {
        if (code_table[i].code != NULL) {
            fwrite(&code_table[i].encoded_data, sizeof(unsigned char), 1, fp);
            fwrite(&code_table[i].len, sizeof(int), 1, fp);
            fwrite(code_table[i].code, sizeof(char), code_table[i].len, fp);
        }
    }
    /*************************************************
     * 写入图像的高度、宽度、通道数
     ************************************************/
    fwrite(&height, sizeof(int), 1, fp);
    fwrite(&width, sizeof(int), 1, fp);
    fwrite(&channel, sizeof(int), 1, fp);
    /*************************************************
     * 写入图像的mask，表示感兴趣的区域
     ************************************************/
    // 计算mask图像所需要的bit数
    int mask_bit_num = (height * width + 7) / 8; // height*width/8 向上取整
    unsigned char * mask_save_buf = (unsigned char *)malloc(mask_bit_num * sizeof(unsigned char));
    for(int i=0; i<height*width; i++){
        if(binary_img_buf[i] == 255){
            mask_save_buf[i/8] |= (1 << (7 - i%8));     // 将第i个bit置为1
        }else{
            mask_save_buf[i/8] &= ~(1 << (7 - i%8));    // 将第i个bit置为0
        }
    }
    fwrite(mask_save_buf, sizeof(unsigned char), mask_bit_num, fp);
    free(mask_save_buf); // 释放内存

    /*************************************************
     * 最后写入编码后的图像数据
     ************************************************/
    int pixel_bit_num;
    int pixel_cnt = 0;
    for(int i = 0; i < height * width * channel; i++) {
        if (binary_img_buf[i / channel] == 255) {
            // 只编码感兴趣的区域，跳过那些不感兴趣的区域
            pixel_bit_num = (code_table[rgb_img_buf[i]].len + 7)/8; // 向上取整
            unsigned char * pixel_save_buf = (unsigned char *)malloc(pixel_bit_num * sizeof(unsigned char));
            for(int j=0; j<code_table[rgb_img_buf[i]].len; j++){
                if(code_table[rgb_img_buf[i]].code[j] == '1'){
                    pixel_save_buf[j/8] |= (1 << (7 - j%8));     // 将第j个bit置为1
                }else{
                    pixel_save_buf[j/8] &= ~(1 << (7 - j%8));    // 将第j个bit置为0
                }
            } // end for
            fwrite(pixel_save_buf, sizeof(unsigned char), pixel_bit_num, fp);
            free(pixel_save_buf); // 释放内存
        }
    }

    // 关闭文件
    fclose(fp);
    free_huffman_tree(root);        // 释放Huffman树
    free_huffman_code(code_table);  // 释放Huffman编码表
}

/**
 * @brief 对图像进行Huffman解码
 * @param filename 文件名
 * @param height 存储解码后图像的高度
 * @param width 存储解码后图像的宽度
 * @param channel 存储解码后图像的通道数
 * @return 解码后的图像数据
 */
unsigned char * decode_image(const char * filename,
                             int * height, int * weight, int * channel) {
    // 读取编码表
    FILE *fp = fopen(filename, "rb");
    HuffmanCode *code_table = (HuffmanCode *) malloc(sizeof(HuffmanCode) * 256);
    for (int i = 0; i < 256; i++) {
        code_table[i].code = NULL;
        code_table[i].len = 0;
    }

    unsigned char byte;

    /*************************************************
     * 首先读取编码表的长度，然后读取编码表
     ************************************************/
    int huffman_code_cnt;
    fread(&huffman_code_cnt, sizeof(int), 1, fp);
    for (int i = 0; i < huffman_code_cnt; i++) {
        fread(&byte, sizeof(unsigned char), 1, fp);
        code_table[byte].encoded_data = byte;   // 读取被编码的数据
        fread(&code_table[byte].len, sizeof(int), 1, fp); // 读取编码长度
        code_table[byte].code = (char *)malloc(sizeof(char) * code_table[byte].len); // 为编码值分配内存
        fread(code_table[byte].code, sizeof(char), code_table[byte].len, fp); // 读取编码值
    }

    /*************************************************
     * 接下来读取图像的高度、宽度、通道数
     ************************************************/
    fread(height, sizeof(int), 1, fp);
    fread(weight, sizeof(int), 1, fp);
    fread(channel, sizeof(int), 1, fp);
    int h = *height;
    int w = *weight;

    /*************************************************
     * 然后读取mask图像，得到感兴趣的区域
     ************************************************/
    unsigned char * decode_mask_buf = (unsigned char *)malloc(h * w * sizeof(unsigned char));
    // 计算mask图像所需要的bit数
    int mask_bit_num = (*height * *weight + 7) / 8; // height*width/8 向上取整
    for(int i = 0; i < mask_bit_num; i++) {
        fread(&byte, sizeof(unsigned char), 1, fp);
        for(int j=0; j<8; j++){
            // 检查是否超过图像的大小
            if(i*8+j >= h*w) break;
            if((byte >> (7 - j)) & 1){
                // 将bit=1的区域置为255，表示感兴趣的区域
                decode_mask_buf[i*8+j] = 255;
            }else{
                // 将bit=0的区域置为0，表示不感兴趣的区域
                decode_mask_buf[i*8+j] = 0;
            }
        }
    }
    // debug
    // save_image_raw("result//002_decode_mask.raw", h, w, 1, decode_mask_buf);

    /*************************************************
     * 最后读取编码后的图像数据
     ************************************************/
    int max_byte_num = h * w * (* channel); // 编码后所需的字节数总不能比原来还多吧
    unsigned char * read_bytes = (unsigned char *)malloc(max_byte_num * sizeof(unsigned char));
    // 一直读到文件末尾
    int byte_cnt = 0;
    while(!feof(fp)){
        // 先把所有的字节读进来
        fread(&byte, sizeof(unsigned char), 1, fp);
        read_bytes[byte_cnt++] = byte;
    }
    // 解码
    // 这里的主要思路是：按照RGBRGBRGB......的顺序，按照既定的顺序，对照编码表进行解码
    unsigned char * decode_img_buf = (unsigned char *)malloc(h * w * (* channel) * sizeof(unsigned char));
    int byte_index = 0;
    // 遍历所有的像素（不同通道的也视作不同的像素）
    for(int i=0; i<h*w*(*channel); i++){
        if(decode_mask_buf[i/(*channel)] < 255){
            decode_img_buf[i] = 0;  // 不感兴趣的区域置为0
        } else{
            decode_img_buf[i] = 255; // 这里也给定一个数，以防在编码表中没有找到（正常情况下不会发生）
            // 查找编码表
            for(int j=0; j<256; j++){
                if(code_table[j].code != NULL){
                    int flag = 1;
                    int code_len = code_table[j].len; // 该编码的长度
                    for(int k=0; k<code_len; k++){
                        // 读进来的字节的第k个bit（有可能跨多个字节）与编码表中第j个编码的第k位编码进行比较
                        if(((read_bytes[byte_index + k/8] >> (7 - k%8)) & 1) != (code_table[j].code[k] - '0')){
                            flag = 0;
                            break;
                        }
                    }
                    if(flag){
                        // 上面逐一比较，如果完全相同，则找到了对应的编码
                        decode_img_buf[i] = code_table[j].encoded_data;
                        byte_index += (code_len + 7) / 8; // 向上取整
                        break;
                    }
                }// end 对照
            } // end 查找编码表
        } // end if
    }

    // 关闭文件
    fclose(fp);
    free_huffman_code(code_table);  // 释放Huffman编码表

    return decode_img_buf;
}
