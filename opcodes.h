#include <sys/types.h>
#include <stdint.h>

#ifndef _OPCODES_
#define _OPCODES_

#define NUM_OPCODES	17

#define OPCODE_RET	0x01
#define OPCODE_CALL	0x02
#define OPCODE_INT	0x03
#define OPCODE_JMP	0x04
#define OPCODE_JE	0x05
#define OPCODE_Z	0x06
#define OPCODE_MV	0x07
#define OPCODE_ADD	0x08
#define OPCODE_SUB	0x09
#define OPCODE_MUL	0x0A
#define OPCODE_DIV	0x0B
#define OPCODE_XOR	0x0C
#define OPCODE_PUSH	0x0D
#define OPCODE_POP	0x0E
#define OPCODE_NOP	0x0F
#define OPCODE_AND	0x10
#define OPCODE_CMP	0x11

typedef union u_opcode {
	int64_t (*opcode_0)(uint8_t);
	int64_t (*opcode_1)(uint8_t, uint64_t);
	int64_t (*opcode_2)(uint8_t, uint64_t, uint64_t);	
} t_opcode;

int64_t opcode_call (uint8_t flags, uint64_t addr);
int64_t opcode_ret 	(uint8_t flags);
int64_t opcode_jmp 	(uint8_t flags, uint64_t addr);
int64_t opcode_je 	(uint8_t flags, uint64_t addr);
int64_t opcode_mv	(uint8_t flags, uint64_t to, uint64_t from);
int64_t opcode_z	(uint8_t flags, uint64_t reg);
int64_t opcode_add	(uint8_t flags, uint64_t reg, uint64_t what);
int64_t opcode_sub	(uint8_t flags, uint64_t reg, uint64_t what);
int64_t opcode_mul	(uint8_t flags, uint64_t reg, uint64_t what);
int64_t opcode_div	(uint8_t flags, uint64_t reg, uint64_t what);
int64_t opcode_xor	(uint8_t flags, uint64_t reg, uint64_t what);
int64_t opcode_int	(uint8_t flags, uint64_t which);
int64_t opcode_push (uint8_t flags, uint64_t what);
int64_t opcode_pop 	(uint8_t flags, uint64_t reg);
int64_t opcode_nop	(uint8_t flags);
int64_t opcode_and  (uint8_t flags, uint64_t reg, uint64_t what);
int64_t opcode_cmp  (uint8_t flags, uint64_t w1, uint64_t w2);

void init_opcode_table (t_opcode *);

#endif
