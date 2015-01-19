#include <stdio.h>

#include "virtual.h"
#include "jit.h"

jit_context_t *create_jit_context(uint8_t *bytecode, unsigned int bytecode_size)
{
	jit_context_t *n = malloc(sizeof(jit_context_t));
	n->bytecode = bytecode;
	n->bytecode_size = bytecode_size;

	n->memory = malloc(1024 * 1024);
	n->memory_offset = 0;

	n->entries = NULL;
	n->last_entry = NULL;

	n->relocs = NULL;
	n->last_reloc = NULL;

	jit_entry_t *first_entry = create_jit_entry(bytecode);
	add_entry(n, first_entry);

	return n;
}

void release_jit_context(jit_context_t *context)
{
	
}

void add_entry(jit_context_t *context, jit_entry_t *entry)
{
	if(context->last_entry == NULL) {
		context->entries = entry;
		context->last_entry = entry;
	} else {
		context->last_entry->next = entry;
		context->last_entry = entry;
	}

}

void add_reloc(jit_context_t *context, jit_reloc_t *reloc)
{
	if(context->last_reloc == NULL) {
		context->relocs = reloc;
		context->last_reloc = reloc;
	} else {
		context->last_reloc->next = reloc;
		context->last_reloc = reloc;
	}
}

jit_entry_t *create_jit_entry(uint8_t *bytecode)
{
	jit_entry_t *n = malloc(sizeof(jit_entry_t));
	n->bytecode = bytecode;
	n->jitted_addr = NULL;
	n->next = NULL;

	return n;
}

void do_jit(jit_context_t *context)
{

}

void run_jit(jit_context_t *context)
{

}