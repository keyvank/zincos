#pragma once

#include "cpu/types.h"

#define KERNEL_CODE_SEGMENT (0x08)
#define KERNEL_DATA_SEGMENT (0x10)
#define USER_CODE_SEGMENT (0x18)
#define USER_DATA_SEGMENT (0x20)

void gdt_install();
