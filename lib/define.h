#ifndef DEFINE_H
#define DEFINE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#define PAGE_SIZE 512

// 定义页表项结构
typedef struct
{
    int page_id;  // 页标识符
    int frame_id; // 页框标识符
    int valid;    // 页是否有效的标志位
} PageTableEntry;

// 定义段表项结构
typedef struct
{
    int segment_id;             // 段标识符
    int base_address;           // 段在内存中的基地址
    int length;                 // 段的长度
    PageTableEntry *page_table; // 页表
    int page_count;             // 页数目
    int *page_frames;           // 分配到的页框数组
} SegmentTableEntry;

// 定义进程控制块（PCB）结构
typedef struct
{
    int process_id;                   // 进程标识符
    SegmentTableEntry *segment_table; // 段表
    int segment_count;                // 段数目
} PCB;

// 定义页框结构
typedef struct
{
    int flag;             // 标志页框是否被分配.用于跟踪内存空闲状态
    char data[PAGE_SIZE]; // 每个页框大小为PAGE_SIZE个字节
} PageFrame;

// 定义内存管理结构
typedef struct
{
    int memory_size;   // 内存大小
    int page_size;     // 页大小
    PageFrame *bitmap; // 页框数组
} MemoryManager;

#endif