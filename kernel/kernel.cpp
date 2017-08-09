#include "kernel/kernel.h"
#include "drivers/vga.h"
#include "kernel/util.h"
#include "cpu/isr.h"
#include "cpu/idt.h"
#include "cpu/gdt.h"
#include "cpu/timer.h"
#include "drivers/keyboard.h"
#include "kernel/multiboot.h"
#include "kernel/memory.h"
#include "kernel/heap.h"
#include "cpu/asmutil.h"
#include "kernel/paging.h"
#include "kernel/syscall.h"
#include "cpu/ports.h"
#include "kernel/process.h"
#include "drivers/disk.h"

#define APPS_START_SECTOR (116)

void keyboard_callback(registers_t const p_registers) {
    UNUSED(p_registers);
    /* The PIC leaves us the scancode in port 0x60 */
    u8_t scancode = port_byte_in(0x60);
    char sc_ascii[16];
    int_to_ascii(scancode, sc_ascii);
    kprint("Keyboard scancode: ");
    kprint(sc_ascii);
    kprint(", ");
    print_letter(scancode);
    kprint("\n");
}

u32_t tick = 0;
void timer_callback(registers_t const p_registers) {
    UNUSED(p_registers);
    // Do nothing
}

void page_fault_callback(registers_t const p_registers) {
    UNUSED(p_registers);
    kprint("Page fault!\n");
    while(true);
    // Do nothing
}

void syscall_dispatcher(registers_t const p_registers) {
    UNUSED(p_registers);
    kprint("System call!\n");
    // Do nothing
}

process_t proc;

kernel::kernel(multiboot_info_t const &p_multiboot_info) :
    m_memory(get_best_region(p_multiboot_info)),
    m_heap(reinterpret_cast<u8_t *>(m_memory.allocate_blocks(KERNEL_HEAP_SIZE_IN_PAGES)), KERNEL_HEAP_SIZE_IN_PAGES * 4096),
    m_identity_page_directory(NULL),
    m_identity_page_tables(NULL),
    m_user_page_directory(NULL),
    m_user_page_tables(NULL) {

  clear_screen();
	kprint("We are in the kernel!\nWelcome to ZincOS!\n\n");

  if(this->m_memory.get_block_count() == 0) {
    kprint("Cannot initialize memory!");
    return;
  }

  kprint(this->m_memory.get_block_count());
  kprint(" memory blocks initialized!\n");

  kprint(this->m_heap.get_size());
  kprint(" bytes of kernel heap initialized!\n");

  // Enable Paging by Identity Mapping whole 4GB of memory
  this->m_identity_page_directory = reinterpret_cast<page_directory_t *>(this->m_memory.allocate_blocks(1)); // Allocate a block for Identity Page Directory
  this->m_identity_page_tables = reinterpret_cast<page_table_t *>(this->m_memory.allocate_blocks(1024)); // Allocate 1024 blocks for Identity Page Tables
  this->m_user_page_directory = reinterpret_cast<page_directory_t *>(this->m_memory.allocate_blocks(1));
  this->m_user_page_tables = reinterpret_cast<page_table_t *>(this->m_memory.allocate_blocks(1024));
  if(!this->m_identity_page_directory || !this->m_identity_page_tables || !this->m_user_page_directory || !this->m_user_page_tables) {
    kprint("Cannot allocate memory for paging!");
    return;
  }
  for(size_t i = 0; i < 1024; i++) {
    for(size_t j = 0; j < 1024; j++)
      this->m_identity_page_tables[i].entries[j] = ((i * 1024 + j)  * 0x1000) | PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE;
    this->m_identity_page_directory->entries[i] = reinterpret_cast<u32_t>(&(this->m_identity_page_tables[i])) | PAGE_DIRECTORY_ENTRY_PRESENT | PAGE_DIRECTORY_ENTRY_WRITABLE;
  }
  for(size_t i = 0; i < 1024; i++) {
    for(size_t j = 0; j < 1024; j++) {
      this->m_user_page_tables[i].entries[j] = ((i * 1024 + j)  * 0x1000)
        | ( i < 2 /* First 8MB of memory for kernel! */ ? PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE : PAGE_TABLE_ENTRY_USER);
    }
    this->m_user_page_directory->entries[i] = reinterpret_cast<u32_t>(&(this->m_user_page_tables[i])) | ( i < 2 /* First 8MB of memory for kernel! */ ? PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE : PAGE_TABLE_ENTRY_USER);
  }
  load_page_directory(reinterpret_cast<u32_t *>(this->m_identity_page_directory));
  enable_paging();
  kprint("Paging enabled!\n");

	gdt_install();
  isr_irq_install();
  init_keyboard(keyboard_callback);
  init_timer(25,timer_callback);
  init_paging(page_fault_callback);
  init_syscalls();
	asm volatile("sti");

  kprint("Loading a process...\n\n");
  create_process();
  terminate_process();

}

#define USER_ENTRY_ADDRESS (8 * _MB)
#define USER_STACK_ADDRESS (1 * _GB)
#define USER_STACK_SIZE_IN_PAGES (2)
void kernel::create_process() {
  proc.id            = 1;
  proc.page_directory = reinterpret_cast<page_directory_t *>(this->m_memory.allocate_blocks(1));
  memory_copy(reinterpret_cast<u8_t *>(this->m_user_page_directory),reinterpret_cast<u8_t *>(proc.page_directory),sizeof(page_directory_t));
  proc.priority      = 1;
  proc.state         = PROCESS_STATE_RUNNING;

  thread_t *thr = &proc.threads[0];
  thr->kernel_stack  = 0;
  thr->parent       = &proc;
  thr->priority     = 1;
  thr->state        = PROCESS_STATE_RUNNING;

  thr->stack_size_in_pages = USER_STACK_SIZE_IN_PAGES;
  addr_t stack = this->m_memory.allocate_blocks(thr->stack_size_in_pages);
  thr->virtual_stack = (addr_t)USER_STACK_ADDRESS;
  thr->physical_stack = stack;
  for(size_t i = 0; i < thr->stack_size_in_pages; i++)
    map_physical(proc.page_directory, this->m_user_page_directory, reinterpret_cast<u32_t>((u8_t*)thr->virtual_stack - (i + 1) * 4096), reinterpret_cast<u32_t>(stack), PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_memory);

  addr_t img = this->m_memory.allocate_blocks(1);
  read_sectors(APPS_START_SECTOR, 2, (u8_t*)img);
  map_physical(proc.page_directory, this->m_user_page_directory, USER_ENTRY_ADDRESS, reinterpret_cast<u32_t>(img), PAGE_TABLE_ENTRY_PRESENT | PAGE_TABLE_ENTRY_WRITABLE | PAGE_TABLE_ENTRY_USER, this->m_memory);

  memory_set (reinterpret_cast<u8_t *>(&thr->frame), 0, sizeof (trap_frame_t));
  thr->frame.eip    = USER_ENTRY_ADDRESS;
  thr->frame.flags  = 0x200;

  load_page_directory(reinterpret_cast<u32_t *>(proc.page_directory));
  tss_set_kernel_stack(get_esp());
  enter_usermode(reinterpret_cast<u32_t>(thr->virtual_stack),reinterpret_cast<u32_t>(thr->frame.eip));
}

void kernel::terminate_process() {
}
