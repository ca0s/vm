#include <stdint.h>

#define LINE	256

#define E_UNMATCH 	-2
#define E_INVIND	-3

typedef struct s_mnemo {
	char *mnemo;
	uint8_t opcode;
	uint8_t flags;
} t_memo;

void uso (void);
int assemble (char *map, int size);
int disas (char *map, int size);
void init_opcodes (t_memo *);
char **parse (char *what, int max, int *pos, int *error);
