#pragma once

#include "cpu/types.h"
#include "kernel/paging.h"

#define PROCESS_STATE_RUNNING (0)
#define PROCESS_STATE_READY (1)
#define PROCESS_STATE_BLOCKED (2)

#define MAX_THREAD 8

struct trap_frame_t {
   u32_t esp;
   u32_t ebp;
   u32_t eip;
   u32_t edi;
   u32_t esi;
   u32_t eax;
   u32_t ebx;
   u32_t ecx;
   u32_t edx;
   u32_t flags;
};

struct process_t;
struct thread_t {
   process_t *parent;

   addr_t virtual_stack; // Virtual address of stack (Grows downwards)
   addr_t physical_stack; // Physical address of stack (Grows upwards)
   size_t stack_size_in_pages;

   addr_t kernel_stack;
   u8_t priority;
   int state;
   trap_frame_t frame;
};

struct process_t{
   u32_t id;
   u8_t priority;
   page_directory_t *page_directory;
   u8_t state;
   process_t *next;
   thread_t threads[MAX_THREAD];
};
