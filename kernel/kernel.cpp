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

kernel *krn;
addr_t operator new(long unsigned int const p_size) {
  return krn->m_heap.allocate(p_size);
}

void operator delete(addr_t const p_address) {
  return krn->m_heap.free(p_address);
}


bool userland = false;
void scheduler(registers_t const p_registers) {
  UNUSED(p_registers);
  load_page_directory((u32_t*)krn->m_identity_page_directory);

  if(userland) {
    thread &thr = (*krn->m_processes[krn->m_process_index]->threads)[0];
    thr.frame.esp = p_registers.useresp;
    thr.frame.ebp = p_registers.ebp;
    thr.frame.eip = p_registers.eip;
    thr.frame.edi = p_registers.edi;
    thr.frame.esi = p_registers.esi;
    thr.frame.eax = p_registers.eax;
    thr.frame.ebx = p_registers.ebx;
    thr.frame.ecx = p_registers.ecx;
    thr.frame.edx = p_registers.edx;
    thr.frame.flags = p_registers.eflags;
  }

  krn->m_process_index++;
  if(krn->m_process_index == krn->m_processes.get_size())
    krn->m_process_index = 0;

  load_page_directory(reinterpret_cast<u32_t *>(krn->m_processes[krn->m_process_index]->page_directory));
  userland = true;
  enter_usermode((*krn->m_processes[krn->m_process_index]->threads)[0].frame);
}

void page_fault_callback(registers_t const p_registers) {
    UNUSED(p_registers);
    u32_t faulting_address;
   asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    kprint("Page fault!\n");
    kprint(faulting_address);
    while(true);
    // Do nothing
}

kernel::kernel(multiboot_info_t const &p_multiboot_info) :
    m_memory(get_best_region(p_multiboot_info)),
    m_heap(reinterpret_cast<u8_t *>(m_memory.allocate_blocks(KERNEL_HEAP_SIZE_IN_PAGES)), KERNEL_HEAP_SIZE_IN_PAGES * 4096),
    m_identity_page_directory(NULL),
    m_identity_page_tables(NULL),
    m_user_page_directory(NULL),
    m_user_page_tables(NULL),
    m_processes(m_heap) {

  krn = this;

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

  init_paging(page_fault_callback);
  init_syscalls();
	asm volatile("sti");

  kprint("Loading a process...\n\n");

  create_process();
  create_process();
  create_process();
  //terminate_process(0);
  //terminate_process(0);
  //terminate_process(0);
  tss_set_kernel_stack(get_esp());
  init_timer(5000, scheduler);


}

void kernel::create_process() {
  process *new_process = new process(this);
  this->m_processes.add(new_process);
}

void kernel::terminate_process(size_t const p_index) {
  delete this->m_processes[p_index];
  this->m_processes.remove(p_index);
}
