#ifndef __JIT__
#define __JIT__

#include <stdint.h>

typedef struct jit_context {
	uint8_t *bytecode;
	unsigned int bytecode_size;

	void *memory;
	unsigned int memory_offset;

	jit_entry_t *entries;
	jit_entry_t *last_entry;

	jit_reloc_t *relocs;
	jit_reloc_t *last_reloc;
} jit_context_t;

typedef struct jit_entry {
	uint8_t *bytecode;
	void *jitted_addr;
	jit_entry_t *next;
} jit_entry_t;

typedef struct jit_reloc {
	void *jitted_addr;
	void *value;
	jit_reloc_t *next;
} jit_reloc_t;

typedef void (jit_opcode_cb)(jit_context_t *context);

typedef struct jit_opcode {
	char *pattern;
	jit_opcode_cb *compile;
} jit_opcode_t;

jit_context_t *create_jit_context();
void add_entry(jit_context_t *context, jit_entry_t *entry);
void add_reloc(jit_context_t *context, jit_reloc_t *reloc);
void do_jit(jit_context_t *context);

#endif