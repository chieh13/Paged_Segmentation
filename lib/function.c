#ifndef FUNCTIION_H
#define FUNCTION_H

#include "define.h"
#include "function.h"

void MemoryNotes(MemoryManager *mm)
{
    FILE *file = fopen("D:\\Code\\OS\\test\\memory.txt", "r+"); // 打开文件

    // 检查文件是否成功打开
    if (file != NULL)
    {
        // 将文件指针移动到文件开头
        fseek(file, 0, SEEK_SET);
        for (int i = 0; i < mm->memory_size / mm->page_size; i++)
        {
            fprintf(file, "%d ", mm->bitmap[i].flag);
        }
        // 清除文件剩余内容
        int position = ftell(file);
        ftruncate(fileno(file), position);

        //  刷新文件缓冲区
        fflush(file);
    }
}

// 初始化内存管理器
void initMemoryManager(MemoryManager *mm, int memory_size, int page_size)
{

    mm->memory_size = memory_size;
    mm->page_size = page_size;

    mm->bitmap = (PageFrame *)malloc(memory_size / page_size * sizeof(PageFrame));

    for (int i = 0; i < memory_size / page_size; i++)
    {
        mm->bitmap[i].flag = 0; // 0表示空闲
    }

    MemoryNotes(mm);
}

// 分配内存空间
int allocateMemory(MemoryManager *mm, int size)
{
    int required_pages = size / mm->page_size;
    // 不足一页的按一页计算
    if (size % mm->page_size != 0)
    {
        required_pages++;
    }

    // 在位图中查找连续的空闲页框
    int start = -1;
    int count = 0;
    for (int i = 0; i < mm->memory_size / mm->page_size; i++)
    {
        if (mm->bitmap[i].flag == 0)
        {
            if (count == 0)
            {
                start = i;
            }
            count++;
            if (count == required_pages)
            {
                break;
            }
        }
        else
        {
            count = 0;
            start = -1;
        }
    }

    // 如果找到连续的空闲页框，则分配内存空间
    if (count == required_pages)
    {
        for (int i = start; i < start + required_pages; i++)
        {
            mm->bitmap[i].flag = 1; // 1表示已分配
        }

        MemoryNotes(mm);

        return start; // 返回起始的空闲页框号
    }

    // 没有足够的连续空闲页框
    return -1;
}

// 释放内存空间
void deallocateMemory(MemoryManager *mm, int start, int size)
{
    int start_page = start / mm->page_size;
    int required_pages = size / mm->page_size;
    if (size % mm->page_size != 0)
    {
        required_pages++;
    }

    for (int i = start_page; i < start_page + required_pages; i++)
    {
        mm->bitmap[i].flag = 0; // 0表示空闲
    }

    MemoryNotes(mm);
}

// 创建进程
PCB *createProcess(int process_id, int segment_count, int *segment_sizes, MemoryManager *mm)
{
    // 创建进程控制块（PCB）
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    pcb->process_id = process_id;
    pcb->segment_count = segment_count;

    // 分配段表和页表的内存空间
    pcb->segment_table = (SegmentTableEntry *)malloc(segment_count * sizeof(SegmentTableEntry));

    // 初始化段表和页表
    for (int i = 0; i < segment_count; i++)
    {
        pcb->segment_table[i].segment_id = i;
        pcb->segment_table[i].base_address = -1;
        pcb->segment_table[i].length = segment_sizes[i];

        // 每个段的页数目
        int page_frame_count = segment_sizes[i] / mm->page_size;
        if (segment_sizes[i] % mm->page_size != 0)
        {
            page_frame_count++;
        }
        pcb->segment_table[i].page_count = page_frame_count;

        pcb->segment_table[i].page_frames = (int *)malloc(page_frame_count * sizeof(int));
        for (int j = 0; j < page_frame_count; j++)
        {
            pcb->segment_table[i].page_frames[j] = -1; // 页框号为-1表示还未分配页框
        }

        pcb->segment_table[i].page_table = (PageTableEntry *)malloc(pcb->segment_table[i].page_count * sizeof(PageTableEntry));
        for (int j = 0; j < page_frame_count; j++)
        {
            pcb->segment_table[i].page_table[j].page_id = j;
            pcb->segment_table[i].page_table[j].frame_id = -1; // -1表示页未分配内存
            pcb->segment_table[i].page_table[j].valid = 0;     // 0表示页无效
        }
    }

    return pcb;
}

// 销毁进程
void destroyProcess(PCB *pcb, MemoryManager *mm)
{
    pcb->process_id = -1;
    // 释放占用的内存空间
    for (int i = 0; i < pcb->segment_count; i++)
    {
        int page_frame_count = pcb->segment_table[i].page_count;
        for (int j = 0; j < page_frame_count; j++)
        {
            if (pcb->segment_table[i].page_table[j].frame_id != -1)
            {
                deallocateMemory(mm, pcb->segment_table[i].page_table[j].frame_id * mm->page_size, mm->page_size);
            }
        }
        free(pcb->segment_table[i].page_table);
    }
    free(pcb->segment_table);
}

// 分配内存给段
int allocatePageFrames(SegmentTableEntry *segment, MemoryManager *mm)
{
    int page_frame_count = segment->page_count;

    // 在位图中查找连续的空闲页框,分配内存
    int start = allocateMemory(mm, segment->length);
    // 内存足够分配
    if (start != -1)
    {
        printf("在为段%d分配内存\n", segment->segment_id);
        for (int i = start; i < start + page_frame_count; i++)
        {
            mm->bitmap[i].flag = 1; // 1表示已分配
            segment->page_table[i - start].frame_id = i;
            segment->page_table[i - start].page_id = i - start;
            segment->page_table[i - start].valid = 1;
        }
        return start * mm->page_size; // 返回段的起始地址
    }

    // 没有足够的连续空闲页框
    return -1; // 分配失败
}

// 回收某一页框
void deallocateOnePageFrame(int page_id, SegmentTableEntry *segment, MemoryManager *mm)
{

    for (int i = 0; i < segment->page_count; i++)
    {
        if (segment->page_table[i].frame_id != -1 && segment->page_table[i].page_id == page_id)
        {
            deallocateMemory(mm, segment->page_table[i].frame_id * mm->page_size, mm->page_size);
            segment->page_table[i].frame_id = -1;
            segment->page_table[i].valid = 0;
        }
    }
}

// 回收一个段分得的所有页框
void deallocatePageFrames(SegmentTableEntry *segment, MemoryManager *mm)
{
    int page_frame_count = segment->page_count;

    for (int i = 0; i < page_frame_count; i++)
    {
        if (segment->page_table[i].frame_id != -1)
        {
            deallocateMemory(mm, segment->page_table[i].frame_id * mm->page_size, mm->page_size);
            segment->page_table[i].frame_id = -1;
            segment->page_table[i].valid = 0;
        }
    }
}

// 逻辑地址转换为物理地址
uintptr_t logicaToPhysical(PCB *process, int seg_id, int page_id, int offset, MemoryManager *mm)
{
    // 检测是否段越界
    if (seg_id > process->segment_count - 1)
    {
        return -1;
    }

    // 检测是否页越界
    if (page_id > process->segment_table[seg_id].page_count - 1)
    {
        return -1;
    }

    // 找到对应的物理块
    int frame_id = process->segment_table[seg_id].page_table[page_id].frame_id;
    uintptr_t physicalAddress = (uintptr_t) & (mm->bitmap[frame_id]);

    return physicalAddress;
}

#endif