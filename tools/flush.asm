; Adapted from https://github.com/IAIK/flush_flush/blob/master/sc/cacheutils.h

global _start

section .text

_start:
    mov rax, 0xA17400088
    mov rbx, 0xA17400088
    ;clflush [rbx]
    ;clflush [0xA17400088]
    ;mov rax, [rbx]
    mov rcx, rax
    sub rax, rcx
    mov rax, rcx
    mov rax, 0xF1740008811
