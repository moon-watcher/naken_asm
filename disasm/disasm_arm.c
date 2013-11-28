/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "disasm_arm.h"
#include "disasm_common.h"
#include "table_arm.h"

#define READ_RAM(a) memory_read_m(memory, a)
#define ARM_NIB(n) ((opcode>>n)&0xf)

// NOTE "" is AL
static char *arm_cond[] = 
{
  "eq", "ne", "cs", "ne",
  "mi", "pl", "vs", "vc",
  "hi", "ls", "ge", "lt",
  "gt", "le", "", "nv"
};

#if 0
char *arm_alu_ops[] = 
{
  "and", "eor", "sub", "rsb",
  "add", "adc", "sbc", "rsc",
  "tst", "teq", "cmp", "cmn",
  "orr", "mov", "bic", "mvn"
};
#endif

char *arm_shift[] =
{
  "lsl", "lsr", "asr", "ror"
};

static char *arm_reg[] =
{
  "r0", "r1", "r2", "r3",
  "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11",
  "r12", "sp", "lr", "pc"
};

int get_cycle_count_arm(unsigned short int opcode)
{
  return -1;
}

static int compute_immediate(int immediate)
{
  int rotate=immediate>>8;
  immediate&=0xff;

  return immediate<<(rotate<<1);
#if 0
  int shift=(immediate>>8)*2;
  int shift_mask=(1<<(shift+1))-1;
  immediate=immediate&0xff;

  //printf("immediate=%d shift=%d shift_mask=%x\n", immediate, shift, shift_mask);

  if (shift==0) { return immediate; }
  return ((immediate&shift_mask)<<(32-shift))|(immediate>>shift);
#endif
}

static void arm_calc_shift(char *temp, int shift, int reg)
{
  if ((shift&1)==0)
  {
    sprintf(temp, "%s, %s %s", arm_reg[reg], arm_shift[(shift>>1)&0x3], arm_reg[shift>>4]);
  }
    else
  {
    int shift_amount=shift>>3;
    if (shift_amount!=0)
    {
      sprintf(temp, "%s, %s #0x%x", arm_reg[reg], arm_shift[(shift>>1)&0x3], shift>>3);
    }
      else
    {
      sprintf(temp, "%s", arm_reg[reg]);
    }
  }
}

static void arm_register_list(char *instruction, int opcode)
{
char temp[16];
int first=33;
int count=0;
int n;

  for (n=0; n<16; n++)
  {
    if ((opcode&1)==1)
    {
      if (first==33)
      {
        sprintf(temp, "r%d", n);
        if (count!=0) { strcat(instruction, ", "); }
        strcat(instruction, temp);
        first=n;
      }
      count++;
    }
      else
    {
      if (n-first>1)
      {
        sprintf(temp, "-r%d", n-1);
        strcat(instruction, temp);
      }
      first=33;
    }
    opcode>>=1;
  }

}

#if 0
  if ((opcode&MUL_LONG_MASK)==MUL_LONG_OPCODE)
  {
    int u=(opcode>>22)&1;
    int a=(opcode>>21)&1;
    int s=(opcode>>20)&1;

    if (a==0)
    {
      sprintf(instruction, "%cmull%s%s %s, %s, %s", u==1?'u':'s', arm_cond[ARM_NIB(28)], s==1?"s":"", arm_reg[ARM_NIB(16)], arm_reg[ARM_NIB(0)], arm_reg[ARM_NIB(8)]);
    }
      else
    {
      sprintf(instruction, "%cmlal%s%s %s, %s, %s, %s", u==1?'u':'s', arm_cond[ARM_NIB(28)], s==1?"s":"", arm_reg[ARM_NIB(16)], arm_reg[ARM_NIB(0)], arm_reg[ARM_NIB(8)], arm_reg[ARM_NIB(12)]);
    }
  }
    else
  if ((opcode&LDM_STM_MASK)==LDM_STM_OPCODE)
  {
  }
#endif

static void process_alu(char *instruction, uint32_t opcode, int index)
{
int i=(opcode>>25)&1;
int s=(opcode>>20)&1;
int operand2=opcode&0xfff;
char temp[32];

  if (i==0)
  {
    arm_calc_shift(temp, operand2>>4, operand2&0xf);
  }
    else
  {
    sprintf(temp, "#0x%x {#0x%02x, %d}", compute_immediate(opcode&0xfff), opcode&0xff, (opcode&0xf00)>>7);
  }

  if ((opcode&table_arm[index].mask)==0x01a00000)
  {
    sprintf(instruction, "%s%s%s %s, %s", table_arm[index].instr, arm_cond[ARM_NIB(28)], s==1?"s":"", arm_reg[ARM_NIB(12)], temp);
  }
    else
  {
    sprintf(instruction, "%s%s%s %s, %s, %s", table_arm[index].instr, arm_cond[ARM_NIB(28)], s==1?"s":"", arm_reg[ARM_NIB(12)], arm_reg[ARM_NIB(16)], temp);
  }
}

static void process_mul(char *instruction, uint32_t opcode)
{
int a=(opcode>>21)&1;
int s=(opcode>>20)&1;

  if (a==0)
  {
    sprintf(instruction, "mul%s%s %s, %s, %s", arm_cond[ARM_NIB(28)], s==1?"s":"", arm_reg[ARM_NIB(16)], arm_reg[ARM_NIB(0)], arm_reg[ARM_NIB(8)]);
  }
    else
  {
    sprintf(instruction, "mla%s%s %s, %s, %s, %s", arm_cond[ARM_NIB(28)], s==1?"s":"", arm_reg[ARM_NIB(16)], arm_reg[ARM_NIB(0)], arm_reg[ARM_NIB(8)], arm_reg[ARM_NIB(12)]);
  }
}

static void process_swap(char *instruction, uint32_t opcode)
{
int b=(opcode>>22)&1;

  sprintf(instruction, "swp%s%s %s, %s, [%s]", arm_cond[ARM_NIB(28)], b==1?"b":"", arm_reg[ARM_NIB(12)], arm_reg[ARM_NIB(0)], arm_reg[ARM_NIB(16)]);
}

static void process_mrs(char *instruction, uint32_t opcode)
{
int ps=(opcode>>22)&1;

  sprintf(instruction, "mrs%s %s, %s", arm_cond[ARM_NIB(28)], arm_reg[ARM_NIB(12)], ps==1?"SPSR":"CPSR");
}

static void process_msr_all(char *instruction, uint32_t opcode)
{
int ps=(opcode>>22)&1;

  sprintf(instruction, "msr%s %s, %s", arm_cond[ARM_NIB(28)], ps==1?"SPSR":"CPSR", arm_reg[ARM_NIB(0)]);
}

static void process_msr_flag(char *instruction, uint32_t opcode)
{
int i=(opcode>>25)&1;
int ps=(opcode>>22)&1;

  if (i==0)
  {
    sprintf(instruction, "msr%s %s_flg, %s", arm_cond[ARM_NIB(28)], ps==1?"SPSR":"CPSR", arm_reg[ARM_NIB(0)]);
  }
    else
  {
    sprintf(instruction, "msr%s %s_flg, #%d {#%d, %d}", arm_cond[ARM_NIB(28)], ps==1?"SPSR":"CPSR", compute_immediate(opcode&0xfff), opcode&0xff, (opcode&0xf00)>>7);
  }
}

static void process_ldr_str(char *instruction, uint32_t opcode, int index)
{
int w=(opcode>>21)&1;
int b=(opcode>>22)&1;
int u=(opcode>>23)&1;
int pr=(opcode>>24)&1;
int i=(opcode>>25)&1;
int offset=opcode&0xfff;
int rn=ARM_NIB(16);
char temp[32];

  if (i==0)
  {
    if (offset==0)
    {
      sprintf(temp, "[%s]", arm_reg[rn]);
    }
      else
    {
      if (pr==1)
      {
        sprintf(temp, "[%s, #%s%d]", arm_reg[rn], u==0?"-":"", offset);
      }
        else
      {
        sprintf(temp, "[%s], #%s%d", arm_reg[rn], u==0?"-":"", offset);
      }
    }
  }
    else
  {
    int shift=offset>>8;
    int type=(shift>>1)&0x3;
    int rm=offset&0xf;

    if ((shift&1)==0)
    {
      //*cycles_min=2;
      //*cycles_max=2;

      if (pr==1)
      {
        sprintf(temp, "[%s, %s, %s %s]", arm_reg[rn], arm_reg[rm], arm_shift[type], arm_reg[shift>>4]);
      }
        else
      {
        sprintf(temp, "[%s], %s, %s %s", arm_reg[rn], arm_reg[rm], arm_shift[type], arm_reg[shift>>4]);
      }
    }
      else
    {
      if (pr==1)
      {
        sprintf(temp, "[%s, %s, %s #%d]", arm_reg[rn], arm_reg[rm], arm_shift[type], shift>>3);
      }
        else
      {
        if ((shift>>3)==0)
        {
          sprintf(temp, "[%s], %s, %s #%d", arm_reg[rn], arm_reg[rm], arm_shift[type], shift>>3);
        }
          else
        {
          sprintf(temp, "[%s], %s", arm_reg[rn], arm_reg[rm]);
        }
      }
    }
  }

  sprintf(instruction, "%s%s%s %s, %s%s", table_arm[index].instr, arm_cond[ARM_NIB(28)], b==0?"":"b", arm_reg[ARM_NIB(12)], temp, w==0?"":"!");
}

static void process_undefined(char *instruction, uint32_t opcode)
{
  // hmm.. why?
  strcpy(instruction, "???");
}

static void process_ldm_stm(char *instruction, uint32_t opcode, int index)
{
char *pru_str[] = { "db", "ib", "da", "ia" };
int w=(opcode>>21)&1;
int s=(opcode>>22)&1;
int pru=(opcode>>23)&0x3;

  sprintf(instruction, "%s%s%s %s%s, {", table_arm[index].instr, pru_str[pru], s==1?"s":"",  arm_reg[ARM_NIB(16)], w==1?"!":"");

  arm_register_list(instruction, opcode);

  strcat(instruction, "}");
}

static void process_branch(char *instruction, uint32_t opcode, uint32_t address)
{
int l=(opcode>>24)&1;

  int32_t offset=(opcode&0xffffff);
  if ((offset&(1<<23))!=0) { offset|=0xff000000; }
  offset<<=2;

  // address+8 (to allow for the pipeline)
  sprintf(instruction, "%s%s 0x%02x (%d)", l==0?"b":"bl", arm_cond[ARM_NIB(28)], (address+8)+offset, offset);
}

static void process_branch_exchange(char *instruction, uint32_t opcode)
{
  sprintf(instruction, "bx%s %s", arm_cond[ARM_NIB(28)], arm_reg[ARM_NIB(0)]);
}

static void process_swi(char *instruction, uint32_t opcode)
{
  sprintf(instruction, "swi%s", arm_cond[ARM_NIB(28)]);
}

static void process_co_swi(char *instruction, uint32_t opcode)
{
}

static void process_co_transfer(char *instruction, uint32_t opcode)
{
}

static void process_co_op_mask(char *instruction, uint32_t opcode)
{
  sprintf(instruction, "cdp%s %d, %d, cr%d, cr%d, cr%d, %d", arm_cond[ARM_NIB(28)], ARM_NIB(8), ARM_NIB(20), ARM_NIB(12), ARM_NIB(16), ARM_NIB(0), (opcode>>5)&0x7);
}

static void process_co_transfer_mask(char *instruction, uint32_t opcode)
{
int ls=(opcode>>20)&1;
int w=(opcode>>21)&1;
int n=(opcode>>22)&1;
int u=(opcode>>23)&1;
int pr=(opcode>>24)&1;
int offset=opcode&0xff;

  if (offset==0)
  {
    sprintf(instruction, "%s%s%s %d, cr%d, [r%d]", ls==1?"ldc":"stc", arm_cond[ARM_NIB(28)], n==1?"l":"", ARM_NIB(8), ARM_NIB(12), ARM_NIB(16));
  }
    else
  if (pr==1)
  {
    sprintf(instruction, "%s%s%s %d, cr%d, [r%d, #%s%d]%s", ls==1?"ldc":"stc", arm_cond[ARM_NIB(28)], n==1?"l":"", ARM_NIB(8), ARM_NIB(12), ARM_NIB(16), u==0?"-":"", offset, w==1?"!":"");
  }
    else
  {
    sprintf(instruction, "%s%s%s %d, cr%d, [r%d], #%s%d%s", ls==1?"ldc":"stc", arm_cond[ARM_NIB(28)], n==1?"l":"", ARM_NIB(8), ARM_NIB(12), ARM_NIB(16), u==0?"-":"", offset, w==1?"!":"");
  }
}

int disasm_arm(struct _memory *memory, int address, char *instruction, int *cycles_min, int *cycles_max)
{
uint32_t opcode;

  *cycles_min=-1;
  *cycles_max=-1;
  opcode=get_opcode32(memory, address);
  //printf("%08x: opcode=%08x\n", address, opcode);

  int n=0;
  while(table_arm[n].instr!=NULL)
  {
    //printf("%08x  %08x  %08x %08x\n", opcode, table_arm[n].mask, (opcode&table_arm[n].mask), table_arm[n].opcode);
    if ((opcode&table_arm[n].mask)==table_arm[n].opcode)
    {
      //*cycles_min=table_arm[n].cycles;
      //*cycles_max=table_arm[n].cycles;

      switch(table_arm[n].type)
      {
        case OP_ALU:
          //*cycles_min=2;
          //*cycles_max=2;
          process_alu(instruction, opcode, n);
          break;
        case OP_MULTIPLY:
          process_mul(instruction, opcode);
          break;
        case OP_SWAP:
          process_swap(instruction, opcode);
          break;
        case OP_MRS:
          process_mrs(instruction, opcode);
          break;
        case OP_MSR_ALL:
          process_msr_all(instruction, opcode);
          break;
        case OP_MSR_FLAG:
          process_msr_flag(instruction, opcode);
          break;
        case OP_LDR_STR:
          process_ldr_str(instruction, opcode, n);
          break;
        case OP_UNDEFINED:
          process_undefined(instruction, opcode);
          break;
        case OP_LDM_STM:
          process_ldm_stm(instruction, opcode, n);
          break;
        case OP_BRANCH:
          process_branch(instruction, opcode, address);
          //*cycles_max=3;
          break;
        case OP_BRANCH_EXCHANGE:
          process_branch_exchange(instruction, opcode);
          break;
        case OP_SWI:
          process_swi(instruction, opcode);
          break;
        case OP_CO_SWI:
          process_co_swi(instruction, opcode);
          break;
        case OP_CO_TRANSFER:
          process_co_transfer(instruction, opcode);
          break;
        case OP_CO_OP_MASK:
          process_co_op_mask(instruction, opcode);
          break;
        case OP_CO_TRANSFER_MASK:
          process_co_transfer_mask(instruction, opcode);
          break;
        default:
          strcpy(instruction, "???");
          break;
      }
    }

    n++;
  }

  return 4;
}

void list_output_arm(struct _asm_context *asm_context, int address)
{
int cycles_min,cycles_max;
char instruction[128];
unsigned int opcode=get_opcode32(&asm_context->memory, address);

  fprintf(asm_context->list, "\n");
  disasm_arm(&asm_context->memory, address, instruction, &cycles_min, &cycles_max);
  fprintf(asm_context->list, "0x%04x: 0x%08x %-40s cycles: ", address, opcode, instruction);

  if (cycles_min==-1)
  { fprintf(asm_context->list, "\n"); }
    else
  if (cycles_min==cycles_max)
  { fprintf(asm_context->list, "%d\n", cycles_min); }
    else
  { fprintf(asm_context->list, "%d-%d\n", cycles_min, cycles_max); }

}

void disasm_range_arm(struct _memory *memory, int start, int end)
{
// Are these correct and the same for all MSP430's?
char *vectors[16] = { "", "", "", "", "", "",
                      "", "", "", "",
                      "", "", "", "",
                      "",
                      "Reset/Watchdog/Flash" };
char instruction[128];
int vectors_flag=0;
int cycles_min=0,cycles_max=0;
int num;

  printf("\n");

  printf("%-7s %-5s %-40s Cycles\n", "Addr", "Opcode", "Instruction");
  printf("------- ------ ----------------------------------       ------\n");

  while(start<=end)
  {
    if (start>=0xffe0 && vectors_flag==0)
    {
      printf("Vectors:\n");
      vectors_flag=1;
    }

    num=READ_RAM(start)|(READ_RAM(start+1)<<8);

    disasm_arm(memory, start, instruction, &cycles_min, &cycles_max);

    if (vectors_flag==1)
    {
      printf("0x%04x: 0x%04x  Vector %2d {%s}\n", start, num, (start-0xffe0)/2, vectors[(start-0xffe0)/2]);
      start+=2;
      continue;
    }

    if (cycles_min<1)
    {
      printf("0x%04x: 0x%04x %-40s ?\n", start, num, instruction);
    }
      else
    if (cycles_min==cycles_max)
    {
      printf("0x%04x: 0x%04x %-40s %d\n", start, num, instruction, cycles_min);
    }
      else
    {
      printf("0x%04x: 0x%04x %-40s %d-%d\n", start, num, instruction, cycles_min, cycles_max);
    }

#if 0
    count-=4;
    while (count>0)
    {
      start=start+4;
      num=READ_RAM(start)|(READ_RAM(start+1)<<8);
      printf("0x%04x: 0x%04x\n", start, num);
      count-=4;
    }
#endif

    start=start+4;
  }
}

