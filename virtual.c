#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "virtual.h"
#include "opcodes.h"
#include "interrupts.h"

t_regs regs;
int64_t offset;
uint8_t *mem;
char debug = 1;
int_func int_vect[NUM_INTS];

int main (int argc, char *argv[])
{
	int fd;
	uint8_t *bin;
	struct stat info;
	uint64_t code_size, data_size, code_offset, data_offset;
	int64_t res;
	uint64_t arg1, arg2;
	t_opcode opcodes[NUM_OPCODES + 1];
	int executed_opcodes = 0;
	
	if (argc < 2) {
		printf ("Uso: %s EXE\n", argv[0]);
		return -1;
	}
	
	if ((fd = open(argv[1], O_RDONLY)) < 0)
		return -1;
		
	fstat (fd, &info);
	if (info.st_size > MAX_CODE_SIZE || info.st_size < 40) {
		perror ("Error: tamaño incorrecto\n");
		return -1;
	}
	
	// Mapeamos el binario
	if ((bin = mmap (NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		perror ("Error: mmap\n");
		return -1;
	}
	if (! 
		(bin[0] == 'C' &&
		 bin[1] == 'V' &&
		 bin[2] == 'M' 
		)) 
	{
		perror ("Error: binario inválido\n");
		return -1;
	}
	// 1MB de RAM
	if ((mem = mmap (NULL, MEMORY, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) {
		perror ("Error: mmap\n");
		return -1;
	}
		
	// Cada cacho a su sitio
	code_offset	= (uint64_t) bin[3];
	data_offset = (uint64_t) bin[11];
	code_size 	= (uint64_t) bin[19];
	data_size	= (uint64_t) bin[27];
	memcpy (&mem[CODE_BEGIN], &bin[code_offset], code_size); 	// Code
	memcpy (&mem[DATA_BEGIN], &bin[data_offset], data_size);	// Data
	
	// No los necesitamos
	munmap (bin, info.st_size);
	close (fd);
	
	// Iniciamos algunos registros	
	memset (&regs, 0, sizeof(regs));
	regs.f.rip = CODE_BEGIN; 	// Code begins @ 512 KB
	regs.f.rsp = STACK_BEGIN; 	// Stack begins @ 256 KB
	regs.f.rbp = STACK_BEGIN;
		
	init_opcode_table(opcodes);
	init_int_vect();
	
	if (debug) {
		int i;
		printf ("Bytecode:");
		for (i=0; i<code_size; i++) {
			if (i % 16 == 0) printf ("\n");
			printf ("0x%.02hx ", mem[CODE_BEGIN + i]);
		}
		printf ("\n");
	}
		
	while ((regs.f.rip >= CODE_BEGIN) && (regs.f.rip <= (CODE_BEGIN + code_size)) && (regs.f.rip < MEMORY)) {
		if (debug) {
			printf ("Executing instruction %i [%p]\n", executed_opcodes++, (void *)regs.f.rip);
			printf ("Flags: %x\nOpcode: %x\n", mem[regs.f.rip], mem[regs.f.rip+1]);
		}
		offset = 0;
		// Opcode valido?
		if (mem[regs.f.rip +1] > NUM_OPCODES) {
			perror ("Error: bad opcode\n");
			break;
		}
		if (mem[regs.f.rip] & MASK_ARGS) {				// Tiene argumentos?
			// Primer argumento
			if (mem[regs.f.rip] & MASK_A1M) {				// Registro?
				arg1 = (uint64_t) ((uint8_t) mem[regs.f.rip + 2]);
				offset += 2 + 1;
			} else if (mem[regs.f.rip] & MASK_A1T) {		// 64 bits?
				//arg1 = (uint64_t) mem[regs.f.rip + 2];
				arg1 = *((uint64_t *)(&mem[regs.f.rip + 2]));
				offset += 2 + 8;
			} else {
				arg1 = (uint64_t) ((uint32_t) mem[regs.f.rip + 2]);
				offset += 2 + 4;
			}
			
			if (mem[regs.f.rip] & MASK_NARGS) {			// Dos argumentos?
				if (mem[regs.f.rip] & MASK_A2M) {			// Registro?
					arg2 = (uint64_t) ((uint8_t) mem[regs.f.rip + offset]);
					offset += 1;
				} else if (mem[regs.f.rip] & MASK_A2T) {	// 64 bits?
					arg2 = (uint64_t) mem[regs.f.rip + offset];
					offset += 8;
				} else {
					arg2 = (uint64_t) ((uint32_t) mem[regs.f.rip + offset]);
					offset += 4;
				}
				res = opcodes[mem[regs.f.rip + 1]].opcode_2 (mem[regs.f.rip], arg1, arg2);
			} 
			else res = opcodes[mem[regs.f.rip + 1]].opcode_1 (mem[regs.f.rip], arg1);
		}
		else {		
			offset = 2;									// No argumentos
			res = opcodes[mem[regs.f.rip + 1]].opcode_0 (mem[regs.f.rip]);
		}
		regs.f.rip += offset;
		if (res != 0)
			regs.f.rip = CODE_BEGIN + res;
	}
	
	if (debug) {
		printf ("\n\nEstado final:\n");
		int_vect[INT_SHOWSTATE] ();
	}
	return 0;
}

uint64_t resolv_content (uint8_t flags, uint64_t what, uint8_t arg)
{
	#define I_M 0
	#define I_D	1
	#define I_T	2
	static uint8_t masks[2][3] = {
		{
			MASK_A1M, MASK_A1D, MASK_A1T
		},
		{
			MASK_A2M, MASK_A2D, MASK_A2T
		}
	};

	if (arg != 1 && arg != 2) {
		perror ("Error: bad opcode\n");
		exit(-1);
	}

	arg--;
	if (flags & masks[arg][I_M]) {				// Registro?
		if (flags & masks[arg][I_D]) {			// Directo?
			if (flags & masks[arg][I_T]) {		// 64 bits?
				return regs.a[what];
			} else {
				return (uint64_t)((uint32_t) regs.a[what]);
			}
		} else {								// Indirecto
			if (flags & masks[arg][I_T]) {		// 64 bits?
				//return (uint64_t) mem[regs.a[what]];
				return fetch_64(regs.a[what]);
			} else {
				//return (uint64_t) ((uint32_t) mem[regs.a[what]]);
				return fetch_32(regs.a[what]);
			}
		}
	} else {									// Valor
		if (flags & masks[arg][I_D]) {			// Directo?
			if (flags & masks[arg][I_T]) {		// 64 bits?
				return what;
			} else {
				return (uint64_t) ((uint32_t) what);
			}
		} else {								// Indirecto
			if (flags & masks[arg][I_T]) {		// 64 bits?
				//return (uint64_t) mem[what];
				return fetch_64(what);
			} else {
				//return (uint64_t) ((uint32_t) mem[what]);
				return fetch_32(what);
			}
		}
	}
}
