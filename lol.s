main:
mv r0, 55
nop
jmp shit
lol:
push 20
int 0
pop r3
int 0
jmp end
shit:
mv r1, 55
je lol
end:
int 1
ret
call end
z r0
mv r1, [shit]
