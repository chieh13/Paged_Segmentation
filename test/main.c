#include <stdio.h>
#include <stdlib.h>
#include "..\lib\define.h"
#include "..\lib\function.h"

#define MAX_PROCESS 10


// 菜单函数，打印菜单选项
void printMenu()
{
    printf("********Segmented Paging System********\n");
    printf("Please select an operation:\n");
    printf("1. Initialize system\n");
    printf("2. Create process\n");
    printf("3. View segments and page frames of a process\n");
    printf("4. Reclaim page frames of a process\n");
    printf("5. Destroy process\n");
    printf("6. Exit program\n");
}

int main()
{
    MemoryManager mm;
    PCB *process = (PCB *)malloc(MAX_PROCESS * sizeof(PCB));
    for (int i = 0; i < MAX_PROCESS; i++)
    {
        process[i].process_id = -1;
    }

    int choice;
    int process_count = 0;

    while (1)
    {
        printMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            int memory_size;
            printf("Enter memory size: ");
            scanf("%d", &memory_size);
            initMemoryManager(&mm, memory_size, PAGE_SIZE);
            printf("System initialized. There are %d page frames in memory.\n", memory_size / PAGE_SIZE);
            break;
        }

        case 2:
        {
            int process_id, segment_count;
            printf("Enter process identifier: ");
            scanf("%d", &process_id);
            printf("Enter number of segments: ");
            scanf("%d", &segment_count);
            printf("Enter size of each segment: ");
            int *segment_sizes = (int *)malloc(segment_count * sizeof(int));
            for (int i = 0; i < segment_count; i++)
            {
                scanf("%d", &segment_sizes[i]);
            }
            process_count++;
            if (process_count > 10)
            {
                printf("Maximum number of processes reached.\n");
                break;
            }
            process[process_count] = *(createProcess(process_id, segment_count, segment_sizes, &mm));
            for (int i = 0; i < segment_count; i++)
            {
                int seg_base = allocatePageFrames(&process[process_count].segment_table[i], &mm);
                process[process_count].segment_table[i].base_address = seg_base;
                if (seg_base != -1)
                {
                    printf("Memory allocation for Segment %d successful.\n", i);
                    printf("Start address of Segment %d: %d\n", i, seg_base);
                }
                else
                {
                    printf("Memory allocation for Segment %d failed.\n", i);
                    printf("Insufficient available memory. Please reset the memory space.\n");
                    break;
                }
            }
            free(segment_sizes);
            printf("Process created successfully.\n");
            break;
        }

        case 3:
        {
            if (process_count == 0)
            {
                printf("No processes currently exist.\n");
                break;
            }
            printf("Enter process ID: ");
            int pid;
            scanf("%d", &pid);
            PCB *current_process;
            for (int i = 0; i < MAX_PROCESS; i++)
            {
                if (process[i].process_id == pid)
                {
                    current_process = &process[process_count];
                }
            }
            printf("Current process's allocated segments and page frames:\n");
            printf("Process ID: %d\n", current_process->process_id);

            printf("Number of segments: %d\n", current_process->segment_count);
            for (int i = 0; i < current_process->segment_count; i++)
            {
                printf("Segment ID: %d\n", current_process->segment_table[i].segment_id);
                printf("    Base address of the segment: %d\n", current_process->segment_table[i].base_address);
                printf("    Length of the segment: %d\n", current_process->segment_table[i].length);
                for (int j = 0; j < current_process->segment_table[i].page_count; j++)
                {
                    if (current_process->segment_table[i].page_table[j].valid == 1)
                    {
                        uintptr_t physicalAddress = logicaToPhysical(current_process, i, j, 0, &mm);

                        printf("    Page number: %d  Frame number: %d Physical address: %" PRIuPTR "", current_process->segment_table[i].page_table[j].page_id, current_process->segment_table[i].page_table[j].frame_id, physicalAddress);
                    }
                    printf("\n");
                }
            }

            break;
        }

        case 4:
        {
            if (process_count == 0)
            {
                printf("No processes currently exist.\n");
                break;
            }
            printf("Enter process ID: ");
            int pid;
            scanf("%d", &pid);
            int cur_id = 0, i = 0;
            while (i < MAX_PROCESS)
            {
                if (process[i].process_id == pid)
                {
                    cur_id = i;
                    break;
                }
                i++;
            }
            if (cur_id < 10 && cur_id)
            {
                // 回收进程分配的页框
                printf("Enter segment number and page number: ");
                int seg_id, page_id;
                scanf("%d %d", &seg_id, &page_id);
                deallocateOnePageFrame(page_id, &process[cur_id].segment_table[seg_id], &mm);
                printf("Page frame successfully reclaimed.\n");
                break;
            }
            else
            {
                printf("Process does not exist. Please enter a valid process ID.\n");
                break;
            }
        }

        case 5:
        {
            if (process_count != 0)
            {
                printf("Enter process ID: ");
                int pid;
                scanf("%d", &pid);
                for (int i = 0; i < MAX_PROCESS; i++)
                {
                    if (process[i].process_id == pid)
                    {
                        destroyProcess(&process[i], &mm);
                        process_count--;
                        break;
                    }
                }
            }
            else
            {
                printf("All processes have been destroyed.\n");
            }
            break;
        }

        case 6:
        {
            while (process_count)
            {
                for (int i = 0; i < MAX_PROCESS; i++)
                {
                    if (process[i].process_id != -1)
                    {
                        destroyProcess(&process[i], &mm);
                        process_count--;
                    }
                }
            }
            free(process);
            printf("Program exited.\n");
            exit(0);
        }

        default:
            printf("Invalid option. Please enter a valid choice.\n");
            break;
        }
    }

    return 0;
}
