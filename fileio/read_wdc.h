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

#ifndef _READ_WDC_H
#define _READ_WDC_H

#include <stdint.h>

#include "common/memory.h"

int read_wdc(char *filename, struct _memory *memory);

#endif

