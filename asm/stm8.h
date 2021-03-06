/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#ifndef _ASM_STM8_H
#define _ASM_STM8_H

#include "common/assembler.h"

int parse_instruction_stm8(struct _asm_context *asm_context, char *instr);

#endif


