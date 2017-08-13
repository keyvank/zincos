[BITS 32]
[extern main]
global _start
_start:
  call main
mov ebx, eax ; Return code of main function
mov eax, 0 ; syscall_exit
int 123
jmp $
