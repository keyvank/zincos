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
#include "libcpp/string.h"

static kernel *_kernel;

void _timer_handler(registers_t const p_registers) { _kernel->timer_handler(p_registers); }
void _page_fault_handler(registers_t const p_registers) { _kernel->page_fault_handler(p_registers); }
void _keyboard_handler(registers_t const p_registers) { _kernel->keyboard_handler(p_registers); }
extern "C" int _sys_exit(u32_t const p_exit_code) { return _kernel->sys_exit(p_exit_code); }
extern "C" int _sys_write(char_t const * const p_string){ return _kernel->sys_write(p_string); }
addr_t operator new(long unsigned int const p_size) { return _kernel->m_heap.allocate(p_size); }
void operator delete(addr_t const p_address) { return _kernel->m_heap.free(p_address); }

void kernel::task_switch() {
  if(_kernel->m_processes.get_size() > 0 ) {

    _kernel->m_process_index++;
    if(_kernel->m_process_index >= _kernel->m_processes.get_size())
      _kernel->m_process_index = 0;

    load_page_directory(reinterpret_cast<u32_t *>(_kernel->m_processes[_kernel->m_process_index]->m_page_directory));

    this->m_userland = true;
    enter_usermode((*_kernel->m_processes[_kernel->m_process_index]->threads)[0].m_cpu_state);
  } else this->panic("No process to run!");
}

void kernel::timer_handler(registers_t const p_registers) {
  load_page_directory((u32_t*)this->m_identity_page_directory);
  if(this->m_userland) {
    thread &thr = (*this->m_processes[_kernel->m_process_index]->threads)[0];
    thr.m_cpu_state.esp = p_registers.useresp;
    thr.m_cpu_state.ebp = p_registers.ebp;
    thr.m_cpu_state.eip = p_registers.eip;
    thr.m_cpu_state.edi = p_registers.edi;
    thr.m_cpu_state.esi = p_registers.esi;
    thr.m_cpu_state.eax = p_registers.eax;
    thr.m_cpu_state.ebx = p_registers.ebx;
    thr.m_cpu_state.ecx = p_registers.ecx;
    thr.m_cpu_state.edx = p_registers.edx;
    thr.m_cpu_state.flags = p_registers.eflags;
  }
  this->task_switch();
}

void kernel::page_fault_handler(registers_t const p_registers) {
  UNUSED(p_registers);
  u32_t faulting_address;
  asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
  this->m_terminal.write("Page fault!\n");
  while(true);
  // Do nothing
}

size_t active_terminal_process_index = 0;
bool shift = false, ctrl = false, alt = false;
void kernel::keyboard_handler(registers_t const p_registers) {
  UNUSED(p_registers);
  u8_t scancode = port_byte_in(0x60);
  bool up = is_up(scancode);
  scancode = (up ? scancode - 0x80 : scancode);
  if(scancode == KEY_LEFT_SHIFT || scancode == KEY_RIGHT_SHIFT)
    shift = !up;
  else if(scancode == KEY_LEFT_ALT)
    alt = !up;
  else if(scancode == KEY_LEFT_CTRL)
    ctrl = !up;
  else {
    if(!up && shift && scancode == KEY_TAB) {
      this->m_processes[active_terminal_process_index]->m_terminal->set_inactive();
      active_terminal_process_index++;
      if(active_terminal_process_index == this->m_processes.get_size())
        active_terminal_process_index = 0;
      this->m_active_terminal = this->m_processes[active_terminal_process_index]->m_terminal;
      this->m_active_terminal->set_active();

    } else if(this->m_userland) {
      load_page_directory(reinterpret_cast<u32_t *>(this->m_identity_page_directory));
      this->m_active_terminal->keyboard_event(scancode, up, ctrl, alt, shift);
      load_page_directory(reinterpret_cast<u32_t *>(_kernel->m_processes[_kernel->m_process_index]->m_page_directory));
    }
  }
}

int kernel::sys_exit(u32_t const p_exit_code) {
  load_page_directory(reinterpret_cast<u32_t *>(_kernel->m_identity_page_directory));
  terminal *term = _kernel->m_processes[_kernel->m_process_index]->m_terminal;
  term->write("\nProcess terminated with exit code: ");
  term->write(p_exit_code);
  _kernel->terminate_process(_kernel->m_process_index);
  _kernel->task_switch();
  return 0;
}

int kernel::sys_write(char_t const * const p_string) {
  this->m_processes[_kernel->m_process_index]->m_terminal->write(p_string);
  return 0;
}

kernel::kernel(multiboot_info_t const &p_multiboot_info) :
    m_memory(get_best_region(p_multiboot_info)),
    m_heap(reinterpret_cast<u8_t *>(m_memory.allocate_blocks(KERNEL_HEAP_SIZE_IN_PAGES)), KERNEL_HEAP_SIZE_IN_PAGES * 4096),
    m_terminal(*this),
    m_active_terminal(&m_terminal),
    m_identity_page_directory(NULL),
    m_identity_page_tables(NULL),
    m_user_page_directory(NULL),
    m_user_page_tables(NULL),
    m_processes(m_heap),
    m_terminals(m_heap),
    m_userland(false) {

  _kernel = this;

  m_terminal.set_active();
  m_terminal.clear();
	this->m_terminal.write("We are in the kernel!\nWelcome to ZincOS!\n\n");

  if(this->m_memory.get_block_count() == 0) {
    this->m_terminal.write("Cannot initialize memory!");
    return;
  }

  this->m_terminal.write(this->m_memory.get_block_count());
  this->m_terminal.write(" memory blocks initialized!\n");

  this->m_terminal.write(this->m_heap.get_size());
  this->m_terminal.write(" bytes of kernel heap initialized!\n");

  // Enable Paging by Identity Mapping whole 4GB of memory
  this->m_identity_page_directory = reinterpret_cast<page_directory_t *>(this->m_memory.allocate_blocks(1)); // Allocate a block for Identity Page Directory
  this->m_identity_page_tables = reinterpret_cast<page_table_t *>(this->m_memory.allocate_blocks(1024)); // Allocate 1024 blocks for Identity Page Tables
  this->m_user_page_directory = reinterpret_cast<page_directory_t *>(this->m_memory.allocate_blocks(1));
  this->m_user_page_tables = reinterpret_cast<page_table_t *>(this->m_memory.allocate_blocks(1024));
  if(!this->m_identity_page_directory || !this->m_identity_page_tables || !this->m_user_page_directory || !this->m_user_page_tables) {
    this->panic("Cannot allocate memory for paging!");
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
  this->m_terminal.write("Paging enabled!\n");

	gdt_install();
  isr_irq_install();
  init_keyboard(_keyboard_handler);
  init_paging(_page_fault_handler);
  init_syscalls();
  set_syscall(0, reinterpret_cast<addr_t>(_sys_exit));
  set_syscall(1, reinterpret_cast<addr_t>(_sys_write));
	asm volatile("sti");

  this->m_terminal.write("Loading a process...\n\n");
  create_process();
  create_process();
  create_process();

  active_terminal_process_index = 0;
  this->m_terminal.set_inactive();
  this->m_active_terminal = this->m_processes[active_terminal_process_index]->m_terminal;
  this->m_active_terminal->set_active();

  tss_set_kernel_stack(get_esp());
  init_timer(5000, _timer_handler);
}

void kernel::create_process() {
  terminal *new_terminal = new terminal(*this);
  process *new_process = new process(*this, new_terminal);
  this->m_processes.add(new_process);
}

void kernel::terminate_process(size_t const p_index) {
  //delete this->m_processes[p_index]->m_terminal;
  delete this->m_processes[p_index];
  this->m_processes.remove(p_index);
}

void kernel::panic(char_t const * const p_message) {
  this->m_terminal.write(p_message);
  while(true);
}
