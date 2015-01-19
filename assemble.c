#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "opcodes.h"
#include "virtual.h"
#include "assembler.h"

t_memo op[NUM_OPCODES] = {
		{ "ret", 	OPCODE_RET, 	0 },
		{ "call",	OPCODE_CALL, 	MASK_ARGS | MASK_A1D | MASK_A1M | MASK_A1T },
		{ "int",	OPCODE_INT,		MASK_ARGS | MASK_A1D | MASK_A1M | MASK_A1T },
		{ "jmp",	OPCODE_JMP,		MASK_ARGS | MASK_A1D | MASK_A1M | MASK_A1T },
		{ "je",		OPCODE_JE,		MASK_ARGS | MASK_A1D | MASK_A1M | MASK_A1T },
		{ "z",		OPCODE_Z,		MASK_ARGS | MASK_A1M | MASK_A1T },
		{ "mv",		OPCODE_MV,		MASK_ARGS | MASK_NARGS | MASK_A1D | MASK_A1M | 
									MASK_A1T  | MASK_A2D   | MASK_A2M | MASK_A2T },
		{ "add",	OPCODE_ADD,		MASK_ARGS | MASK_NARGS | MASK_A1M | MASK_A2D |
									MASK_A2M  | MASK_A2T },
		{ "sub",	OPCODE_SUB,		MASK_ARGS | MASK_NARGS | MASK_A1M | MASK_A2D |
									MASK_A2M  | MASK_A2T },
		{ "mul",	OPCODE_MUL,		MASK_ARGS | MASK_NARGS | MASK_A1M | MASK_A2D |
									MASK_A2M  | MASK_A2T },
		{ "div",	OPCODE_DIV,		MASK_ARGS | MASK_NARGS | MASK_A1M | MASK_A2D |
									MASK_A2M  | MASK_A2T },
		{ "xor",	OPCODE_XOR,		MASK_ARGS | MASK_NARGS | MASK_A1M | MASK_A2D |
									MASK_A2M  | MASK_A2T },
		{ "push",	OPCODE_PUSH,	MASK_ARGS | MASK_A1D | MASK_A1M | MASK_A1T },
		{ "pop",	OPCODE_POP,		MASK_ARGS | MASK_A1M | MASK_A1T } 
};

uint8_t MASK_AD[2] = { MASK_A1D, MASK_A2D };
uint8_t MASK_AM[2] = { MASK_A1M, MASK_A2M };
uint8_t MASK_AT[2] = { MASK_A1T, MASK_A2T };

int assemble (char *map, int size)
{
	char **line;
	int len=0;
	int i, x, p, c, error;
	int arg_ind[2] = {0};
	int arg_reg[2] = {0};
	int arg_64[2]  = {0};
	int arglen = 0;
	
	uint8_t opcode[2];
	
	while ((line=parse (map + i, len, &p, &error)) != NULL) {
		for (x = 0; x < NUM_OPCODES; x++)
			if ((c = strcmp (line[0], op[x].mnemo)) == 0)
				break;
		if (c != 0)
			return -1;
			
		opcode[1] = op[x].opcode;	
			
		// Interpretar args	
		for (c=1; c<3; c++) {
			arg_len = strlen (line[c]);
			// Indirecto?
			if (line[c][0] == '[') {
				if (line[c][arg_len-1] != ']')
					return E_UNMATCH;
				if (!(op[x].flags && MASK_AD[c]))
					return E_INVIND;
				arg_ind[c] = 1;
				opcode[0] &= MASK_AD[c];
			}
			// 32/64 ?
			
			// Val / Reg?
			
			
			// Arg valido
			if (! (op[x].flags && MASK_NARGS))
				break;
		}
		i += p+1;
	}
	return 0;
}

int disas (char *map, int size)
{
	return 0;
}

char **parse (char *what, int max, int *pos, int *error)
{
	int i = 0, p = 0, x;
	char ** res = malloc (3 * sizeof (char *));
	memset (res, 0, 3*sizeof (char));
	for (x=0; x<3; x++) {
		// Saltamos caracteres iniciales
		while (what[p] < 'a' && what[p] > 'z') {
			if (what[p] == '\n') {
				goto salida;
			}
			else p++; 
		}
		i = p;
		
		// Delimitamos el mnemónico
		while (what[p] != ' ' && what[p] != '\n') 
			p++;
			
		res[x] = malloc (p-i+1);
		memcpy (res[x], &what[i], p-i);
		res[x][p-i] = 0x00;
		
		i = ++p;
	}
	salida:
	*pos = p;
	*error = 0;
	return res;
}
