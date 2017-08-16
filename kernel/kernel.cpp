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

kernel *_kernel = nullptr;

void _timer_handler(registers_t const p_registers) { _kernel->timer_handler(p_registers); }
void _page_fault_handler(registers_t const p_registers) { _kernel->page_fault_handler(p_registers); }
void _keyboard_handler(registers_t const p_registers) { _kernel->keyboard_handler(p_registers); }
extern "C" void _sys(registers_t const p_registers) { _kernel->sys(p_registers); }
extern "C" int _sys_exit(u32_t const p_exit_code) { return _kernel->sys_exit(p_exit_code); }
extern "C" int _sys_write(char_t const * const p_string){ return _kernel->sys_write(p_string); }
extern "C" int _sys_read(char_t * const p_buffer, size_t const p_count){ return _kernel->sys_read(p_buffer, p_count); }
extern "C" int _sys_alloc(size_t const p_count, addr_t * const p_buffer){ return _kernel->sys_alloc(p_count, p_buffer); }
addr_t operator new(long unsigned int const p_size) { return _kernel->m_heap.allocate(p_size); }
addr_t operator new[](long unsigned int const p_size) { return _kernel->m_heap.allocate(p_size); }
void operator delete(addr_t const p_address) { _kernel->m_heap.free(p_address); }
void operator delete[](addr_t const p_address) { _kernel->m_heap.free(p_address); }

void debug(char_t const * const p_string) {_kernel->m_terminal.write(p_string);}
void debug(u32_t const p_integer) {_kernel->m_terminal.write(p_integer);}

void kernel::task_switch() {
  if(_kernel->m_processes.get_size() > 0 ) {


    do {
      _kernel->m_process_index++;
      if(_kernel->m_process_index >= _kernel->m_processes.get_size())
        _kernel->m_process_index = 0;
    } while(_kernel->m_processes[_kernel->m_process_index]->state == process_state_t::process_state_blocked);

    load_page_directory(reinterpret_cast<u32_t *>(_kernel->m_processes[_kernel->m_process_index]->m_page_directory));

    this->m_userland = true;
    enter_usermode(_kernel->m_processes[_kernel->m_process_index]->threads[0]->m_cpu_state);
  } else this->panic("No process to run!");
}

void kernel::timer_handler(registers_t const p_registers) {
  load_page_directory((u32_t*)this->m_identity_page_directory);
  if(this->m_userland) {
    thread *thr = this->m_processes[_kernel->m_process_index]->threads[0];
    thr->m_cpu_state.esp = p_registers.useresp;
    thr->m_cpu_state.ebp = p_registers.ebp;
    thr->m_cpu_state.eip = p_registers.eip;
    thr->m_cpu_state.edi = p_registers.edi;
    thr->m_cpu_state.esi = p_registers.esi;
    thr->m_cpu_state.eax = p_registers.eax;
    thr->m_cpu_state.ebx = p_registers.ebx;
    thr->m_cpu_state.ecx = p_registers.ecx;
    thr->m_cpu_state.edx = p_registers.edx;
    thr->m_cpu_state.flags = p_registers.eflags;
  }
  this->task_switch();
}

void kernel::page_fault_handler(registers_t const p_registers) {
  UNUSED(p_registers);
  load_page_directory(reinterpret_cast<u32_t *>(this->m_identity_page_directory));
  u32_t faulting_address;
  asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
  if(this->m_userland) {
    this->m_processes[_kernel->m_process_index]->m_terminal->write("Page fault!");
    this->terminate_process(_kernel->m_process_index);
    this->task_switch();
  }
  else
    this->panic("Page fault!\n");
  // Do nothing
}

size_t active_terminal_process_index = 0;
bool shift = false, ctrl = false, alt = false;
void kernel::keyboard_handler(registers_t const p_registers) {
  UNUSED(p_registers);
  load_page_directory(reinterpret_cast<u32_t *>(this->m_identity_page_directory));
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
      this->m_active_terminal->keyboard_event(scancode, up, ctrl, alt, shift);
    }
  }
  if(this->m_userland) {
    load_page_directory(reinterpret_cast<u32_t *>(_kernel->m_processes[_kernel->m_process_index]->m_page_directory));
  }
}

void kernel::sys(registers_t const p_registers) {
  load_page_directory(reinterpret_cast<u32_t *>(_kernel->m_identity_page_directory));
  thread *thr = this->m_processes[_kernel->m_process_index]->threads[0];
  thr->m_cpu_state.esp = p_registers.useresp;
  thr->m_cpu_state.ebp = p_registers.ebp;
  thr->m_cpu_state.eip = p_registers.eip;
  thr->m_cpu_state.edi = p_registers.edi;
  thr->m_cpu_state.esi = p_registers.esi;
  thr->m_cpu_state.eax = p_registers.eax;
  thr->m_cpu_state.ebx = p_registers.ebx;
  thr->m_cpu_state.ecx = p_registers.ecx;
  thr->m_cpu_state.edx = p_registers.edx;
  thr->m_cpu_state.flags = p_registers.eflags;
}

int kernel::sys_exit(u32_t const p_exit_code) {
  terminal *term = _kernel->m_processes[_kernel->m_process_index]->m_terminal;
  term->write("\nProcess terminated with exit code: ");
  term->write(p_exit_code);
  _kernel->terminate_process(_kernel->m_process_index);
  _kernel->task_switch();
  return 0;
}

int kernel::sys_write(char_t const * const p_string) {
  page_directory_t *process_directory = this->m_processes[_kernel->m_process_index]->m_page_directory;
  char_t *p_physical_string = reinterpret_cast<char_t *>(get_physical(process_directory, reinterpret_cast<u32_t>(p_string)));
  this->m_processes[_kernel->m_process_index]->m_terminal->write(p_physical_string);
  load_page_directory(reinterpret_cast<u32_t *>(process_directory));
  return 0;
}

int kernel::sys_read(char_t * const p_buffer, size_t const p_count) {
  page_directory_t *process_directory = this->m_processes[_kernel->m_process_index]->m_page_directory;
  process *proc = this->m_processes[_kernel->m_process_index];
  proc->m_sys_read_buffer = reinterpret_cast<char_t *>(get_physical(process_directory, reinterpret_cast<u32_t>(p_buffer)));
  proc->m_sys_read_count = p_count;
  proc->state = process_state_t::process_state_blocked;
  proc->flush_input_buffer();
  _kernel->task_switch();
  return 0;
}

int kernel::sys_alloc(size_t const p_count, addr_t * const p_address) {
  process *proc = this->m_processes[_kernel->m_process_index];
  addr_t *physical = reinterpret_cast<addr_t *>(get_physical(proc->m_page_directory, reinterpret_cast<u32_t>(p_address)));
  *physical = proc->expand_heap(p_count);
  load_page_directory(reinterpret_cast<u32_t *>(proc->m_page_directory));
  return 0;
}

kernel::kernel(multiboot_info_t const &p_multiboot_info) :
    m_memory(get_best_region(p_multiboot_info)),
    m_heap(reinterpret_cast<u8_t *>(m_memory.allocate_blocks(KERNEL_HEAP_SIZE_IN_PAGES)), KERNEL_HEAP_SIZE_IN_PAGES * 4096),
    m_new_delete_initializer(this),
    m_vfs(),
    m_terminal(*this),
    m_active_terminal(&m_terminal),
    m_identity_page_directory(NULL),
    m_identity_page_tables(NULL),
    m_user_page_directory(NULL),
    m_user_page_tables(NULL),
    m_processes(),
    m_terminals(),
    m_userland(false) {

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
  init_syscalls(_sys);
  set_syscall(0, reinterpret_cast<addr_t>(_sys_exit));
  set_syscall(1, reinterpret_cast<addr_t>(_sys_write));
  set_syscall(2, reinterpret_cast<addr_t>(_sys_read));
  set_syscall(3, reinterpret_cast<addr_t>(_sys_alloc));
	asm volatile("sti");

  this->m_terminal.write("Loading a process...\n\n");

  create_process(reinterpret_cast<addr_t>(1));
  create_process(reinterpret_cast<addr_t>(9));
  create_process(reinterpret_cast<addr_t>(17));
  create_process(reinterpret_cast<addr_t>(0));

  active_terminal_process_index = 0;
  this->m_terminal.set_inactive();
  this->m_active_terminal = this->m_processes[active_terminal_process_index]->m_terminal;
  this->m_active_terminal->set_active();

  tss_set_kernel_stack(get_esp());
  init_timer(5000, _timer_handler);
}

void kernel::create_process(addr_t const p_program) {
  terminal *new_terminal = new terminal(*this);
  process *new_process = new process(*this, new_terminal, p_program);
  new_terminal->m_process = new_process;
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
