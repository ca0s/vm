main:
	push 0
	push "12345678"
	push "abcdefgh"
	mv r0, rsp
	call strlen
	add rsp, 20
	int 0
	call test
	xor r0, r0
	int 1

strlen:
	mv r1, 0
		strlen_loop:
		mv r3, [r0]
		and r3, 0xff
		cmp r3, 0
		je strlen_end
		add r0, 1
		add r1, 1
		jmp strlen_loop
	strlen_end:
	int 0
	ret

test:
call test2
ret
test2:
call test3
ret
test3:
int 0
ret