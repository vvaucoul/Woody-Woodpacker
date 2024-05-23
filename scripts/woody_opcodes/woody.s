bits 64
default rel
global _start

%define ROUNDS 32
; 32 rounds of encryption

_start:
		xor		rax, rax							; rax = 0

		; write ....WOODY....
		mov		rdi, 1								; syscall id for write
		lea		rsi, [rel message]					; message to write
		mov		rdx, 14								; size of message
		add		rax, 1								; 
		syscall										; write(1, message, 14)
		
		; entrypoint setup
		lea		rax, [original_entrypoint]			; load effective address of original_entrypoint, make a pointer to the entrypoint of the original code
		add		rax, qword [original_entrypoint]	; 
		sub		rax, qword [stub_entrypoint]		;
		sub		rax, original_entrypoint			; 
		push	rax									; push the offset to the entrypoint of the original code on the stack

		; text segment pointer setup
		lea		rdi, [text_segment]					; load effective address of text_segment, make a pointer to the text segment
		add		rdi, qword [text_segment]			;
		sub		rdi, qword [stub_entrypoint]		;
		sub		rdi, text_segment					;

		; loop setup
		mov		rcx, qword [text_segment_size]		; rcx = size of text segment
		shr		rcx, 1								; binary shift right, rcx /= 2
		add 	rcx, 1								; rcx += 1
		mov		rdx, qword [key]					; rdx = key

decrypt_code:
		mov		r10, ROUNDS - 1						; setup counter for secound jumploop
		mov		al, byte [rdi]						; x = first byte of text segment
		mov		bl, byte [rdi + 1]					; y = second byte of text segment

first_pass:
		; INV_R(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))
		xor		bl, al								; text segement character (8 bits)
		ror		bl, 3								;
		xor		rax, rdx							; key (64 bits)
		sub		al, bl								;
		rol		al, 8								;

decrypt_code_round:
		cmp		r10, 0								; check if the counter is 0
		je		decrypt_code_round_end				; if it is, jump to the end of the loop
		
		; INV_R(x, y, k) (y ^= x, y = ROR(y, 3), x ^= k, x -= y, x = ROL(x, 8))
		xor		bl, al								; text segement character (8 bits)
		ror		bl, 3								;
		xor		rax, rdx							; key (64 bits)
		sub		al, bl								;
		rol		al, 8								;

		dec		r10									; decrement the counter
		jmp		decrypt_code_round					; jump to the start of the loop

decrypt_code_round_end:
		mov		byte [rdi], al						; store the first byte of the text segment
		mov		byte [rdi + 1], bl					; store the second byte of the text segment

		add		rdi, 2								; increment the text segment pointer
		loop	decrypt_code						; decrement rcx and jump to decrypt_code while rcx != 0

end:
		mul		rcx									; multiply rax by rcx
		ret											; return to the entrypoint of the original code with the offset on the stack

message					db '....WOODY....',10
original_entrypoint		dq 0x1000111001111111
text_segment			dq 0x1000011001110011
text_segment_size		dq 0x1001001001001100
key						dq 0x1001100001001100
stub_entrypoint			dq 0x1111111111110011
