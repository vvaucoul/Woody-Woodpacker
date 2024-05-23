bits 64
global encrypt_text_section

%define ROUNDS 32

encrypt_text_section:
    push rbp
    mov rbp, rsp

    xor rcx, rcx
    ; sub rdi, 1

loop:
    cmp rcx, rdi
    jge encrypt_end

    mov al, byte [rsi + rcx] ; x
    mov bl, byte [rsi + rcx + 1] ; y

    ; R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)
    ror al, 8
    add al, bl
    xor rax, rdx
    rol bl, 3
    xor bl, al

    mov r10, ROUNDS - 1
encrypt_round:
    cmp r10, 0
    jle encrypt_round_end

    ; R(x, y, k) (x = ROR(x, 8), x += y, x ^= k, y = ROL(y, 3), y ^= x)
    ror al, 8
    add al, bl
    xor rax, rdx
    rol bl, 3
    xor bl, al

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