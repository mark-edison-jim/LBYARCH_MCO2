section .text
bits 64
default rel
global asm_kernel
extern printf 

asm_kernel:
    mov r10, rcx
    mov rbx, [rsp + 48]
    mov rax, [rsp + 40]
    
compute:
    movsd xmm0, [r10]
    
    movsd xmm1, [rdx]
    
    movsd xmm2, [r8]
    
    movsd xmm3, [r9]
    
    subsd xmm1, xmm0
    subsd xmm3, xmm2
    mulsd xmm1, xmm1
    mulsd xmm3, xmm3
    addsd xmm1, xmm3
    sqrtsd xmm1, xmm1
    
    movsd [rax], xmm1
    
    add r10, 8
    add rdx, 8 
    add r8, 8 
    add r9, 8
    add rax, 8
    
    dec rbx
    jnz compute
    ret
