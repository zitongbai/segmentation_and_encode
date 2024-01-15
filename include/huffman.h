//
// Created by ziton on 2024/1/13.
//

#ifndef SEGMENTATION_AND_ENCODE_HUFFMAN_H
#define SEGMENTATION_AND_ENCODE_HUFFMAN_H

#define MaxSize 256

void calculate_freq(const unsigned char * rgb_img_buf, const unsigned char * binary_img_buf,
                    int * freq, int height, int width, int channel);


/************************************************************************/
/* 为了Huffman编码，我们需要如下的数据结构                                    */
/************************************************************************/
// 定义Huffman节点
typedef struct HuffmanNode {
    int weight; // 频率
    unsigned char data; // 数据
    struct HuffmanNode * llink; // 左孩子
    struct HuffmanNode * rlink; // 右孩子
} HuffmanNode;

// 定义Huffman编码表
typedef struct HuffmanCode {
    unsigned char encoded_data; // 被编码的数据（就是对应0~255中的某一个值）
    char * code; // 上面这个数据的编码
    int len; // 编码长度
}HuffmanCode;

// 定义栈


typedef struct {
    char Data[MaxSize];
    int index;
} Stack;

void stack_init(Stack *s);
void stack_clear(Stack *s);
int stack_is_full(Stack *s);
int stack_is_empty(Stack *s);
void stack_push(Stack *s, char d);
char stack_pop(Stack *s);
int stack_size(Stack *s);

typedef struct {
    HuffmanNode* Data[MaxSize];
    int Size;
} MinHeap;

int minheap_is_full(MinHeap *heap);
int minheap_is_empty(MinHeap *heap);
int minheap_push(MinHeap *heap, HuffmanNode* X);
void minheap_perc_down(MinHeap *heap, int p);
void minheap_build_heap(MinHeap *heap);
HuffmanNode* minheap_pop(MinHeap *heap);

HuffmanNode * create_huffman_tree(const int * freq, int freq_size);
void generate_huffman_code(HuffmanNode * root, HuffmanCode * code_table, int code_len, Stack * stack, int * cnt);
void free_huffman_tree(HuffmanNode * root);
void free_huffman_code(HuffmanCode * code_table);
void print_huffman_code(HuffmanCode * code_table);
void encode_image(const char * filename,
                  const unsigned char * rgb_img_buf, const unsigned char * binary_img_buf,
                  int height, int width, int channel);
unsigned char * decode_image(const char * filename,
                             int * height, int * weight, int * channel);




#endif //SEGMENTATION_AND_ENCODE_HUFFMAN_H
