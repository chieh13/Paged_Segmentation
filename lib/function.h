#ifndef FUNCTIION_H
#define FUNCTION_H

#include "define.h"

// 记录内存状况
void MemoryNotes(MemoryManager *mm);

// 初始化内存管理器
void initMemoryManager(MemoryManager *mm, int memory_size, int page_size);

// 分配内存空间
int allocateMemory(MemoryManager *mm, int size);

// 释放内存空间
void deallocateMemory(MemoryManager *mm, int start, int size);

// 创建进程
PCB *createProcess(int process_id, int segment_count, int *segment_sizes, MemoryManager *mm);

// 销毁进程
void destroyProcess(PCB *pcb, MemoryManager *mm);

// 分配内存给段
int allocatePageFrames(SegmentTableEntry *segment, MemoryManager *mm);

// 回收某一页框
void deallocateOnePageFrame(int page_id, SegmentTableEntry *segment, MemoryManager *mm);

// 回收一个段分得的所有页框
void deallocatePageFrames(SegmentTableEntry *segment, MemoryManager *mm);

// 逻辑地址转换为物理地址
uintptr_t logicaToPhysical(PCB *process, int seg_id, int page_id, int offset, MemoryManager *mm);

#endif