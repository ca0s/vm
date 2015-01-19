#include <sys/types.h>
#include <stdint.h>

#ifndef _VIRTUAL_
#define _VIRTUAL_

// Memoria total
#define MEMORY			1024 * 1024
#define CODE_BEGIN		1024 * 512
#define DATA_BEGIN		1024 * 256
#define STACK_BEGIN		1024 * 256 - 1
#define MAX_CODE_SIZE	1024 * 512
#define MAX_DATA_SIZE	1024 * 256

// Indice de los registros
#define NUM_REG	16
#define REG_RIP	0x00
#define REG_RSP	0x01
#define REG_RBP	0x02
#define REG_RER	0x03
#define REG_RCO	0x04
#define REG_RDO	0x05
#define REG_R0	0x06
#define REG_R1	0x07
#define REG_R2	0x08
#define REG_R3	0x09
#define REG_R4	0x0A
#define REG_R5	0x0B
#define REG_R6	0x0C
#define REG_R7	0x0D
#define REG_R8	0x0E
#define REG_RP	0x0F

// Tiene argumentos?
#define MASK_ARGS	0x80	// 10000000
// 1 o 2 argumentos?
#define MASK_NARGS	0x40	// 01000000
// 32 o 64 bits?
#define	MASK_A1T	0x02	// 00000010
#define MASK_A2T	0x01	// 00000001
// Dato o registro?
#define MASK_A1M	0x08	// 00001000
#define MASK_A2M	0x04	// 00000100
// Direccionamiento directo o indirecto?
#define MASK_A1D	0x20	// 00100000
#define MASK_A2D	0x10 	// 00010000

#define fetch_32(addr) ((uint64_t) *((uint32_t *) &mem[addr]))
#define fetch_64(addr) (*((uint64_t *) &mem[addr]))

#define write_32(addr, val) *((uint32_t *) &mem[addr]) = (uint32_t) val
#define write_64(addr, val) *((uint64_t *) &mem[addr]) = (uint64_t) val

typedef struct s_regs {
	union {
		struct {
			uint64_t rip;
			uint64_t rsp;
			uint64_t rbp;
			uint64_t rer;
			uint64_t rco;
			uint64_t rdo;
			uint64_t r0;
			uint64_t r1;
			uint64_t r2;
			uint64_t r3;
			uint64_t r4;
			uint64_t r5;
			uint64_t r6;
			uint64_t r7;
			uint64_t r8;
			uint64_t rp;
		} f;
		uint64_t a[NUM_REG];
	};
} t_regs;

typedef struct s_segment {
	uint8_t *begin;
	uint8_t *end;
	uint8_t flags;
} t_segment;

uint64_t resolv_content (uint8_t flags, uint64_t what, uint8_t arg);

#endif
