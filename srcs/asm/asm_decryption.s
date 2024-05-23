bits 64
global decrypt_text_section

%define ROUNDS 32

decrypt_text_section:
    push rbp
    mov rbp, rsp

    xor rcx, rcx

loop:
    cmp rcx, rdi
    jge encrypt_end

    mov al, byte [rsi + rcx] ; x
    mov bl, byte [rsi + rcx + 1] ; y

    ; INV_R(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))
    xor bl, al
    ror bl, 3
    xor rax, rdx
    sub al, bl
    rol al, 8

    mov r10, ROUNDS - 1
encrypt_round:
    cmp r10, 0
    jle encrypt_round_end

    ; INV_R(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))
    xor bl, al
    ror bl, 3
    xor rax, rdx
    sub al, bl
    rol al, 8

    dec r10
    jmp encrypt_round

encrypt_round_end:
    mov byte [rsi + rcx], al
    mov byte [rsi + rcx + 1], bl

    add rcx, 2
    jmp loop

encrypt_end:
    leave
    ret