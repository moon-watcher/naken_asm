/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _DISASM_THUMB_H
#define _DISASM_THUMB_H

#include "common/assembler.h"

int get_cycle_count_thumb(unsigned short int opcode);
int disasm_thumb(struct _memory *memory, uint32_t address, char *instruction, int *cycles_min, int *cycles_max);
void list_output_thumb(struct _asm_context *asm_context, uint32_t start, uint32_t end);
void disasm_range_thumb(struct _memory *memory, uint32_t flags, uint32_t start, uint32_t end);

#endif

