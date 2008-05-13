/* gameplaySP
 *
 * Copyright (C) 2007 Exophase <exophase@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short int u16;
typedef signed short int s16;
typedef unsigned long u32;
typedef signed long s32;
typedef unsigned long long int u64;
typedef signed long long int s64;

#include "disasm.h"

#define bits(value, offset, mask)                                             \
  ((value >> (offset)) & (mask))                                              \

#define bits_cmp(value, offset, mask, check)                                  \
  (bits(value, offset, mask) == (check))                                      \

#define op_bits(offset, mask)                                                 \
  bits(opcode, offset, mask)                                                  \

#define op_bit(offset)                                                        \
  op_bits(offset, 0x1)                                                        \

#define op_bits_cmp(offset, mask, check)                                      \
  bits_cmp(opcode, offset, mask, check)                                       \

#define reg_op(offset)                                                        \
  (reg_names[op_bits(offset, 0xF)])                                           \

const char *condition_table[] =
{
  "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
  "hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

const char *data_proc_opcode_table[] =
{
  "and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
  "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"
};

const char *shift_table[] = { "lsl", "lsr", "asr", "ror" };

const char *psr_op_type[] = { "cpsr", "spsr" };

const char *psr_flags[] =
{
  "",  "c",  "x",  "xc",  "s",  "sc",  "sx",  "sxc",
  "f", "fc", "fx", "fxc", "fs", "fsc", "fsx", "fsxc"
};

const char *memory_transfer_op_type[] = { "str", "ldr" };

const char *memory_transfer_op2_type[2][4] =
{
  { "", "h", "", "" },
  { "", "h", "sb", "sh" }
};

const char *long_multiply_type[] = { "umull", "umlal", "smull", "smlal" };

const char *block_memory_op_type[] = { "stm", "ldm" };

const char *block_memory_table[] = { "da", "ia", "db", "ib" };

const char *reg_names[] =
{
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
  "r9", "r10", "r11", "r12", "sp", "lr", "pc"
};


void disasm_arm_reg_operand(u32 reg_number, char *buffer)
{
  sprintf(buffer, "%s", reg_names[reg_number]);
}

void disasm_arm_reg_operand2(u32 opcode, char *buffer)
{
  u32 rm = op_bits(0, 0xF);
  u32 shift_type = op_bit(4);
  u32 shift_op = op_bits(5, 0x3);

  if(shift_type)
  {
    u32 rs = op_bits(8, 0xF);
    sprintf(buffer, "%s, %s %s", reg_names[rm], shift_table[shift_op],
     reg_names[rs]);
  }
  else
  {
    u32 shift = op_bits(7, 0x1F);

    if(shift == 0)
    {
      switch(shift_op)
      {
        case 0:
          sprintf(buffer, "%s", reg_names[rm]);
          return;

        case 1:
        case 2:
          shift = 32;
          break;

        case 3:
          sprintf(buffer, "%s, rrx #1", reg_names[rm]);
          return;
      }
    }

    sprintf(buffer, "%s, %s #%d", reg_names[rm], shift_table[shift_op],
     shift);
  }
}

void disasm_arm_mem_reg_operand2(u32 opcode, char *buffer)
{
  u32 shift_op = op_bits(5, 0x3);
  u32 shift = op_bits(7, 0x1F);
  u32 rm = op_bits(0, 0xF);

  if(shift == 0)
  {
    switch(shift_op)
    {
      case 0:
        sprintf(buffer, "%s", reg_names[rm]);
        break;

      case 1:
      case 2:
        shift = 32;
        break;

      case 3:
        sprintf(buffer, "%s, rrx #1", reg_names[rm]);
        return;
    }
  }

  sprintf(buffer, "%s, %s #%d", reg_names[rm], shift_table[shift_op], shift);
}


void disasm_arm_imm_operand2(u32 op2, char *buffer)
{
  u32 rot = ((op2 >> 8) & 0xF) * 2;
  u32 mag = op2 & 0xFF;
  u32 val = (mag >> rot) | (mag << (32 - rot));

  sprintf(buffer, "#0x%x", val);
}


void disasm_arm_data_proc(u32 opcode, char *op2, char *buffer)
{
  u32 dp_op = op_bits(21, 0xF);
  const char *s_bit_str = "";
  const char *condition_str = condition_table[op_bits(28, 0xF)];

  if(op_bit(20))
    s_bit_str = "s";

  if((dp_op >= 0x8) && (dp_op <= 0xB))
  {
    sprintf(buffer, "%s%s %s, %s", data_proc_opcode_table[dp_op],
     condition_str, reg_op(16), op2);
  }
  else

  if((dp_op == 0xD) || (dp_op == 0xF))
  {
    sprintf(buffer, "%s%s%s %s, %s", data_proc_opcode_table[dp_op],
     condition_str, s_bit_str, reg_op(12), op2);
  }
  else
  {
    sprintf(buffer, "%s%s%s %s, %s, %s",
     data_proc_opcode_table[dp_op], condition_str, s_bit_str,
     reg_op(12), reg_op(16), op2);
  }
}

void disasm_arm_memory_transfer(u32 opcode, char *op2, char *buffer)
{
  u32 inc_type = op_bit(24);

  const char *direction_str = "";
  const char *wordsize_str = "";
  const char *op_str = memory_transfer_op_type[op_bit(20)];
  const char *condition_str = condition_table[op_bits(28, 0xF)];

  if(op_bit(22))
    wordsize_str = "b";

  if(op_bit(23) == 0)
    direction_str = "-";

  if(op_bit(24))
  {
    // Preincrement
    char *writeback_str = "";
    if(op_bit(21))
      writeback_str = "!";

    sprintf(buffer, "%s%s%s %s%s, [%s, %s%s]",
     op_str, condition_str, wordsize_str, reg_op(12),
     writeback_str, reg_op(16), direction_str, op2);
  }
  else
  {
    // Postincrement
    char *memory_management_str = "";
    if(op_bit(21))
      memory_management_str = "t";

    sprintf(buffer, "%s%s%s%s %s, [%s], %s%s",
     op_str, condition_str, wordsize_str, memory_management_str,
     reg_op(12), reg_op(16), direction_str, op2);
  }
}


void disasm_arm_memory_transfer2(u32 opcode, char *op2, char *buffer)
{
  u32 inc_type = op_bit(24);

  const char *direction_str = "";
  const char *op_str = memory_transfer_op_type[op_bit(20)];
  const char *condition_str = condition_table[op_bits(28, 0xF)];
  const char *wordsize_str =
   memory_transfer_op2_type[op_bit(20)][op_bits(5, 0x3)];

  if(op_bit(23) == 0)
    direction_str = "-";

  if(op_bit(24))
  {
    // Preincrement
    char *writeback_str = "";
    if(op_bit(21))
      writeback_str = "!";

    sprintf(buffer, "%s%s%s %s%s, [%s, %s%s]",
     op_str, condition_str, wordsize_str, reg_op(12), writeback_str,
     reg_op(16), direction_str, op2);
  }
  else
  {
    // Postincrement
    sprintf(buffer, "%s%s%s %s, [%s], %s%s", op_str, condition_str,
     wordsize_str, reg_op(12), reg_op(16), direction_str, op2);
  }
}

void get_rlist(u32 opcode, char *rlist_str)
{
  char next_str[16];
  char *rlist_ptr = rlist_str;
  int i = 0;
  int span_start, span_end;
  int hit = 0;

  *rlist_ptr = '{';
  rlist_ptr++;

  while(i < 16)
  {
    // Find a high bit
    while((op_bit(i) == 0) && (i < 16))
    {
      i++;
    }

    if(i == 16)
      break;

    span_start = i;

    // Find a low bit or the end
    while((op_bit(i) == 1) && (i < 16))
    {
      i++;
    }

    hit = 1;
    span_end = i - 1;

    if(span_start == span_end)
    {
      sprintf(next_str, "%s, ", reg_names[span_start]);
    }
    else

    if((span_start - span_end) == 1)
    {
      sprintf(next_str, "%s, %s, ", reg_names[span_start],
       reg_names[span_end]);
    }
    else
    {
      sprintf(next_str, "%s - %s, ", reg_names[span_start],
       reg_names[span_end]);
    }

    strcpy(rlist_ptr, next_str);
    rlist_ptr += strlen(next_str);
  }

  if(hit == 0)
    rlist_ptr += 2;

  strcpy(rlist_ptr - 2, "}");
}


void disasm_arm_instruction(u32 opcode, char *buffer, u32 pc,
 disasm_label *labels, u32 num_labels)
{
  const char *condition_str = condition_table[op_bits(28, 0xF)];

  switch(op_bits(25, 0x7))
  {
    // Data processing reg, multiply, bx, memory transfer half/byte, swap,
    // PSR reg
    case 0x0:
    {
      if(op_bits_cmp(8, 0x1FFFF, 0x12FFF))
      {
        // bx
        sprintf(buffer, "bx%s %s", condition_str, reg_op(0));
      }
      else

      if(op_bits_cmp(4, 0x9, 0x9))
      {
        if(op_bits_cmp(5, 0x3, 0x0))
        {
          const char *s_bit_str = "";

          if(op_bit(20))
            s_bit_str = "s";

          // multiply, swap
          switch(op_bits(21, 0xF))
          {
            // mul
            case 0:
              sprintf(buffer, "mul%s%s %s, %s, %s", condition_str,
               s_bit_str, reg_op(16), reg_op(0), reg_op(8));
              break;

            // mla
            case 1:
              sprintf(buffer, "mla%s%s %s, %s, %s, %s", condition_str,
               s_bit_str, reg_op(16), reg_op(0), reg_op(8), reg_op(12));
              break;

            // long multiply
            case 4:
            case 5:
            case 6:
            case 7:
              sprintf(buffer, "%s%s%s %s, %s, %s, %s",
               long_multiply_type[op_bits(21, 0xF) - 4], condition_str,
               s_bit_str, reg_op(12), reg_op(16), reg_op(0), reg_op(8));
              break;

            // swp
            case 8:
              sprintf(buffer, "swp%s %s, %s, [%s]",
               condition_str, reg_op(16), reg_op(0), reg_op(12));
              break;

            // swpb
            case 10:
              sprintf(buffer, "swpb%s %s, %s, [%s]",
               condition_str, reg_op(16), reg_op(0), reg_op(12));
              break;

            default:
              sprintf(buffer, "undefined");
              break;
          }
        }
        else
        {
          // halfword/byte memory transfer
          char op2[16];
          if(op_bit(22))
          {
            u32 imm = op_bits(0, 0xF) | (op_bits(8, 0xF) << 4);
            sprintf(op2, "#%d", imm);
          }
          else
          {
            sprintf(op2, "%s", reg_op(0));
          }
          disasm_arm_memory_transfer2(opcode, op2, buffer);
        }
      }
      else
      {
        if(op_bits_cmp(23, 0x3, 0x2) && op_bits_cmp(20, 0x1, 0x0))
        {
          // PSR reg
          if(op_bit(21))
          {
            char reg_op2[16];
            disasm_arm_reg_operand2(op_bits(0, 0xFFF), reg_op2);

            // msr
            sprintf(buffer, "msr%s %s_%s, %s", condition_str,
             psr_op_type[op_bit(22)], psr_flags[op_bits(16, 0xF)],
             reg_op2);
          }
          else
          {
            // mrs
            sprintf(buffer, "mrs%s %s, %s", condition_str,
             reg_names[op_bits(12, 0xF)], psr_op_type[op_bit(22)]);
          }
        }
        else
        {
          char reg_op2[16];
          disasm_arm_reg_operand2(op_bits(0, 0xFFF), reg_op2);

          // data processing reg
          disasm_arm_data_proc(opcode, reg_op2, buffer);
        }
      }
      break;
    }

    // Data processing imm, PSR imm
    case 0x1:
    {
      char imm_op[16];
      disasm_arm_imm_operand2(opcode, imm_op);

      if(op_bits_cmp(23, 0x3, 0x2) && op_bits_cmp(20, 0x1, 0x0))
      {
        // PSR imm
        sprintf(buffer, "msr%s %s_%s, %s", condition_str,
         psr_op_type[op_bit(22)], imm_op);
      }
      else
      {
        // data processing imm
        disasm_arm_data_proc(opcode, imm_op, buffer);
      }
      break;
    }

    // Memory transfer imm
    case 0x2:
    {
      char imm_op[16];
      sprintf(imm_op, "#%d", op_bits(0, 0xFFF));

      disasm_arm_memory_transfer(opcode, imm_op, buffer);
      break;
    }

    // Memory transfer reg, undefined
    case 0x3:
    {
      if(op_bit(4))
      {
        sprintf(buffer, "undefined");
      }
      else
      {
        char reg_op[16];

        disasm_arm_mem_reg_operand2(opcode, reg_op);
        disasm_arm_memory_transfer(opcode, reg_op, buffer);
      }
      break;
    }

    // Block memory transfer
    case 0x4:
    {
      const char *base_type = block_memory_table[op_bits(23, 0x3)];
      const char *s_bit_str = "";
      const char *writeback_str = "";
      const char *memory_type = block_memory_op_type[op_bit(20)];
      char rlist_str[256];

      if(op_bit(21))
        writeback_str = "!";

      if(op_bit(22))
        s_bit_str = "^";

      get_rlist(opcode, rlist_str);

      sprintf(buffer, "%s%s%s %s%s, %s%s", memory_type, condition_str,
       base_type, reg_names[op_bits(16, 0xF)], writeback_str, rlist_str,
       s_bit_str);

      break;
    }

    // Branch
    case 0x5:
    {
      u32 target_pc =
       pc + 8 + ((((s32)(op_bits(0, 0xFFFFFF) << 8)) >> 8) * 4);
      const char *link_str = "";
      u32 i;

      if(op_bit(24))
        link_str = "l";

      for(i = 0; i < num_labels; i++)
      {
        if((u32)labels[i].address == target_pc)
        {
          sprintf(buffer, "b%s%s %s", link_str, condition_str,
           labels[i].name);
          break;
        }
      }

      if(i == num_labels)
        sprintf(buffer, "b%s%s %x", link_str, condition_str, target_pc);

      break;
    }

    // Coprocessor
    case 0x6:
    {
      sprintf(buffer, "[implement me]");
      break;
    }

    // Coprocessor, SWI
    case 0x7:
    {
      if(op_bit(24))
      {
        sprintf(buffer, "swi%s 0x%x", condition_str, op_bits(0, 0xFFFFFF));
      }
      else
      {
        sprintf(buffer, "[implement me]");
      }
      break;
    }
  }
}

#define reg_op_t(offset)                                                      \
  (reg_names[op_bits(offset, 0x7)])                                           \

const char *t_add_sub_table[] = { "add", "sub" };
const char *t_alu_op_imm_table[] = { "mov", "cmp", "add", "sub" };

const char *t_alu_op_table[] =
{
  "and", "eor", "lsl", "lsr", "asr", "adc", "sbc", "ror",
  "tst", "neg", "cmp", "cmn", "orr", "mul", "bic", "mvn"
};

const char *t_alu_op_hi_table[] = { "add", "cmp", "mov", "bx" };
const char *t_mem_op_reg_table[] = { "str", "strb", "ldr", "ldrb" };
const char *t_mem_op2_reg_table[] = { "strh", "ldsb", "ldrh", "ldsh" };
const char *t_mem_op_imm_table[] = { "str", "ldr", "strb", "ldrb" };
const char *t_mem_op_table[] = { "str", "ldr" };
const char *t_mem_op2_table[] = { "strh", "ldrh" };
const char *t_bmem_op_table[] = { "stm", "ldm" };


void disasm_thumb_instruction(u32 opcode, char *buffer, u32 pc,
 disasm_label *labels, u32 num_labels)
{
  switch(op_bits(13, 0x7))
  {
    case 0x0:
    {
      // shift, add/sub
      u32 sub_op = op_bits(11, 0x3);

      switch(sub_op)
      {
        case 0:
        case 1:
        case 2:
          // shift
          sprintf(buffer, "%s %s, %s, #%d", shift_table[sub_op],
           reg_op_t(0), reg_op_t(3), op_bits(6, 0x1F));
          break;

        case 3:
          // add/sub
          sub_op = op_bit(9);

          if(op_bit(10))
          {
            // add/sub imm
            u32 imm = op_bits(6, 0x7);
            if(imm)
            {
              sprintf(buffer, "%s %s, %s, #%d", t_add_sub_table[sub_op],
               reg_op_t(0), reg_op_t(3), imm);
            }
            else
            {
              sprintf(buffer, "mov %s, %s", reg_op_t(0), reg_op_t(3));
            }
          }
          else
          {
            sprintf(buffer, "%s %s, %s, %s", t_add_sub_table[sub_op],
             reg_op_t(0), reg_op_t(3), reg_op_t(6));
          }
          break;
      }
      break;
    }

    case 0x1:
      // alu op immediate
      sprintf(buffer, "%s %s, #%d", t_alu_op_imm_table[op_bits(11, 0x3)],
       reg_op_t(8), op_bits(0, 0xFF));
      break;

    case 0x2:
      // alu op/hireg op/memory op reg
      switch(op_bits(10, 0x7))
      {
        case 0:
          // alu op
          sprintf(buffer, "%s %s, %s", t_alu_op_table[op_bits(6, 0xF)],
           reg_op_t(0), reg_op_t(3));
          break;

        case 1:
        {
          // hireg op
          u32 sub_op = op_bits(8, 0x3);

          if(sub_op < 3)
          {
            sprintf(buffer, "%s %s, %s", t_alu_op_hi_table[sub_op],
             reg_names[op_bits(0, 0x7) | (op_bit(7) << 3)],
             reg_op(3));
          }
          else
          {
            sprintf(buffer, "%s %s", t_alu_op_hi_table[sub_op],
             reg_op(3));
          }

          break;
        }

        case 2:
        case 3:
          // ldr pc relative
          sprintf(buffer, "ldr %s, [pc, #%d]", reg_op_t(8),
           op_bits(0, 0xFF));
          break;

        case 4:
        case 5:
        case 6:
        case 7:
          // ldr/str reg
          if(op_bit(9))
          {
            sprintf(buffer, "%s %s, [%s, %s]",
             t_mem_op2_reg_table[op_bits(10, 0x3)], reg_op_t(0),
             reg_op_t(3), reg_op_t(6));
          }
          else
          {
            sprintf(buffer, "%s %s, [%s, %s]",
             t_mem_op_reg_table[op_bits(10, 0x3)], reg_op_t(0),
             reg_op_t(3), reg_op_t(6));
          }
          break;
      }
      break;

    case 0x3:
    {
      // memory op imm
      u32 offset = op_bits(6, 0x1F);

      if(op_bit(12) == 0)
        offset *= 4;

      sprintf(buffer, "%s %s, [%s, #%d]",
       t_mem_op_imm_table[op_bits(11, 0x3)], reg_op_t(0), reg_op_t(3),
       offset);
      break;
    }

    case 0x4:
      // halfword memory op imm/offset sp
      if(op_bit(12))
      {
        // load/store sp relative
        sprintf(buffer, "%s %s, [sp, #%d]", t_mem_op_table[op_bit(11)],
         reg_op_t(8), op_bits(0, 0xFF) * 4);
      }
      else
      {
        // load/store halfword
        sprintf(buffer, "%s %s, [%s, #%d]",
         t_mem_op2_table[op_bit(11)], reg_op_t(0), reg_op_t(3),
         op_bits(6, 0x1F) * 2);
      }
      break;

    case 0x5:
      // add pc/sp/push/pop
      if(op_bit(12))
      {
        if(op_bits_cmp(8, 0xF, 0x0))
        {
          // add sp, imm
          sprintf(buffer, "add sp, #%d", op_bits(0, 0x7F) * 4);
        }
        else
        {
          // push/pop
          u32 rlist = op_bits(0, 0xFF);
          char rlist_str[256];
          const char *op_type = NULL;

          if(op_bit(11))
          {
            // pop
            if(op_bit(8))
              rlist |= (1 << 15);

            op_type = "pop";
          }
          else
          {
            // push
            if(op_bit(8))
              rlist |= (1 << 14);

            op_type = "push";
          }

          get_rlist(rlist, rlist_str);
          sprintf(buffer, "%s %s", op_type, rlist_str);
        }
      }
      else
      {
        // get relative address
        const char *rm_str = "pc";

        if(op_bit(11))
          rm_str = "sp";

        sprintf(buffer, "add %s, %s, #%d", reg_op_t(8),
         rm_str, op_bits(0, 0xFF) * 4);
      }
      break;

    case 0x6:
      // stm/ldm/b(cond)/undef/swi
      if(op_bit(12))
      {
        // conditional branch/swi
        u32 cond = op_bits(8, 0xF);
        if(cond < 0xF)
        {
          sprintf(buffer, "b%s %x", condition_table[cond],
           pc + 4 + (((s32)(op_bits(0, 0xFF) << 24) >> 24) * 2));
        }
        else
        {
          sprintf(buffer, "swi %d", op_bits(0, 0xFF));
        }
      }
      else
      {
        // load/store multiple
        u32 rlist = op_bits(0, 0xFF);
        char rlist_str[256];

        get_rlist(rlist, rlist_str);

        sprintf(buffer, "%sia %s!, %s", t_bmem_op_table[op_bit(11)],
         reg_op_t(8), rlist_str);
      }
      break;

    case 0x7:
      // b/bl/undef
      switch(op_bits(11, 0x3))
      {
        case 0:
          sprintf(buffer, "b %x",
           pc + 4 + (((s32)(op_bits(0, 0x3FF) << 22) >> 22) * 2));
          break;

        case 2:
          sprintf(buffer, "bll %x", op_bits(0, 0x7FF));
          break;

        case 3:
          sprintf(buffer, "blh %x", op_bits(0, 0x7FF));
          break;
      }
      break;
  }
}

/*

int main()
{
  char buffer[1024];
  int opcode;
  int pc = 0;

  while(1)
  {
    if(scanf("%x", &opcode) == EOF)
      break;

    disasm_arm_instruction(opcode, buffer, pc);
    printf("%08x\t%08x\t%s\n", pc, opcode, buffer);
    pc += 4;
  }

  return 0;
}

*/
