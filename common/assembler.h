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

#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H

#include <stdio.h>

#include "common/cpu_list.h"
#include "common/macros.h"
#include "common/memory.h"
#include "common/memory_pool.h"
#include "common/print_error.h"
#include "common/symbols.h"
#include "common/tokens.h"

#define TOKENLEN 512
#define PARAM_STACK_LEN 4096
#define INCLUDE_PATH_LEN 4096

//#define DL_EMPTY -1
//#define DL_DATA -2
//#define DL_NO_CG -3

#define SEGMENT_CODE 0
#define SEGMENT_BSS 1

struct _asm_context
{
  FILE *list;
  struct _memory memory;
  struct _symbols symbols;
  struct _macros macros;
  parse_instruction_t parse_instruction;
  parse_directive_t parse_directive;
  list_output_t list_output;
  int address;
  int segment;
  int line;
  int pass;
  int instruction_count;
  int data_count;
  int code_count;
  int ifdef_count;
  int parsing_ifdef;
  // tokens_get start - maybe move into its own struct
  FILE *in;
  const char *filename;
  struct _token_buffer token_buffer;
  char pushback[TOKENLEN];
  char pushback2[TOKENLEN];
  int pushback_type;
  int pushback2_type;
  char unget[512];
  int unget_ptr;
  int unget_stack[MAX_NESTED_MACROS+1];
  int unget_stack_ptr;
  // tokens_get end
  int debug_file;
  char def_param_stack_data[PARAM_STACK_LEN];
  int def_param_stack_ptr[MAX_NESTED_MACROS+1];
  int def_param_stack_count;
  char include_path[INCLUDE_PATH_LEN];
  int cpu_list_index;
  uint8_t cpu_type;
  uint8_t bytes_per_address;
  uint8_t is_dollar_hex : 1;
  uint8_t can_tick_end_string : 1;
  uint8_t quiet_output : 1;
  uint8_t error : 1;
  uint8_t msp430_cpu4 : 1;
  uint8_t no_symbols : 1;
  uint8_t pass_1_write_disable : 1;
  uint8_t write_list_file : 1;
  uint8_t dump_symbols : 1;
  uint8_t dump_macros : 1;
  uint8_t optimize : 1;
  uint32_t flags;
  uint32_t extra_context;
};

int add_to_include_path(struct _asm_context *asm_context, char *paths);
void assembler_init(struct _asm_context *asm_context);
void assembler_free(struct _asm_context *asm_context);
void assembler_print_info(struct _asm_context *asm_context, FILE *out);
int assemble(struct _asm_context *asm_context);

#endif

