/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _SIMULATE_MSP430_H
#define _SIMULATE_MSP430_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/common.h"

struct _simulate_msp430
{
  uint16_t reg[16];
};

struct _simulate *simulate_init_msp430();
void simulate_free_msp430(struct _simulate *simulate);
int simulate_dumpram_msp430(struct _simulate *simulate, int start, int end);
void simulate_push_msp430(struct _simulate *simulate, uint32_t value);
int simulate_set_reg_msp430(struct _simulate *simulate, char *reg_string, uint32_t value);
uint32_t simulate_get_reg_msp430(struct _simulate *simulate, char *reg_string);
void simulate_set_pc_msp430(struct _simulate *simulate, uint32_t value);
void simulate_reset_msp430(struct _simulate *simulate);
void simulate_dump_registers_msp430(struct _simulate *simulate);
int simulate_run_msp430(struct _simulate *simulate, int max_cycles, int step);

#endif

