/* Target Code for flare32
   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */
#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "df.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "expr.h"
#include "builtins.h"
#include "explow.h"
#include "print-rtl.h"

#include "flare32-protos.h"

__attribute__ ((used)) void
flare32_dump_file_debug_rtx (FILE* file, const_rtx x)
{
  rtx_writer w (file, 0, false, false, NULL);
  w.print_rtx (x);
  fprintf (file, "\n");
}
/* This file should be included last.  */
#include "target-def.h"

//#define FLARE32_DEBUG
//#define FLARE32_CONSTRAINT_DEBUG

#ifdef FLARE32_DEBUG
#define flare32_debug_fprintf(...) \
  do { fprintf (__VA_ARGS__); } while (0)
#define flare32_debug_rtx(...) \
  do { debug_rtx (__VA_ARGS__); } while (0)
#else
#define flare32_debug_fprintf(...) do { } while (0)
#define flare32_debug_rtx(...) do { } while (0)
#endif

#ifdef FLARE32_CONSTRAINT_DEBUG
#define flare32_constraint_debug_fprintf(...) \
  do { fprintf (__VA_ARGS__); } while (0)
#define flare32_constraint_debug_rtx(...) \
  do { debug_rtx (__VA_ARGS__); } while (0)
#else
#define flare32_constraint_debug_fprintf(...) do { } while (0)
#define flare32_constraint_debug_rtx(...) do { } while (0)
#endif

// Per-function machine data.
struct GTY (()) machine_function
{
  /* Number of bytes saved on the stack for callee saved registers. */
  int callee_saved_reg_size;

  /* Number of bytes saved on the stack for local variables.
    Includes padding for alignment once.  */
  int local_vars_size;

  /* Number of bytes of arguments passed onto the stack. */
  int stack_args_size;

  HOST_WIDE_INT outgoing_args_size;

  /* The sum of 2 sizes: locals vars and padding byte for saving the
    registers.  Used in expand_prologue () and expand_epilogue (). */
  int size_for_adjusting_sp;

  /* Padding for alignment */
  //int
  //  //padding_stack_args,
  //  //padding_locals;

  /* # of bytes of static stack space allocated by the function. */
  int static_stack_size;
};

static constexpr int STACK_ALIGNMENT = STACK_BOUNDARY / BITS_PER_UNIT;

#define FLARE32_STACK_ALIGN(LOC) \
  (ROUND_UP ((LOC), STACK_ALIGNMENT))

static void
update_stack_args_size (struct machine_function* self)
{
  // Padding needed for each element of the frame.
  self->stack_args_size = crtl->args.pretend_args_size;

  // Arguments on the stack should already be aligned?
  //// Align to the stack alignment
  //int padding_stack_args = self->stack_args_size % STACK_ALIGNMENT;

  //if (padding_stack_args)
  //{
  //  padding_stack_args = STACK_ALIGNMENT 
  //      - padding_stack_args;
  //}

  //self->stack_args_size += padding_stack_args;
  //flare32_debug_fprintf (stderr, "update_stack_args_size (): %i\n",
  //  self->stack_args_size);
}

static void
update_local_vars_size (struct machine_function* self)
{
  // Padding needed for each element of the frame.
  self->local_vars_size = FLARE32_STACK_ALIGN
    (HOST_WIDE_INT (get_frame_size ()));

  // Align to the stack alignment
  //int padding_locals = self->local_vars_size % STACK_ALIGNMENT;

  //if (padding_locals)
  //{
  //  padding_locals = STACK_ALIGNMENT - padding_locals;
  //}

  //self->local_vars_size += padding_locals;
  //flare32_debug_fprintf (stderr, "update_local_vars_size (): %i\n",
  //  self->local_vars_size);
}

static void
update_callee_saved_reg_size (struct machine_function* self)
{
  self->callee_saved_reg_size = 0;

  // Save callee-saved registers
  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
  {
    if (flare32_regno_actually_callee_saved (regno))
    //if (flare32_regno_actually_callee_saved_no_hfp (regno))
    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    {
      self->callee_saved_reg_size += UNITS_PER_WORD;
    }
  }
  //flare32_debug_fprintf (stderr, "update_callee_saved_reg_size (): %i\n",
  //  self->callee_saved_reg_size);
}

static void
update_outgoing_args_size (struct machine_function* self)
{
  self->outgoing_args_size
    = ACCUMULATE_OUTGOING_ARGS
      ? HOST_WIDE_INT (crtl->outgoing_args_size)
      : 0;
  //flare32_debug_fprintf (stderr, "update_outgoing_args_size (): %i\n",
  //  self->outgoing_args_size);
}

static void
update_size_for_adjusting_sp (struct machine_function* self)
{
  //machine.size_for_adjusting_sp = crtl->args.pretend_args_size
  //  + machine.local_vars_size
  //  + (ACCUMULATE_OUTGOING_ARGS
  //  ? static_cast<HOST_WIDE_INT>(crtl->outgoing_args_size) : 0);

  //size_for_adjusting_sp = stack_args_size + UNITS_PER_WORD
  //  + self->callee_saved_reg_size
  //  + self->local_vars_size
  //  + (ACCUMULATE_OUTGOING_ARGS
  //  ? HOST_WIDE_INT(crtl->outgoing_args_size) : 0);
  //self->size_for_adjusting_sp
  //  = self->local_vars_size
  //  + (ACCUMULATE_OUTGOING_ARGS
  //    ? HOST_WIDE_INT(crtl->outgoing_args_size)
  //    : 0);
  //self->size_for_adjusting_sp
  //  = self->local_vars_size
  //  + self->outgoing_args_size;
  self->size_for_adjusting_sp
    = //crtl->args.pretend_args_size
    //+
    self->local_vars_size
    + self->outgoing_args_size;
  //flare32_debug_fprintf (stderr, "update_size_for_adjusting_sp (): %i\n",
  //  self->size_for_adjusting_sp);
}
static void
update_static_stack_size (struct machine_function* self)
{
  self->static_stack_size
    = //self->stack_args_size
    //+
    (frame_pointer_needed ? UNITS_PER_WORD : 0)
    + self->callee_saved_reg_size
    + self->local_vars_size
    + self->outgoing_args_size;
  //flare32_debug_fprintf (stderr, "update_static_stack_size (): %i\n",
  //  self->static_stack_size);
}

//static inline int
//fp_offset_from_old_sp (struct machine_function* self) const
//{
//  return (-self->stack_args_size);
//}

//static inline int
//ap_offset_from_fp (struct machine_function* self)
//{
//  return (self->stack_args_size + UNITS_PER_WORD);
//}

static void
flare32_compute_frame ()
{
  update_stack_args_size (cfun->machine);
  update_local_vars_size (cfun->machine);
  update_callee_saved_reg_size (cfun->machine);
  update_outgoing_args_size (cfun->machine);
  update_size_for_adjusting_sp (cfun->machine);
  update_static_stack_size (cfun->machine);
}

#define LOSE_AND_RETURN(msgid, x) \
do \
{ \
  flare32_operand_lossage (msgid, x); \
  return; \
} while (0)

//static void
//print_poly_int (FILE *file, poly_int64 x)
//{
//  HOST_WIDE_INT const_x;
//  if (x.is_constant (&const_x))
//    fprintf (file, HOST_WIDE_INT_PRINT_DEC, const_x);
//  else
//    {
//      fprintf (file, "[" HOST_WIDE_INT_PRINT_DEC, x.coeffs[0]);
//      for (int i = 1; i < NUM_POLY_INT_COEFFS; ++i)
//        fprintf (file, ", " HOST_WIDE_INT_PRINT_DEC, x.coeffs[i]);
//      fprintf (file, "]");
//    }
//}
//static void
//flare32_dbg_print_rtx (rtx x)
//{
//  if (GET_CODE (x) == REG)
//  {
//    fprintf (stderr, "%s:%s:%d",
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)),
//      REGNO (x));
//  }
//  else if (GET_CODE (x) == SUBREG)
//  {
//    //fprintf (stderr, "SUBREG:{%d, %d, %d}",
//    //  REGNO (SUBREG_REG (x)),
//    //  GET_MODE (x) == SImode,
//    //  GET_MODE_SIZE (GET_MODE (x)));
//    //fprintf (stderr, "%s:%s",
//    //  GET_RTX_NAME (GET_CODE (x)),
//    //  GET_MODE_NAME (GET_MODE (x)));
//    //fprintf (stderr,
//    //  //"%s:%s:%s:%d:%d:%d:%d:",
//    //  "%s:%s:%s:%d:%d:%d:%",
//    //  //"%s:%s:%d:%d",
//    //  GET_RTX_NAME (GET_CODE (x)),
//    //  GET_MODE_NAME (GET_MODE (x)),
//    //  GET_MODE_NAME (GET_MODE (SUBREG_REG (x))),
//    //  REGMODE_NATURAL_SIZE (GET_MODE (x)),
//    //  REGNO (SUBREG_REG (x)),
//    //  REGMODE_NATURAL_SIZE (GET_MODE (SUBREG_REG (x)))
//    //  //paradoxical_subreg_p (x)
//    //  //SUBREG_PROMOTED_UNSIGNED_P (x)
//    //  //SUBREG_BYTE (x)
//    //  );
//    //print_poly_int (stderr, SUBREG_BYTE (x));
//    fprintf (stderr,
//      "%s:%s[",
//      GET_RTX_NAME (GET_CODE (x)),
//      GET_MODE_NAME (GET_MODE (x)));
//    flare32_dbg_print_rtx (SUBREG_REG (x));
//    //fprintf (stderr, ":%d",
//    //  SUBREG_BYTE (x));
//    fprintf (stderr, "]:");
//    print_poly_int (stderr, SUBREG_BYTE (x));
//  }
//  else if (flare32_absolute_ex (x))
//  {
//    fprintf (stderr, "%s:%s",
//      GET_RTX_NAME (GET_CODE (x)),
//      GET_MODE_NAME (GET_MODE (x)));
//  }
//  else if (GET_CODE (x) == MEM)
//  {
//    fprintf (stderr, "%s:%s[",
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)));
//    flare32_dbg_print_rtx (XEXP (x, 0));
//    fprintf (stderr, "]");
//  }
//  else if (GET_CODE (x) == PLUS)
//  {
//    rtx
//      left = XEXP (x, 0),
//      right = XEXP (x, 1);
//    fprintf (stderr, "(%s:%s ",
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)));
//    flare32_dbg_print_rtx (left);
//    fprintf (stderr, " ");
//    flare32_dbg_print_rtx (right);
//    fprintf (stderr, ")");
//  }
//  else
//  {
//    fprintf (stderr, "%s:%s",
//      //unsigned(GET_CODE (x)),
//      GET_RTX_NAME (GET_CODE (x)),
//      //GET_MODE (x) == SImode,
//      //GET_MODE_SIZE (GET_MODE (x))
//      GET_MODE_NAME (GET_MODE (x)));
//  }
//}

//static constexpr int MIN_SIMM18 = -131072;
//static constexpr int MAX_SIMM18 = 131071;

// Define how to find the value returned by a function.
// VALTYPE is the data type of the value (as a tree).
// If the precise function being called is known, FUNC is its
// FUNCTION_DECL; otherwise, FUNC is 0.
//

// We always return values in register r0 for flare32.
// This may need to be adjusted this for DImode?
static rtx
flare32_function_value (const_tree valtype,
  const_tree fntype_or_decl ATTRIBUTE_UNUSED,
  bool outgoing ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (TYPE_MODE (valtype), FLARE32_R0);
}

// Define how to find the value returned by a library function.
//
// We always return values in register r0 for flare32.
// This may need to be adjusted this for DImode?
static rtx
flare32_libcall_value (machine_mode mode,
  const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, FLARE32_R0);
}

// Handle TARGET_FUNCTION_VALUE_REGNO_P.
//
// We always return values in register r0 for flare32.
// This may need to be adjusted this for DImode?
static bool
flare32_function_value_regno_p (const unsigned int regno)
{
  return (regno == FLARE32_R0);
}

//static rtx
//flare32_static_chain (const_tree ARG_UNUSED (fndecl_or_type),
//  bool incoming_p)
//{
//  rtx addr, mem;
//
//  if (incoming_p)
//  {
//    addr = plus_constant (Pmode, arg_pointer_rtx, 2 * UNITS_PER_WORD);
//  }
//  else
//  {
//    addr = plus_constant (Pmode, stack_pointer_rtx, -UNITS_PER_WORD);
//  }
//
//  mem = gen_rtx_MEM (Pmode, addr);
//  MEM_NOTRAP_P (mem) = 1;
//
//  return mem;
//}

/* Worker function for TARGET_ASM_TRAMPOLINE_TEMPLATE.  */

//static void
//flare32_asm_trampoline_template (FILE *f)
//{
//  //fprintf (f, "\tpush  $sp, $r0\n");
//  //fprintf (f, "\tldi.l $r0, 0x0\n");
//  //fprintf (f, "\tsto.l 0x8($fp), $r0\n");
//  //fprintf (f, "\tpop   $sp, $r0\n");
//  //fprintf (f, "\tjmpa  0x0\n");
//  fprintf (f, "\tpush r0\n");
//  //fprintf (f, "\tcpy r0, #0x0\n");
//  fprintf (f, "\txor r0, r0\n");
//  //fprintf (f, "\tstr r0, [fp, #0x8]\n");
//  fprintf (f, "\tpop r0\n");
//  fprintf (f, "\tbra.l 0x0\n");
//}

/* Worker function for TARGET_TRAMPOLINE_INIT.  */

//static void
//flare32_trampoline_init (rtx m_tramp, tree fndecl, rtx chain_value)
//{
//  //rtx mem, fnaddr = XEXP (DECL_RTL (fndecl), 0);
//
//  //emit_block_move (m_tramp, assemble_trampoline_template (),
//  //                GEN_INT (TRAMPOLINE_SIZE), BLOCK_OP_NORMAL);
//
//  //mem = adjust_address (m_tramp, SImode, 4);
//  //emit_move_insn (mem, chain_value);
//  //mem = adjust_address (m_tramp, SImode, 16);
//  //emit_move_insn (mem, fnaddr);
//}

// Emit an error message when we're in an asm, and a fatal error for
// "normal" insns.  Formatted output isn't easily implemented, since we
// use output_operand_lossage to output the actual message and handle the
// categorization of the error.
static void
flare32_operand_lossage (const char *msgid, rtx op)
{
  debug_rtx (op);
  output_operand_lossage ("%s", msgid);
}


static bool
flare32_valid_base_reg_p (rtx reg, bool strict_p)
{
  //const int regno = REGNO (reg);
  bool ret = false;
  flare32_debug_fprintf (stderr,
    "flare32_valid_base_reg_p () start: %d\n",
    int(strict_p));
  flare32_debug_rtx (reg);

  if (!strict_p && GET_CODE (reg) == SUBREG)
  {
    //reg = SUBREG_REG (reg);
    reg
      = simplify_gen_subreg (SImode, reg, DImode,
        subreg_lowpart_offset (SImode, DImode));
    flare32_debug_fprintf (stderr, "!strict_p && SUBREG:\n");
    flare32_debug_rtx (reg);
  }

  //if (GET_MODE (reg) == Pmode)
  if (REG_P (reg)
    //|| GET_CODE (reg) == SUBREG
  )
  {
    if (strict_p)
    {
      flare32_debug_fprintf (stderr, "REG_P with strict_p: %d:%d:%d:%d\n",
        REGNO (reg),
        reg_renumber[REGNO (reg)],
        HARD_REGNO_OK_FOR_BASE_P (REGNO (reg)),
        HARD_REGNO_OK_FOR_BASE_P (reg_renumber[REGNO (reg)]));
      ret = (HARD_REGNO_OK_FOR_BASE_P (REGNO (reg))
        || HARD_REGNO_OK_FOR_BASE_P (reg_renumber[REGNO (reg)]));
    }
    else
    {
      //if (GET_CODE (reg) == SUBREG)
      //{
      //  reg = SUBREG_REG (reg);
      //}
      flare32_debug_fprintf (stderr, "REG_P with !strict_p: %d:%d:%d\n",
        REGNO (reg),
        !HARD_REGISTER_NUM_P (REGNO (reg)),
        (
          //!HARD_REGISTER_NUM_P (REGNO (reg))
          //? 0
          //:
          HARD_REGNO_OK_FOR_BASE_P (REGNO (reg))
        )
      );

      ret = (!HARD_REGISTER_NUM_P (REGNO (reg))
        || HARD_REGNO_OK_FOR_BASE_P (REGNO (reg)));
    }
  }
  flare32_debug_fprintf (stderr, "flare32_valid_base_reg_p () end: %d\n",
    int(ret));
  flare32_debug_fprintf (stderr, "\n");

  return ret;
}

static bool
flare32_valid_index_reg_p (rtx reg, bool strict_p)
{
  //const int regno = REGNO (reg);

  //if (GET_MODE (reg) == Pmode)
  //{
  //  if (strict_p)
  //  {
  //    return (HARD_REGNO_OK_FOR_INDEX_P (regno)
  //      || HARD_REGNO_OK_FOR_INDEX_P (reg_renumber[regno]));
  //  }
  //  else
  //  {
  //    return (!HARD_REGISTER_NUM_P (regno)
  //      || HARD_REGNO_OK_FOR_INDEX_P (regno));
  //  }
  //}

  if (!strict_p && GET_CODE (reg) == SUBREG)
  {
    //reg = SUBREG_REG (reg);
    reg
      = simplify_gen_subreg (SImode, reg, DImode,
        subreg_lowpart_offset (SImode, DImode));
  }

  if (REG_P (reg)
    //|| GET_CODE (reg) == SUBREG
    )
  {
    if (strict_p)
    {
      return (HARD_REGNO_OK_FOR_INDEX_P (REGNO (reg))
        || HARD_REGNO_OK_FOR_INDEX_P (reg_renumber[REGNO (reg)]));
    }
    else
    {
      //if (GET_CODE (reg) == SUBREG)
      //{
      //  reg = SUBREG_REG (reg);
      //}
      return (!HARD_REGISTER_NUM_P (REGNO (reg))
        || HARD_REGNO_OK_FOR_INDEX_P (REGNO (reg)));
    }
  }
  return false;
}

//extern rtx_insn *curr_insn;
static bool
flare32_legitimate_address_p (machine_mode mode, rtx x, bool strict_p,
  addr_space_t as)
{
  gcc_assert (ADDR_SPACE_GENERIC_P (as));

  flare32_debug_fprintf (stderr,
    "flare32_legitimate_address_p (): begin: %d\n",
    unsigned(GET_MODE_SIZE (mode)));
  // --------
  // DImode
  //if (GET_MODE_SIZE (mode) > UNITS_PER_WORD)
  //{
  //}
  //if (GET_MODE_SIZE (mode) > UNITS_PER_WORD)
  //{
  //  flare32_debug_fprintf (stderr, 
  //    "flare32_legitimate_address_p (): %i\n",
  //    GET_MODE_SIZE (mode));
  //}
  // SImode, DImode
  //else
  if (GET_MODE_SIZE (mode) >= UNITS_PER_WORD)
  //if (GET_MODE_SIZE (mode) == UNITS_PER_WORD)
  {
    if (GET_MODE_SIZE (mode) > UNITS_PER_WORD)
    {
      flare32_debug_fprintf (stderr,
        "flare32_legitimate_address_p (): dbg 0: %s\n",
        GET_MODE_NAME (mode));
      //flare32_debug_rtx (curr_insn);
      flare32_debug_rtx (x);
      flare32_debug_fprintf (stderr, "\n");
    }
    if (GET_CODE (x) == PLUS
      //&& GET_CODE (XEXP (x, 0)) == REG
      //&& flare32_valid_base_reg_p (XEXP (x, 0), strict_p)
    )
    {
      rtx
        left_0 = XEXP (x, 0),
        right_0 = XEXP (x, 1);
      flare32_debug_fprintf (stderr,
        "flare32_legitimate_address_p (): plus:\n");
      flare32_debug_rtx (left_0);
      flare32_debug_rtx (right_0);
      if (
        //GET_CODE (left_0) == REG
          //&& 
          flare32_valid_base_reg_p (left_0, strict_p)
        && (
          (
            //GET_CODE (right_0) == REG
              //&&
              flare32_valid_index_reg_p (right_0, strict_p)
          ) || flare32_absolute_ex (right_0)
        )
      )
      {
        if (!flare32_absolute_ex (right_0))
        {
          flare32_debug_fprintf (stderr,
            "flare32_legitimate_address_p (): "
              "(32 or >32): base + index: %s\n",
            GET_MODE_NAME (mode));
        }
        else
        {
          flare32_debug_fprintf (stderr,
            "flare32_legitimate_address_p (): "
              "(32 or >32): base + imm: %s\n",
            GET_MODE_NAME (mode));
        }
        //flare32_debug_rtx (x);
        //flare32_debug_fprintf (stderr, "\n");
        return true;
      }
      //if (
      //  GET_CODE (left_0) == PLUS
      //  && GET_CODE (XEXP (left_0, 0)) == REG
      //    && flare32_valid_base_reg_p (XEXP (left_0, 0), strict_p)
      //  && GET_CODE (XEXP (left_0, 1)) == REG
      //    && flare32_valid_index_reg_p (XEXP (left_0, 1), strict_p)
      //  && flare32_absolute_ex (right_0)
      //)
      //{
      //  return true;
      //}

    }
    //else if (GET_CODE == REG)
    //{
    //}
  }
  // QImode, HImode
  else if (GET_MODE_SIZE (mode) < UNITS_PER_WORD)
  //else
  {
    if (GET_CODE (x) == PLUS
      && GET_CODE (XEXP (x, 0)) == REG
        && flare32_valid_base_reg_p (XEXP (x, 0), strict_p)
      && GET_CODE (XEXP (x, 1)) == REG
        && flare32_valid_index_reg_p (XEXP (x, 1), strict_p))
    {
      flare32_debug_fprintf (stderr,
        "flare32_legitimate_address_p (): (8 or 16) base + index: %s\n",
        GET_MODE_NAME (mode));
      flare32_debug_rtx (x);
      flare32_debug_fprintf (stderr, "\n");
      return true;
    }
  }
  // --------
  // no `index rA` instruction
  if (
    //(GET_CODE (x) == REG) &&
    flare32_valid_base_reg_p (x, strict_p))
  {
    flare32_debug_fprintf (stderr,
      "flare32_legitimate_address_p (): base reg: %s\n",
      GET_MODE_NAME (mode));
    //flare32_debug_rtx (curr_insn);
    flare32_debug_rtx (x);
    flare32_debug_fprintf (stderr, "\n");

    return true;
  }
  // --------
  if (flare32_absolute_ex (x))
  //if (flare32_absolute (x))
  {
    flare32_debug_fprintf (stderr,
      "flare32_legitimate_address_p (): absolute: %s\n",
      GET_MODE_NAME (mode));
    //flare32_debug_rtx (curr_insn);
    flare32_debug_rtx (x);
    return true;
  }
  // --------
  //flare32_debug_fprintf (stderr,
  //  "flare32_legitimate_address_p (): dbg: ");
  //flare32_debug_rtx (x);
  return false;
}

//static reg_class_t
//flare32_secondary_reload (bool in_p ATTRIBUTE_UNUSED,
//  rtx x ATTRIBUTE_UNUSED,
//  reg_class_t reload_class_as_t ATTRIBUTE_UNUSED,
//  machine_mode reload_mode ATTRIBUTE_UNUSED,
//  secondary_reload_info* sri)
//{
//  //const reg_class reload_class = reg_class (reload_class_as_t);
//
//  //const auto regno = true_regnum (x);
//
//  sri->icode = CODE_FOR_nothing;
//  sri->extra_cost = 0;
//
//  //if (flare32_reg_class_cpyspec (reload_class)
//  //  || (reload_class == HILO_REGS))
//  //{
//  //  if ((regno != -1) && ((GET_CODE (x) == REG)
//  //    || (GET_CODE (x) == SUBREG))
//  //    && (REGNO_REG_CLASS (regno) == GENERAL_REGS))
//  //  {
//  //    return NO_REGS;
//  //  }
//
//  //  sri->extra_cost = 1;
//  //  return GENERAL_REGS;
//  //}
//  ////else if (((GET_CODE (x) == REG) || (GET_CODE (x) == SUBREG))
//  ////  && flare32_reg_class_cpyspec (REGNO_REG_CLASS (regno)))
//  ////{
//  ////  if (reload_class != GENERAL_REGS)
//  ////  {
//  ////    return GENERAL_REGS;
//  ////  }
//  ////  return NO_REGS;
//  ////}
//  ////if (((GET_CODE (x) == REG) || (GET_CODE (x) == SUBREG))
//  ////  && flare32_reg_class_cpyspec (REGNO_REG_CLASS (regno)))
//  ////{
//  ////}
//
//  return NO_REGS;
//}


static bool
flare32_fixed_condition_code_regs
  (unsigned int *p1, unsigned int *p2)
{
  *p1 = FLARE32_FLAGS;
  *p2 = INVALID_REGNUM;
  return true;
}

//static reg_class_t flare32_target_spill_class (reg_class_t rc,
//  machine_mode mode)
//{
//  return reg_class_t (GENERAL_REGS);
//  //return NO_REGS;
//}


// The PRINT_OPERAND worker.

static void
flare32_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;

  int rgoff = 0;
  int the_regno = 0;
  switch (code)
  {
    case 0:
      // No code, print as usual
      break;

    case 'L':
      // Higher of two registers, print one up
      rgoff = 1;
      break;

    case 'R':
    case 'H':
      // Higher of a register pair, print normal
      rgoff = 0;
      break;

    default:
      //fprintf (stderr, "flare32_print_operand (): dbg 0\n");
      LOSE_AND_RETURN ("invalid operand modifier letter", x);
      break;
  }

  /* Print an operand as without a modifier letter.  */
  switch (GET_CODE (operand))
  {
    case REG:
      the_regno = REGNO (operand) + rgoff;
      if (the_regno >= FIRST_PSEUDO_REGISTER)
      {
        internal_error ("internal error: bad register: %d",
          the_regno);
      }
      fprintf (file, "%s", reg_names[the_regno]);
      break;

    case MEM:
      output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
      break;

    case CONST_DOUBLE:
      //if (CONST_DOUBLE_P (operand))
      //{
      //memcpy
      //}
      //fprintf (file, "%s");
    {
      //fprintf (file, "0x%lx%08lx", l[1] & 0xffffffff, l[0] & 0xffffffff);
      if (GET_MODE_SIZE (GET_MODE (operand)) == UNITS_PER_WORD)
      {
        long l;
        REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (x), l);
        fprintf (file, "0x%04lx",
          l & 0xffffffff);
      }
      else if (GET_MODE_SIZE (GET_MODE (operand)) > UNITS_PER_WORD)
      {
        long l[2];
        REAL_VALUE_TO_TARGET_DOUBLE (*CONST_DOUBLE_REAL_VALUE (x), l);
        fprintf (file, "0x%04lx, #0x%04lx",
          l[0] & 0xffffffff, l[1] & 0xffffffff);
      }
      else
      {
        LOSE_AND_RETURN ("unexpected operand", x);
      }
    }
      break;

    default:
      // No need to handle all strange variants, let
      // output_addr_const do it for us.
      if (CONSTANT_P (operand))
      {
        //fprintf (file, "#");
        output_addr_const (file, operand);
        return;
      }

      //fprintf (stderr, "flare32_print_operand (): dbg 1\n");

      LOSE_AND_RETURN ("unexpected operand", x);
  }

  // New code entries should just be added to the switch below.  If
  // handling is finished, just return.  If handling was just a
  // modification of the operand, the modified operand should be put in
  // "operand", and then do a break to let default handling
  // (zero-modifier) output the operand.

  //fprintf (stderr, "dbg: %i\n", code);
  //switch (code)
  //{
  //  case 0:
  //    /* No code, print as usual.  */
  //    break;

  //  case 'E':
  //    break;

  //  default:
  //    LOSE_AND_RETURN ("invalid operand modifier letter", x);
  //}

  ///* Print an operand as without a modifier letter.  */
  //switch (GET_CODE (operand))
  //{
  //  case REG:
  //    if (REGNO (operand) > FLARE32_SP)
  //    {
  //      internal_error ("internal error: bad register: %d",
  //        REGNO (operand));
  //    }
  //    fprintf (file, "%s", reg_names[REGNO (operand)]);
  //    return;

  //  case MEM:
  //    output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
  //    return;

  //  default:
  //    // No need to handle all strange variants, let output_addr_const
  //    //do it for us.
  //    if (CONSTANT_P (operand))
  //    {
  //      output_addr_const (file, operand);
  //      return;
  //    }

  //    LOSE_AND_RETURN ("unexpected operand", x);
  //}
}

static void
flare32_print_operand_address (FILE* file,
  machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  rtx left, right;
  switch (GET_CODE (x))
  {
  case REG:
    fprintf (file, "%s", reg_names[REGNO (x)]);
    break;

  case PLUS:
    left = XEXP (x, 0);
    right = XEXP (x, 1);

    // plus (plus (reg, reg), immediate)
    if (GET_CODE (left) == PLUS)
    {
      auto base = XEXP (left, 0);
      auto index = XEXP (left, 1);
      fprintf (file, "%s, %s, ", reg_names[REGNO (base)],
        reg_names[REGNO (index)]);
      if (GET_CODE (right) == CONST_INT)
      {
        fprintf (file, "#%ld", INTVAL (right));
      }
      //else if (GET_CODE (right) == CONST_DOUBLE)
      //{
      //}
      else if (
        //CONSTANT_P (right)
        flare32_absolute (right)
        )
      {
        fprintf (file, "#");
        output_addr_const (file, right);
      }
      else
      {
        gcc_unreachable ();
      }
    }
    // plus (reg, (reg or immediate))
    else
    {
      fprintf (file, "%s, ", reg_names[REGNO (left)]);

      if (GET_CODE (right) == REG)
      {
        fprintf (file, "%s", reg_names[REGNO (right)]);
      }
      else if (GET_CODE (right) == CONST_INT)
      {
        fprintf (file, "#%ld", INTVAL (right));
        //fprintf (file, "%ld", INTVAL (right));
      }
      else if (
        //CONSTANT_P (right)
        flare32_absolute (right)
        )
      {
        fprintf (file, "#");
        output_addr_const (file, right);
      }
      else
      {
        gcc_unreachable ();
      }
    }

    //switch (GET_CODE (right))
    //{
    //case CONST_INT:
    //  fprintf (file, "%s, %ld", reg_names[REGNO (left)],
    //    INTVAL (right));
    //  break;


    //case REG:
    //  fprintf (file, "%s, %s", reg_names[REGNO (left)],
    //    reg_names[REGNO (right)]);
    //  break;

    //case PLUS:
    //  fprintf (file, "%s, %s + ", reg_names[REGNO (left)],
    //    reg_names[REGNO (XEXP (right, 0))]);
    //  if (GET_CODE (XEXP (right, 1)) == CONST_INT)
    //  {
    //    fprintf (file, "%ld", INTVAL (XEXP (right, 1)));
    //  }
    //  else if (flare32_absolute (XEXP (right, 1)))
    //  {
    //    output_addr_const (file, XEXP (right, 1));
    //  }
    //  else
    //  {
    //    gcc_unreachable ();
    //  }
    //  break;

    //default:
    //  if (flare32_absolute (right))
    //  {
    //    fprintf (file, "%s, ", reg_names[REGNO (left)]);
    //    output_addr_const (file, right);
    //  }
    //  else
    //  {
    //    gcc_unreachable ();
    //  }
    //  break;
    //}
    break;

  //case LABEL_REF:
  //  fprintf (file, "<some_label_ref>");
  //  break;

  case LABEL_REF:
    output_addr_const (file, x);
    break;
  case SYMBOL_REF:
  case CONST:
  case CONST_INT:
    //fprintf (file, "<some_symbol_ref>");

    fprintf (file, "#");
    output_addr_const (file, x);
    break;

  //case CONST:
  //  fprintf (file, "<some_const>");
  //  break;

  default:
    gcc_unreachable ();
    break;
  }
}

//static reg_class_t
//flare32_preferred_reload_class (rtx x,
//  reg_class_t rclass)
//{
//  //fprintf (stderr, "flare32_preferred_reload_class (): %s",
//  //  reg_class_names[rclass]);
//  //if (REG_P (x))
//  //{
//  //  fprintf (stderr, " %d",
//  //    REGNO (x));
//  //}
//  //fprintf (stderr, ": ");
//  //debug_rtx (x);
//  return GENERAL_REGS;
//}

extern bool
register_operand (rtx op, machine_mode mode);

//static bool
//flare32_mov_reg_operand (machine_mode mode, rtx x)
//{
//  if (!register_operand (x, mode))
//  {
//    return false;
//  }
//
//  if (SUBREG_P (x))
//  {
//    x = SUBREG_REG (x);
//  }
//
//  if (!REG_P (x))
//  {
//    return 0;
//  }
//  if (!HARD_REGISTER_P (x))
//  {
//    return 1;
//  }
//  return (REGNO_REG_CLASS (REGNO (x)) == GENERAL_REGS);
//}

bool
flare32_32_bit_offset_address_reg_plus_reg_p (rtx x)
{
  bool ret = false;
  flare32_constraint_debug_fprintf (stderr,
    "(32-bit offset base + index) start:\n");
  flare32_constraint_debug_rtx (x);
  //flare32_constraint_debug_fprintf (stderr, "\n");

  //if (!MEM_P (x))
  //{
  //  return false;
  //}
  if (MEM_P (x))
  {
    // Inside the MEM
    x = XEXP (x, 0);

    if (GET_CODE (x) == PLUS)
    {
      rtx
        left_0 = XEXP (x, 0),
        right_0 = XEXP (x, 1);

      //#ifndef REG_OK_STRICT
      if (GET_CODE (left_0) == SUBREG)
      {
        //left_0 = SUBREG_REG (left_0);
        left_0
          = simplify_gen_subreg (SImode, left_0, DImode,
            subreg_lowpart_offset (SImode, DImode));
        flare32_constraint_debug_fprintf (stderr, "left_0 SUBREG:\n");
        flare32_constraint_debug_rtx (left_0);
      }
      if (GET_CODE (right_0) == SUBREG)
      {
        //right_0 = SUBREG_REG (right_0);
        right_0
          = simplify_gen_subreg (SImode, right_0, DImode,
            subreg_lowpart_offset (SImode, DImode));
        flare32_constraint_debug_fprintf (stderr, "right_0 SUBREG:\n");
        flare32_constraint_debug_rtx (right_0);
      }
      //#endif // REG_OK_STRICT
      flare32_constraint_debug_fprintf (stderr,
        //"before if (...) ret = true: %d:%d:%d:%d\n",
        "before if (...) ret = true: %d:%d\n",
        int(GET_CODE (left_0) == REG),
        //int(REGNO_OK_FOR_BASE_P (REGNO (left_0))),
        int(GET_CODE (right_0) == REG)
        //int(REGNO_OK_FOR_INDEX_P (REGNO (right_0)))
        );

      if (
        GET_CODE (left_0) == REG
          //&& REGNO_OK_FOR_BASE_P (REGNO (left_0))
        && GET_CODE (right_0) == REG
          //&& REGNO_OK_FOR_INDEX_P (REGNO (right_0))
      )
      {
        ret = true;
      }
    }
  }

  flare32_constraint_debug_fprintf (stderr, "(32-bit offset base + index) end: %d\n",
    int(ret));
  flare32_constraint_debug_fprintf (stderr, "\n");
  return ret;
}
bool
flare32_32_bit_offset_address_reg_plus_imm_p (rtx x)
{
  bool ret = false;
  flare32_constraint_debug_fprintf (stderr,
    "(32-bit offset base + imm) start:\n");
  flare32_constraint_debug_rtx (x);
  //flare32_constraint_debug_fprintf (stderr, "\n");

  //if (!MEM_P (x))
  //{
  //  return false;
  //}
  if (MEM_P (x))
  {
    // Inside the MEM
    x = XEXP (x, 0);

    if (GET_CODE (x) == PLUS)
    {
      rtx
        left_0 = XEXP (x, 0),
        right_0 = XEXP (x, 1);

      //#ifndef REG_OK_STRICT
      if (GET_CODE (left_0) == SUBREG)
      {
        //left_0 = SUBREG_REG (left_0);
        left_0
          = simplify_gen_subreg (SImode, left_0, DImode,
            subreg_lowpart_offset (SImode, DImode));
        flare32_constraint_debug_fprintf (stderr, "left_0 SUBREG:\n");
        flare32_constraint_debug_rtx (left_0);
      }
      //#endif // REG_OK_STRICT
      flare32_constraint_debug_fprintf (stderr,
        //"before if (...) ret = true: %d:%d:%d\n",
        "before if (...) ret = true: %d:%d\n",
        int(GET_CODE (left_0) == REG),
        //int(REGNO_OK_FOR_BASE_P (REGNO (left_0))),
        int(flare32_absolute_ex (right_0)));

      if (
          //GET_CODE (left_0) == REG
          ////&& GET_MODE (left_0) == Pmode
          ////&& REGNO_OK_FOR_BASE_P (REGNO (left_0))
          //!flare32_absolute_ex (left_0)
        //&&
        //flare32_absolute_ex (right_0)
        GET_CODE (left_0) == REG
          //&& REGNO_OK_FOR_BASE_P (REGNO (left_0))
        && flare32_absolute_ex (right_0)
      )
      {
        ret = true;
      }
    }
  }
  flare32_constraint_debug_fprintf (stderr,
    "(32-bit offset base + imm) end: %d\n",
    int(ret));
  flare32_constraint_debug_fprintf (stderr, "\n");
  return ret;
}
bool
flare32_32_bit_offset_address_reg_plus_reg_plus_imm_p (rtx x)
{
  bool ret = false;
  flare32_constraint_debug_fprintf (stderr,
    "(32-bit offset base + index + imm) start:\n");
  flare32_constraint_debug_rtx (x);
  //flare32_constraint_debug_fprintf (stderr, "\n");

  if (MEM_P (x))
  {
    // Inside the MEM
    x = XEXP (x, 0);

    if (GET_CODE (x) == PLUS
      //&& GET_CODE (XEXP (x, 0)) == REG
      //&& flare32_valid_base_reg_p (XEXP (x, 0), strict_p)
    )
    {
      rtx
        left_0 = XEXP (x, 0),
        right_0 = XEXP (x, 1);

      if (
        GET_CODE (left_0) == PLUS
        && flare32_absolute_ex (right_0)
      )
      {
        rtx
          left_1 = XEXP (left_0, 0),
          right_1 = XEXP (left_0, 1);
        //#ifndef REG_OK_STRICT
        if (GET_CODE (left_1) == SUBREG)
        {
          //left_1 = SUBREG_REG (left_1);
          left_1
            = simplify_gen_subreg (SImode, left_1, DImode,
              subreg_lowpart_offset (SImode, DImode));
          flare32_constraint_debug_fprintf (stderr, "left_1 SUBREG:\n");
          flare32_constraint_debug_rtx (left_1);
        }
        if (GET_CODE (right_1) == SUBREG)
        {
          //right_1 = SUBREG_REG (right_1);
          right_1
            = simplify_gen_subreg (SImode, right_1, DImode,
              subreg_lowpart_offset (SImode, DImode));
          flare32_constraint_debug_fprintf (stderr, "right_1 SUBREG:\n");
          flare32_constraint_debug_rtx (right_1);
        }
        //#endif  // REG_OK_STRICT

        flare32_constraint_debug_fprintf (stderr,
          //"before if (...) ret = true: %d:%d:%d:%d\n",
          "before if (...) ret = true: %d:%d\n",
          int(GET_CODE (left_1) == REG),
          //int(REGNO_OK_FOR_BASE_P (REGNO (left_1))),
          int(GET_CODE (right_1) == REG)
          //int(REGNO_OK_FOR_INDEX_P (REGNO (right_1)))
          );

        if (GET_CODE (left_1) == REG
            //&& REGNO_OK_FOR_BASE_P (REGNO (left_1))
          && GET_CODE (right_1) == REG
            //&& REGNO_OK_FOR_INDEX_P (REGNO (right_1))
          )
        {
          ret = true;
        }
      }
    }
  }
  flare32_constraint_debug_fprintf (stderr,
    "(32-bit offset base + index + imm) end: %d\n",
    int(ret));
  flare32_constraint_debug_fprintf (stderr, "\n");
  return ret;
}
bool
flare32_32_bit_offset_address_p (rtx x)
{
  bool
    ret = false,
    reg_plus_reg = false,
    reg_plus_imm = false,
    reg_plus_reg_plus_imm = false;
  flare32_constraint_debug_fprintf (stderr,
    "flare32_32_bit_offset_address_p () start:\n");
  flare32_constraint_debug_rtx (x);

  ret = (
    //MEM_P (x)
    //&& 
    (
      (reg_plus_reg = flare32_32_bit_offset_address_reg_plus_reg_p (x))
      || (reg_plus_imm = flare32_32_bit_offset_address_reg_plus_imm_p (x))
      || (
        reg_plus_reg_plus_imm
          = flare32_32_bit_offset_address_reg_plus_reg_plus_imm_p (x)
      )
    )
  );
  flare32_constraint_debug_fprintf (stderr,
    "flare32_32_bit_offset_address_p () end: %d:%d:%d:%d\n",
    //"flare32_32_bit_offset_address_p () end: %d:%d:%d\n",
    int(ret),
    int(reg_plus_reg),
    int(reg_plus_imm),
    int(reg_plus_reg_plus_imm)
    );
  flare32_constraint_debug_fprintf (stderr, "\n");

  return ret;
}

bool
flare32_8_or_16_bit_offset_address_p (rtx x)
{
  bool ret = false;
  flare32_constraint_debug_fprintf (stderr,
    "flare32_8_or_16_bit_offset_address_p () start:\n");
  flare32_constraint_debug_rtx (x);

  //if (!MEM_P (x))
  //{
  //  //ret = false;
  //}

  //else
  if (MEM_P (x))
  {
    // Inside the "MEM"
    x = XEXP (x, 0);

    //if (GET_CODE (x) == REG)
    //{
    //  return true;
    //}

    if (GET_CODE (x) == PLUS)
    {
      // Register + register indexing is allowed for 8-bit and 16-bit
      // loads and stores.
      //return (
      //  //GET_CODE (XEXP (x, 0)) == REG
      //    //&& GET_MODE (XEXP (x, 0)) == Pmode
      //    //&& REGNO_OK_FOR_BASE_P (REGNO (XEXP (x, 0)))
      //  //!flare32_absolute_ex (XEXP (x, 0))
      //  //&& GET_CODE (XEXP (x, 1)) == REG
      //  //  //&& GET_MODE (XEXP (x, 1)) == Pmode
      //  //  //&& REGNO_OK_FOR_INDEX_P (REGNO (XEXP (x, 1)))
      //  //&& !flare32_absolute_ex (XEXP (x, 1))
      //);
      //if (!(
      //  flare32_absolute_ex (XEXP (x, 0))
      //  || flare32_absolute_ex (XEXP (x, 1))
      //))
      rtx
        left_0 = XEXP (x, 0),
        right_0 = XEXP (x, 1);
      flare32_constraint_debug_fprintf (stderr, "have plus:\n");
      flare32_constraint_debug_rtx (left_0);
      flare32_constraint_debug_rtx (right_0);

      //#ifndef REG_OK_STRICT
      if (GET_CODE (left_0) == SUBREG)
      {
        //left_0 = SUBREG_REG (left_0);
        left_0
          = simplify_gen_subreg (SImode, left_0, DImode,
            subreg_lowpart_offset (SImode, DImode));
        flare32_constraint_debug_fprintf (stderr, "left_0 SUBREG:\n");
        flare32_constraint_debug_rtx (left_0);
      }
      if (GET_CODE (right_0) == SUBREG)
      {
        //right_0 = SUBREG_REG (right_0);
        right_0
          = simplify_gen_subreg (SImode, right_0, DImode,
           subreg_lowpart_offset (SImode, DImode));
        flare32_constraint_debug_fprintf (stderr, "right_0 SUBREG:\n");
        flare32_constraint_debug_rtx (right_0);
      }
      //#endif  // REG_OK_STRICT
      flare32_constraint_debug_fprintf (stderr,
        //"before if (...) ret = true: %d:%d:%d:%d\n",
        "before if (...) ret = true: %d:%d\n",
        int(GET_CODE (left_0) == REG),
        //int(REGNO_OK_FOR_BASE_P (REGNO (left_0))),
        int(GET_CODE (right_0) == REG)
        //int(REGNO_OK_FOR_BASE_P (REGNO (right_0)))
        );

      if (GET_CODE (left_0) == REG
          //&& REGNO_OK_FOR_BASE_P (REGNO (left_0))
        && GET_CODE (right_0) == REG
          //&& REGNO_OK_FOR_INDEX_P (REGNO (right_0))
        )
      {
        ret = true;
      }
    }
  }
  
  flare32_constraint_debug_fprintf (stderr,
    "flare32_8_or_16_bit_offset_address_p () end: %d\n",
    int(ret));
  flare32_constraint_debug_fprintf (stderr, "\n");
  return ret;
}
bool
flare32_non_offset_address_p (rtx x)
{
  bool ret = false;
  flare32_constraint_debug_fprintf (stderr,
    "flare32_non_offset_address_p () start:\n");
  flare32_constraint_debug_rtx (x);

  //if (!MEM_P (x))
  //{
  //  return false;
  //}

  if (MEM_P (x))
  {
    // Inside the "MEM"
    x = XEXP (x, 0);

    //#ifndef REG_OK_STRICT
    if (GET_CODE (x) == SUBREG)
    {
      //x = SUBREG_REG (x);
      x = simplify_gen_subreg (SImode, x, DImode,
        subreg_lowpart_offset (SImode, DImode));
      flare32_constraint_debug_fprintf (stderr, "x SUBREG:\n");
      flare32_constraint_debug_rtx (x);
    }
    //#endif    // REG_OK_STRICT

    flare32_constraint_debug_fprintf (stderr,
      //"before if (...) ret = true: %d:%d\n",
      "before if (...) ret = true: %d\n",
      int(GET_CODE (x) == REG)
      //int(REGNO_OK_FOR_BASE_P (REGNO (x)))
    );

    if (REG_P (x)
      //&& REGNO_OK_FOR_BASE_P (REGNO (x))
    )
    {
      ret = true;
    }

    //return true;
  }
  flare32_constraint_debug_fprintf (stderr,
    "flare32_non_offset_address_p () end: %d\n",
    int(ret));
  flare32_constraint_debug_fprintf (stderr, "\n");
  return ret;
}

//bool
//flare32_non_offset_address_p (rtx x)
//{
//  // Inside the "MEM"
//  x = XEXP (x, 0);
//  return GET_CODE (x) == REG;
//}



// Returns true if `REGNO` should be saved in the prologue
//static bool
//prologue_saved_reg_p (int regno)
//{
//  if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
//  {
//    return true;
//  }
//
//  if (frame_pointer_needed
//    && regno == (HARD_FRAME_POINTER_REGNUM))
//  {
//    return true;
//  }
//
//  if (regno == FLARE32_LR && df_regs_ever_live_p (FLARE32_LR))
//  {
//    return true;
//  }
//
//  return false;
//}

// Compute the size of the local area and the size to be adjusted by the
// prologue and epilogue.
// Implements the macro INITIAL_ELIMINATION_OFFSET, return the OFFSET.

int
flare32_initial_elimination_offset (int from, int to)
{
  int ret = 0;

  //if (((from) == FRAME_POINTER_REGNUM)
  //  && ((to) == HARD_FRAME_POINTER_REGNUM))
  //{
  //  flare32_compute_frame ();

  //  // There's technically a "push fp", but the compiler perhaps
  //  // doesn't need to really know about the "push fp".
  //  // The hard frame pointer is ABOVE the callee-saved registers
  //  // preservation area (actually points at the very first
  //  // location in the callee-saved registers preservation area).

  //  // (fake_fp - fp) == -(callee_saved_reg_size - UNITS_PER_WORD)
  //  ret = -(cfun->machine->callee_saved_reg_size - UNITS_PER_WORD);
  //}
  //else if ((from) == ARG_POINTER_REGNUM
  //  && (to) == HARD_FRAME_POINTER_REGNUM)
  //{
  //  // account for the "push fp"
  //  // arguments start two addresses above the hard frame pointer

  //  // (fake_ap - fp) == (UNITS_PER_WORD * 2) == 8;
  //  ret = (UNITS_PER_WORD * 2);
  //}
  ////if (((from) == ARG_POINTER_REGNUM)
  ////  && ((to) == FRAME_POINTER_REGNUM))
  ////{
  ////  ret = 0x00;
  ////}
  ////if ((((from) == FRAME_POINTER_REGNUM)
  ////  || ((from) == ARG_POINTER_REGNUM)))
  ////{
  ////  //ret = 0x0;
  ////}
  //else
  //{
  //  abort ();
  //}
  //--------
  // FL4SHK Note: seems that, at least for Flare32, we want the offset as
  // (from) - (to).
  if (from == ARG_POINTER_REGNUM
    && to == STACK_POINTER_REGNUM)
  {
    flare32_compute_frame ();

    //ret = UNITS_PER_WORD;
    //ret
    //  = -UNITS_PER_WORD
    //  //- cfun->machine->stack_args_size
    //    // we have `!ARGS_GROW_DOWNWARD`, so we don't need to include
    //    // stack_args_size in this calculation
    //  - (frame_pointer_needed ? UNITS_PER_WORD : 0)
    //  - cfun->machine->callee_saved_reg_size
    //  - cfun->machine->local_vars_size
    //  - cfun->machine->outgoing_args_size;
    ret
      = +UNITS_PER_WORD
      //+ cfun->machine->stack_args_size
        // we have `!ARGS_GROW_DOWNWARD`, so we don't need to include
        // stack_args_size in this calculation
      + (frame_pointer_needed ? UNITS_PER_WORD : 0)
      + cfun->machine->callee_saved_reg_size
      + cfun->machine->local_vars_size
      + cfun->machine->outgoing_args_size;
  }
  else if (from == ARG_POINTER_REGNUM
    && to == HARD_FRAME_POINTER_REGNUM)
  {
    // if frame_pointer_needed, then we have saved `fp`
    //ret = -UNITS_PER_WORD
    //  - (frame_pointer_needed ? UNITS_PER_WORD : 0);
    ret = +UNITS_PER_WORD
      + (frame_pointer_needed ? UNITS_PER_WORD : 0);
  }
  else if (from == FRAME_POINTER_REGNUM
    && to == STACK_POINTER_REGNUM)
  {
    flare32_compute_frame ();

    // if `frame_pointer_needed`, there's technically a "push fp", but the 
    // compiler perhaps doesn't need to really know about the "push fp".
    // The hard frame pointer is ABOVE the callee-saved registers
    // preservation area (actually points at the very first
    // location in the callee-saved registers preservation area).

    // (fake_fp - fp) == -(callee_saved_reg_size - UNITS_PER_WORD)
    //ret
    //  = //-cfun->machine->callee_saved_reg_size
    //    - (frame_pointer_needed ? UNITS_PER_WORD : 0x0)
    //  - cfun->machine->callee_saved_reg_size
    //  - cfun->machine->local_vars_size
    //  - cfun->machine->outgoing_args_size;
    ret
      = //-cfun->machine->callee_saved_reg_size
        + (frame_pointer_needed ? UNITS_PER_WORD : 0x0)
      + cfun->machine->callee_saved_reg_size
      + cfun->machine->local_vars_size
      + cfun->machine->outgoing_args_size;
    //ret
    //  = -UNITS_PER_WORD
    //  - cfun->machine->local_vars_size
    //  - cfun->machine->outgoing_args_size;
  }
  else if (from == FRAME_POINTER_REGNUM
    && to == HARD_FRAME_POINTER_REGNUM)
  {
    flare32_compute_frame ();

    // if `frame_pointer_needed`, there's technically a "push fp", but the 
    // compiler perhaps doesn't need to really know about the "push fp".
    // The hard frame pointer is ABOVE the callee-saved registers
    // preservation area (actually points at the very first
    // location in the callee-saved registers preservation area).

    // (fake_fp - fp) == -(callee_saved_reg_size - UNITS_PER_WORD)
    //ret = -(cfun->machine->callee_saved_reg_size - UNITS_PER_WORD);
    //ret = -(cfun->machine->callee_saved_reg_size
    //  - (frame_pointer_needed ? UNITS_PER_WORD : 0));
    //ret = cfun->machine->callee_saved_reg_size + UNITS_PER_WORD;

    //ret = -(frame_pointer_needed ? UNITS_PER_WORD : 0x0);
    ret = frame_pointer_needed ? UNITS_PER_WORD : 0x0;
  }
  else
  {
    gcc_unreachable ();
  }

  return ret;
}

static bool
flare32_return_in_memory
  (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);
  return (size == -1 || size > 2 * UNITS_PER_WORD);
  //return ((size == -1) || (size > UNITS_PER_WORD));
}

//static unsigned HOST_WIDE_INT 
//flare32_size_of_type (machine_mode mode, const_tree type)
//{
//  unsigned HOST_WIDE_INT size;
//
//  if (type)
//  {
//    //if (AGGREGATE_TYPE_P (type))
//    //{
//    //  return true;
//    //}
//    size = int_size_in_bytes (type);
//  }
//  else
//  {
//    size = GET_MODE_SIZE (mode);
//  }
//
//  return size;
//}

//static bool
//  flare32_must_pass_in_stack (machine_mode mode, const_tree type)
//{
//  if (type && AGGREGATE_TYPE_P (type))
//  {
//    return true;
//  }
//  return (flare32_size_of_type (mode, type) > (2 * UNITS_PER_WORD));
//  //return (flare32_size_of_type (mode, type) > UNITS_PER_WORD);
//}


// Return non-zero if the function argument described by TYPE is to be
// passed by reference.
static bool 
flare32_pass_by_reference (cumulative_args_t cum ATTRIBUTE_UNUSED,
  //machine_mode mode, const_tree type, bool named ATTRIBUTE_UNUSED
  const function_arg_info& arg
  )
{
  //unsigned HOST_WIDE_INT size;

  //if (type)
  //{
  //  if (AGGREGATE_TYPE_P (type))
  //  {
  //    return true;
  //  }
  //  size = int_size_in_bytes (type);
  //}
  //else
  //{
  //  size = GET_MODE_SIZE (mode);
  //}

  ////return (size > (4 * (FLARE32_LAST_ARG_REGNUM + 1)));
  //return (size > UNITS_PER_WORD);

  //if (arg.type && AGGREGATE_TYPE_P (arg.type))
  //{
  //  return true;
  //}
  //return (flare32_size_of_type (arg.mode, arg.type)
  //  > 2 * UNITS_PER_WORD);
  if (arg.aggregate_type_p ())
  {
    return true;
  }
  unsigned HOST_WIDE_INT size = arg.type_size_in_bytes ();
  return size > UNITS_PER_WORD * FLARE32_NUM_ARG_REGS;
}

/* Some function arguments will only partially fit in the registers
   that hold arguments.  Given a new arg, return the number of bytes
   that fit in argument passing registers.  */
static int
flare32_arg_partial_bytes (cumulative_args_t cum_v ATTRIBUTE_UNUSED,
  //machine_mode mode, tree type, bool named
  const function_arg_info& arg
  )
{
  //return 0;
  //CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  //int bytes_left, size;

  //if (*cum >= FLARE32_NUM_ARG_REGS)
  //{
  //  return 0;
  //}

  //if (flare32_pass_by_reference (cum_v, arg))
  //{
  //  //size = 4;
  //  return 4;
  //}
  //return 0;
  // else if (arg.type)
  // {
  //   if (AGGREGATE_TYPE_P (arg.type))
  //   {
  //     return 0;
  //   }
  //   size = int_size_in_bytes (arg.type);
  // }
  // else
  // {
  //   size = GET_MODE_SIZE (arg.mode);
  // }

  // bytes_left = (4 * 6) - ((*cum - 2) * 4);

  // if (size > bytes_left)
  // {
  //   return bytes_left;
  // }
  // else
  // {
  //   return 0;
  // }
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  int bytes_left, size;

  if (*cum >= FLARE32_NUM_ARG_REGS)
  {
    return 0;
  }

  if (flare32_pass_by_reference (cum_v, arg))
  {
    size = UNITS_PER_WORD;
  }
  else if (arg.type)
  {
    if (AGGREGATE_TYPE_P (arg.type))
    {
      return 0;
    }
    size = int_size_in_bytes (arg.type);
  }
  else
  {
    size = GET_MODE_SIZE (arg.mode);
  }

  bytes_left = (UNITS_PER_WORD * FLARE32_NUM_ARG_REGS);

  if (size > bytes_left)
  {
    return bytes_left;
  }
  else
  {
    return 0;
  }
}//

// Return the next register to be used to hold a function argument or
// NULL_RTX if there's no more space.
static rtx
flare32_function_arg (cumulative_args_t cum_v,
  //machine_mode mode, const_tree type ATTRIBUTE_UNUSED,
  //bool named ATTRIBUTE_UNUSED
  const function_arg_info& arg
  )
{
  machine_mode mode = arg.mode;
  //if (!named)
  //{
  //  return NULL_RTX;
  //}

  CUMULATIVE_ARGS* cum = get_cumulative_args (cum_v);

  if ((*cum) <= FLARE32_LAST_ARG_REGNUM)
  {
    // Check for trying to store larger-than-word-size arguments in
    // `cat(r5, r6)`
    if (GET_MODE_SIZE (mode) > UNITS_PER_WORD
      && (*cum) == FLARE32_LAST_ARG_REGNUM)
    {
      return NULL_RTX;
    }

    const auto temp = (*cum);
    //fprintf (stderr, "\nflare32_function_arg REG:  %d\n", temp);
    return gen_rtx_REG (mode, temp);
  }
  else
  {
    //fprintf (stderr, "\nflare32_function_arg NULL_RTX\n");
    return NULL_RTX;
  }
}

#define FLARE32_FUNCTION_ARG_SIZE(MODE, TYPE) \
  ((MODE) != BLKmode ? GET_MODE_SIZE (MODE) \
  : unsigned(int_size_in_bytes (TYPE)))

// This hook need not do anything if the argument in question was passed on
// the stack.
static void
flare32_function_arg_advance (cumulative_args_t cum_v,
  //machine_mode mode,
  //const_tree type, bool named ATTRIBUTE_UNUSED
  const function_arg_info& arg
  )
{
  CUMULATIVE_ARGS* cum = get_cumulative_args (cum_v);

  *cum = (((*cum) <= FLARE32_LAST_ARG_REGNUM)
    ? *cum + ((3 + FLARE32_FUNCTION_ARG_SIZE (arg.mode, arg.type))
      / UNITS_PER_WORD)
    : *cum);
}

//// This hook need not do anything if the argument in question was passed on
//// the stack.
//static void
//flare32_function_arg_advance
//  (cumulative_args_t cum_v, machine_mode mode, const_tree type,
//  bool named ATTRIBUTE_UNUSED)
//{
//  CUMULATIVE_ARGS* cum = get_cumulative_args (cum_v);
//
//  //*cum = ((*cum <= FLARE32_R5)
//  //  ? *cum + ((3 + FLARE32_FUNCTION_ARG_SIZE (mode, type)) / 4)
//  //  : *cum);
//  //fprintf (stderr, "flare32_function_arg_advance:  %d\n", *cum);
//
//  if ((*cum) <= FLARE32_LAST_ARG_REGNUM)
//  {
//    //*cum += (3 + FLARE32_FUNCTION_ARG_SIZE (mode, type)) / 4;
//
//    //// AFAIK we don't have any BLKmode instructions....
//    //if (mode == BLKmode)
//    //{
//    //  abort ();
//    //}
//
//    const auto size = (mode != BLKmode) ? GET_MODE_SIZE (mode)
//      : static_cast<unsigned>(int_size_in_bytes (type));
//
//    //fprintf (stderr, "flare32_function_arg_advance size:  %d\n", size);
//
//    // Small operands
//    if ((size / 4) == 0)
//    {
//      *cum += 1;
//    }
//    else
//    {
//      *cum += size / 4;
//    }
//  }
//  //else
//  //{
//  //  *cum = FLARE32_LAST_ARG_REGNUM + 1;
//  //}
//
//  //fprintf (stderr, "flare32_function_arg_advance:  %d\n\n", *cum);
//}

bool
flare32_absolute (rtx x)
{
  return (
    GET_CODE (x) == SYMBOL_REF
    || GET_CODE (x) == LABEL_REF
    || GET_CODE (x) == CONST
    //|| GET_CODE (x) == CONST_INT
  );
}
bool
flare32_absolute_ex (rtx x)
{
  return (
    flare32_absolute (x)
    || (
      GET_CODE (x) == CONST_INT
      //&& (HOST_WIDE_INT (INTVAL (x))
      //  & ((HOST_WIDE_INT (1) << HOST_WIDE_INT (32)) - HOST_WIDE_INT (1)))
      //  == HOST_WIDE_INT (INTVAL (x))
    )
    //|| GET_CODE (x) == CONST_DOUBLE
  );
}

//bool
//flare32_absolute_plus_ci (rtx x)
//{
//  return (
//    GET_CODE (x) == PLUS
//    && flare32_absolute (XEXP (x, 0))
//    && CONST_INT_P (XEXP (x, 1))
//  );
//}
//static bool
//flare32_absolute_w_ci_plus (rtx x)
//{
//  return (
//      GET_CODE (x) == PLUS
//      && flare32_absolute_innards (XEXP (x, 0))
//      && GET_CODE (XEXP (x, 1)) == CONST_INT
//    );
//}

//bool
//flare32_reg_cpyspec (rtx x)
//{
//  return (GET_CODE (x) == REG
//    && flare32_reg_class_cpyspec (REGNO_REG_CLASS (REGNO (x))));
//}
//bool
//flare32_reg_class_cpyspec (reg_class x)
//{
//  //return ((x == CC_REGS) || (x == HI_REGS) || (x == LO_REGS));
//  return (x == CC_REGS);
//  //return ((x == HI_REGS) || (x == LO_REGS));
//}
//bool
//flare32_reg_general (rtx x)
//{
//  return (GET_CODE (x) == REG
//    && flare32_reg_class_general (REGNO_REG_CLASS (REGNO (x))));
//}
//bool
//flare32_reg_class_general (reg_class x)
//{
//  return (x == GENERAL_REGS);
//}


// Zero initialization is OK for all current fields.
static machine_function*
flare32_init_machine_status ()
{
  return ggc_cleared_alloc<machine_function>();
}

//static bool
//flare32_target_rtx_costs (rtx x,
//  machine_mode mode, int outer_code ATTRIBUTE_UNUSED,
//  int opno ATTRIBUTE_UNUSED, int* total, bool speed)
//{
//  if (speed)
//  {
//    switch (GET_CODE (x))
//    {
//    case PLUS:
//    case MINUS:
//      if (mode == DImode)
//      {
//        *total = COSTS_N_INSNS (2);
//      }
//      //else
//      //{
//      //  *total = COSTS_N_INSNS (1);
//      //}
//      break;
//    case MULT:
//      if (mode == SImode)
//      {
//        *total = COSTS_N_INSNS (4);
//      }
//      else if (mode == DImode)
//      {
//        *total = COSTS_N_INSNS (6);
//      }
//      break;
//
//    case DIV:
//    case UDIV:
//    case MOD:
//    case UMOD:
//      if (mode == SImode)
//      {
//        // Assume 4 bits computed per cycle.
//        *total = COSTS_N_INSNS (8);
//      }
//      else if (mode == DImode)
//      {
//        // Assume 2 bits computed per cycle.
//        *total = COSTS_N_INSNS (32);
//      }
//      break;
//    //case MEM:
//    //  *total = COSTS_N_INSNS (1);
//
//    default:
//      break;
//    }
//  }
//  //else
//  //{
//  //}
//
//  return true;
//}

//static int
//flare32_target_memory_move_cost
//  (machine_mode mode, reg_class_t rclass_as_t,
//  bool in ATTRIBUTE_UNUSED)
//{
//  const reg_class rclass = reg_class (rclass_as_t);
//
//  //int ret = 1;
//
//  //if (rclass != GENERAL_REGS)
//  //{
//  //  ++ret;
//  //}
//
//  //if (mode == DImode)
//  //{
//  //  ++ret;
//  //}
//  int ret = 2;
//
//  if (rclass != GENERAL_REGS)
//  {
//    ret *= 2;
//  }
//
//  if (mode == DImode)
//  {
//    ret *= 2;
//  }
//
//  return ret;
//}

static void
flare32_option_override ()
{
  // Set the per-function-data initializer.
  init_machine_status = flare32_init_machine_status;
}

bool
flare32_regno_actually_callee_saved (int regno)
{
  // Don't need to preserve "pc"
  // Need to preserve "lr" if it gets used.
  // "df_regs_ever_live_p" checks whether or not the register is
  // used in this function.
  //if (regno == STACK_POINTER_REGNUM)
  //{
  //  fprintf (stderr, "flare32_regno_actually_callee_saved: %d; %d %d\n",
  //    regno,
  //    df_regs_ever_live_p (regno), call_used_or_fixed_reg_p (regno));
  //}


  if (regno == HARD_FRAME_POINTER_REGNUM
    || regno == FLARE32_PC)
  {
    return false;
  }
  //if (frame_pointer_needed && regno == (HARD_FRAME_POINTER_REGNUM))
  //{
  //  return true;
  //}

  if (df_regs_ever_live_p (regno))
  {
    if (regno == FLARE32_FAKE_FP || regno == FLARE32_FAKE_AP)
    {
      return false;
    }

    //if (!call_used_regs[regno] || regno == FLARE32_LR)
    if (!call_used_or_fixed_reg_p (regno) || regno == FLARE32_LR)
    {
      return true;
    }
  }
  return false;
}
//bool
//flare32_regno_actually_callee_saved_no_hfp (int regno)
//{
//  return (flare32_regno_actually_callee_saved (regno)
//    && regno != HARD_FRAME_POINTER_REGNUM);
//}

// borrowed from RISC-V
//static void
//flare32_emit_stack_tie (void)
//{
//  //if (Pmode == SImode)
//  {
//    emit_insn (gen_stack_tiesi (stack_pointer_rtx,
//      hard_frame_pointer_rtx));
//  }
//  //else
//  //{
//  //  emit_insn (gen_stack_tiedi (stack_pointer_rtx,
//  //    hard_frame_pointer_rtx));
//  //}
//}

static rtx
flare32_push (int regno, bool frame_related_p)
{
  //rtx insn, movsi_push, rtx_reg;

  //rtx_reg = gen_rtx_REG (SImode, regno);
  //movsi_push = gen_movsi_push (rtx_reg);
  //insn = emit_insn (movsi_push);

  //// Attach a note indicating what happened.
  //add_reg_note (insn, REG_FRAME_, copy_rtx (rtx_reg));
  //add_reg_note (insn, REG_FRAME_RELATED_EXPR, copy_rtx (movsi_push));

  //RTX_FRAME_RELATED_P (insn) = 1;

  // Idea borrowed from the AVR's `emit_push_byte ()`.
  rtx mem, reg;
  rtx_insn* insn;

  mem = gen_rtx_POST_DEC (SImode, stack_pointer_rtx);
  mem = gen_frame_mem (SImode, mem);
  reg = gen_rtx_REG (SImode, regno);

  insn = emit_insn (gen_rtx_SET (mem, reg));

  if (frame_related_p)
  {
    RTX_FRAME_RELATED_P (insn) = 1;
  }

  return insn;
}
static rtx
flare32_pop (int regno)
{
  //rtx insn, movsi_pop, rtx_reg;

  //rtx_reg = gen_rtx_REG (SImode, regno);

  //movsi_pop = gen_movsi_pop (rtx_reg);

  //insn = emit_insn (movsi_pop);

  // Attach a note indicating what happened.
  //add_reg_note (insn, REG_CFA_RESTORE, copy_rtx (rtx_reg));

  //RTX_FRAME_RELATED_P (insn) = 1;

  // idea borrowed from AVR's `emit_pop_byte ()`.
  // Hopefully this will just work.
  rtx insn, mem, reg;

  mem = gen_rtx_PRE_INC (SImode, stack_pointer_rtx);
  mem = gen_frame_mem (SImode, mem);
  reg = gen_rtx_REG (SImode, regno);

  insn = emit_insn (gen_rtx_SET (reg, mem));

  return insn;
}
// Emit RTL equivalent of ADD3 with the given const_int for
// frame-related registers.
//   op0          - Destination register.
//   op1          - First addendum operand (a register).
//   addendum     - Second addendum operand (a constant).
//   kind         - Note kind.  REG_NOTE_MAX if no note must be added.
// Note: adapted from PRU
static rtx
flare32_add3_frame_adjust (rtx op0, rtx op1, int addendum,
  const enum reg_note kind)
{
  rtx insn;

  //if (GET_CODE (op0) == REG
  //  && HARD_REGISTER_P (op0))
  //{
  //  fprintf (stderr, "flare32_add3_frame_adjust: %d\n",
  //    REGNO (op0));
  //}
  //if (GET_CODE (op1) == REG
  //  && HARD_REGISTER_P (op1))
  //{
  //  fprintf (stderr, "...continued: %d\n",
  //    REGNO (op1));
  //}

  rtx op0_adjust = gen_rtx_SET (op0,
    plus_constant (Pmode, op1, addendum));

  insn = emit_insn (op0_adjust);

  /* Attach a note indicating what happened.  */
  if (kind != REG_NOTE_MAX)
  {
    add_reg_note (insn, kind, copy_rtx (op0_adjust));
  }

  RTX_FRAME_RELATED_P (insn) = 1;

  return insn;
}
// Add a const_int to the stack pointer register.
static rtx
flare32_add_to_sp (int addendum, const enum reg_note kind)
{
  return flare32_add3_frame_adjust
    (stack_pointer_rtx, stack_pointer_rtx, addendum, kind);
}

void
flare32_expand_prologue ()
{
  rtx insn;

  flare32_compute_frame ();

  //fprintf (stderr, "flare32_expand_prologue ():  %d %d %d\n",
  //  crtl->args.pretend_args_size, machine.local_vars_size,
  //  machine.stack_args_size);

  if (flag_stack_usage_info)
  {
    // this is apparently only used for the user of GCC, rather than the
    // developer of GCC
    current_function_static_stack_size
      = cfun->machine->static_stack_size;
  }

  //auto gen_push = [&](int regno) -> void
  //{
  //  insn = emit_insn (gen_movsi_push (gen_rtx_REG (SImode, regno)));
  //  RTX_FRAME_RELATED_P (insn) = 1;
  //};

  // Save the frame pointer
  //if (df_regs_ever_live_p (FLARE32_FP))
  //if ((cfun->machine->local_vars_size > 0)
  //  || (cfun->machine->stack_args_size > 0))
  if (frame_pointer_needed)
  {
    //gen_push (HARD_FRAME_POINTER_REGNUM);
    flare32_push (HARD_FRAME_POINTER_REGNUM, true);

    // Set the frame pointer
    //rtx temp_movsi = gen_movsi (hard_frame_pointer_rtx, stack_pointer_rtx);
    //insn = gen_movsi (hard_frame_pointer_rtx, stack_pointer_rtx);
    //rtx temp_rtx_set = gen_rtx_SET (hard_frame_pointer_rtx,
    //  stack_pointer_rtx);
    //insn = emit_insn (temp_rtx_set);

    //// Attach a note indicating what happened.
    ////add_reg_note (insn, REG_FRAME_RELATED_EXPR, copy_rtx (temp_movsi));
    ////add_reg_note (insn, REG_FRAME_RELATED_EXPR, copy_rtx (temp_rtx_set));
    ////insn = GEN_INT (-UNITS_PER_WORD);
    ////insn = emit_insn (gen_addsi3 (hard_frame_pointer_rtx,
    ////  stack_pointer_rtx, insn));

    //RTX_FRAME_RELATED_P (insn) = 1;
    // Not sure this is correct?
    //flare32_add3_frame_adjust (hard_frame_pointer_rtx, stack_pointer_rtx,
    //  UNITS_PER_WORD, REG_FRAME_RELATED_EXPR);
    flare32_add3_frame_adjust (hard_frame_pointer_rtx, stack_pointer_rtx,
      UNITS_PER_WORD, REG_NOTE_MAX);

    //flare32_emit_stack_tie ();
  }

  //fprintf (stderr, "\nflare32_expand_prologue ()\n");

  // Save callee-saved registers
  for (int regno=0; regno<FIRST_PSEUDO_REGISTER; ++regno)
  {
    if (flare32_regno_actually_callee_saved (regno))
    //if (flare32_regno_actually_callee_saved_no_hfp (regno))
    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    {
      //gen_push (regno);
      flare32_push (regno, true);
    }
  }

  // Adjust the stack pointer.
  if (cfun->machine->size_for_adjusting_sp > 0)
  {
    //insn = emit_insn (gen_subsi3 (stack_pointer_rtx, stack_pointer_rtx,
    //  GEN_INT (cfun->machine->size_for_adjusting_sp)));
    //RTX_FRAME_RELATED_P (insn) = 1;
    //insn = flare32_add_to_sp (-cfun->machine->size_for_adjusting_sp,
    //  REG_FRAME_RELATED_EXPR);
    //insn = flare32_add_to_sp (-cfun->machine->size_for_adjusting_sp,
    //  REG_CFA_ADJUST_CFA);
    insn = flare32_add_to_sp (-cfun->machine->size_for_adjusting_sp,
      REG_FRAME_RELATED_EXPR);
  }

  // If we are profiling, make sure no instructions are scheduled before
  // the call to mcount.
  if (crtl->profile)
  {
    emit_insn (gen_blockage ());
  }

}
void
flare32_expand_epilogue ()
{
  //printf ("flare32_expand_epilogue (): %i\n",
  //  int(cfun->calls_alloca));
  //int regno;
  //rtx insn;

  //if (cfun->calls_alloca)
  //{
  //  flare32_emit_stack_tie ();
  //}

  //auto gen_pop = [&](int regno) -> void
  //{
  //  insn = emit_insn (gen_movsi_pop (gen_rtx_REG (SImode, regno)));
  //  RTX_FRAME_RELATED_P (insn) = 1;
  //};

  emit_insn (gen_blockage ());

  if (cfun->machine->size_for_adjusting_sp > 0)
  {
    //insn = emit_insn (gen_addsi3 (stack_pointer_rtx, stack_pointer_rtx,
    //  GEN_INT (cfun->machine->size_for_adjusting_sp)));
    //RTX_FRAME_RELATED_P (insn) = 1;
    //fprintf (stderr, "flare32_expand_epilogue (): %d\n",
    //  cfun->machine->size_for_adjusting_sp);
    //flare32_add_to_sp (cfun->machine->size_for_adjusting_sp,
    //  REG_CFA_ADJUST_CFA);
    flare32_add_to_sp (cfun->machine->size_for_adjusting_sp,
      REG_FRAME_RELATED_EXPR);
    //flare32_add_to_sp (cfun->machine->size_for_adjusting_sp,
    //  REG_NOTE_MAX);
  }

  //fprintf (stderr, "\nflare32_expand_epilogue ()\n");

  // Restore callee-saved registers
  for (int regno=FIRST_PSEUDO_REGISTER-1; regno>=0; --regno)
  {
    if (flare32_regno_actually_callee_saved (regno))
    //if (flare32_regno_actually_callee_saved_no_hfp (regno))
    //if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    {
      //gen_pop (regno);
      flare32_pop (regno);
    }
  }

  // Restore the frame pointer.
  //if (df_regs_ever_live_p (FLARE32_FP))
  //if ((cfun->machine->local_vars_size > 0)
  //  || (cfun->machine->stack_args_size > 0))
  if (frame_pointer_needed)
  {
    //gen_pop (FLARE32_FP);
    //gen_pop (HARD_FRAME_POINTER_REGNUM);
    flare32_pop (HARD_FRAME_POINTER_REGNUM);
  }

  emit_jump_insn (gen_returner ());
  //emit_jump_insn (gen_return ());
  //emit_jump_insn (gen_simple_return ());
}
//void
//flare32_expand_mov32 (rtx& dst, rtx& src, machine_mode mode)
//{
//  //fprintf (stderr, "flare32_expand_mov32 (): begin\n");
//
//  //if (can_create_pseudo_p ())
//  //if (lra_in_progress || reload_completed)
//  //{
//  //  //fprintf (stderr, "flare32_expand_mov32 (): %i %i\n",
//  //  //  MEM_P (dst), MEM_P (src));
//  //}
//  //else // 
//  //if (!lra_in_progress && !reload_completed)
//  //if (can_create_pseudo_p ())
//  {
//    fprintf (stderr,
//      //"flare32_expand_mov32 (): begin: "
//      //"%i %i; %s %s %s; ",
//      "%i:%i:%s ",
//      //"src: %sREG_P %sSUBREG_P %sMEM_P; "
//      //"dst: %sREG_P %sSUBREG_P %sMEM_P\n",
//      can_create_pseudo_p (),
//      (!lra_in_progress && !reload_completed),
//      //GET_MODE_NAME (GET_MODE (dst)),
//      //GET_MODE_NAME (GET_MODE (src)),
//      GET_MODE_NAME (mode)
//      //REG_P (src) ? "" : "!",
//      //SUBREG_P (src) ? "" : "!",
//      //MEM_P (src) ? "" : "!",
//      //REG_P (dst) ? "" : "!",
//      //SUBREG_P (dst) ? "" : "!",
//      //MEM_P (dst) ? "" : "!"
//      );
//
//    flare32_dbg_print_rtx (dst);
//    fprintf (stderr, " ");
//    flare32_dbg_print_rtx (src);
//    fprintf (stderr, "\n");
//
//    if (GET_CODE (dst) == MEM)
//    {
//      //fprintf (stderr,
//      //  "MEM_P (dst): %i %i %i %i\n",
//      //  GET_CODE (XEXP (dst, 0)) == MEM,
//      //  GET_CODE (XEXP (dst, 0)) == PLUS,
//      //  GET_CODE (XEXP (dst, 0)) == REG,
//      //  GET_CODE (XEXP (dst, 0)) == SUBREG
//      //  );
//      //fprintf (stderr, "GET_CODE (dst) == MEM\n"); 
//      // If this is a store, force the value into a register.
//      if (GET_CODE (src) != REG)
//      {
//        //if (!lra_in_progress && !reload_completed)
//        if (can_create_pseudo_p ())
//        {
//          fprintf (stderr,
//            //"flare32_expand_mov32 (): "
//            "src = force_reg: %sREG_P\n",
//            REG_P (src) ? "" : "!");
//          src = force_reg (mode, src);
//          flare32_dbg_print_rtx (src);
//          fprintf (stderr, "\n");
//        }
//        //else
//        //{
//        //  fprintf (stderr, "flare32_expand_mov32 (): src = force_reg\n");
//        //}
//      }
//      if (GET_CODE (XEXP (dst, 0)) == MEM
//        || GET_CODE (XEXP (dst, 0)) == SYMBOL_REF
//        || GET_CODE (XEXP (dst, 0)) == LABEL_REF
//        || GET_CODE (XEXP (dst, 0)) == CONST
//        || GET_CODE (XEXP (dst, 0)) == CONST_INT
//
//        //|| GET_CODE (XEXP (dst, 0)) == CONST_DOUBLE
//      )
//      {
//        //if (!lra_in_progress && !reload_completed)
//        if (can_create_pseudo_p ())
//        {
//          fprintf (stderr,
//            //"flare32_expand_mov32 (): "
//            "dst = gen_rtx_MEM (mode, tmp)\n");
//          rtx tmp = force_reg (mode, XEXP (dst, 0));
//          dst = gen_rtx_MEM (mode, tmp);
//          flare32_dbg_print_rtx (dst);
//          fprintf (stderr, "\n");
//        }
//        //else
//        //{
//        //  fprintf (stderr,
//        //    "flare32_expand_mov32 (): dst = gen_rtx_MEM (mode, tmp)\n");
//        //}
//      }
//    }
//    else 
//    {
//      //if (GET_CODE (src) == MEM)
//      //{
//      //  fprintf (stderr,
//      //    "GET_CODE (dst) != MEM && GET_CODE (src) == MEM\n"); 
//      //}
//      //else
//      //{
//      //  fprintf (stderr,
//      //    "GET_CODE (dst) != MEM && GET_CODE (src) != MEM\n"); 
//      //}
//      // If this is a store, force the value into a register.
//      if (GET_CODE (src) == MEM
//          && (
//            GET_CODE (XEXP (src, 0)) == MEM
//            || GET_CODE (XEXP (src, 0)) == SYMBOL_REF
//            || GET_CODE (XEXP (src, 0)) == LABEL_REF
//            || GET_CODE (XEXP (src, 0)) == CONST
//            || GET_CODE (XEXP (src, 0)) == CONST_INT
//            //|| GET_CODE (XEXP (src, 0)) == CONST_DOUBLE
//          )
//        )
//      {
//        //if (!lra_in_progress && !reload_completed)
//        if (can_create_pseudo_p ())
//        {
//          fprintf (stderr,
//            //"flare32_expand_mov32 (): "
//            "src = gen_rtx_MEM (mode, tmp)\n");
//          rtx tmp = force_reg (mode, XEXP (src, 0));
//          src = gen_rtx_MEM (mode, tmp);
//          flare32_dbg_print_rtx (src);
//          fprintf (stderr, "\n");
//        }
//        //else
//        //{
//        //  fprintf (stderr,
//        //    "flare32_expand_mov32 (): src = gen_rtx_MEM (mode, tmp)\n");
//        //}
//      }
//    }
//  }
//}

//extern int which_alternative;

void
flare32_emit_mov (rtx dst, rtx src, machine_mode mode)
{
  //bool did_inner_print = false;
  rtx operands[2];
  operands[0] = dst;
  operands[1] = src;

  flare32_debug_fprintf (stderr,
    //"dbg: %s ",
    "mov start: %d:%d:%d:%s\n",
    can_create_pseudo_p (),
    lra_in_progress,
    reload_completed,
    GET_MODE_NAME (mode));
  flare32_debug_rtx (operands[0]);
  flare32_debug_rtx (operands[1]);

  if (!(reload_in_progress || reload_completed))
  {
    if (
      //can_create_pseudo_p () && 
      MEM_P (operands[0])
    //&& !flare32_mov_reg_operand (mode, operands[1])
    )
    {
      flare32_debug_fprintf (stderr, "MEM_P (operands[0])\n");
      {
        operands[1] = force_reg (mode, operands[1]);
        flare32_debug_fprintf (stderr,
          "operands[1] = force_reg (...):\n");
        flare32_debug_rtx (operands[1]);
      }

      if (MEM_P (XEXP (operands[0], 0))
        || flare32_absolute_ex (XEXP (operands[0], 0)))
      {
        rtx tmp;
        flare32_debug_fprintf (stderr,
          "operands[0] = gen_rtx_MEM (...):\n");
        flare32_debug_rtx (XEXP (operands[0], 0));
        tmp = force_reg (Pmode, XEXP (operands[0], 0));
        flare32_debug_rtx (tmp);
        operands[0] = gen_rtx_MEM (mode, tmp);
        flare32_debug_rtx (operands[0]);
      }
      else
      {
        flare32_debug_fprintf (stderr,
          "don't have inner `MEM_P`/`flare32_absolute_ex`\n");
      }
    }
    else if (
      //can_create_pseudo_p () &&
      MEM_P (operands[1])
    )
    {
      flare32_debug_fprintf (stderr, "MEM_P (operands[1])\n");
      if (MEM_P (XEXP (operands[1], 0))
        || flare32_absolute_ex (XEXP (operands[1], 0)))
      {
        rtx tmp;
        flare32_debug_fprintf (stderr,
          "operands[1] = gen_rtx_MEM (...):\n");
        flare32_debug_rtx (XEXP (operands[1], 0));
        tmp = force_reg (Pmode, XEXP (operands[1], 0));
        flare32_debug_rtx (tmp);
        operands[1] = gen_rtx_MEM (mode, tmp);
        flare32_debug_rtx (operands[1]);
      }
      else
      {
        flare32_debug_fprintf (stderr,
          "don't have inner `MEM_P`/`flare32_absolute_ex`\n");
      }
    }
  }

  flare32_debug_fprintf (stderr,
    //"dbg: %s ",
    "mov end: %d:%d:%d:%s ",
    can_create_pseudo_p (),
    lra_in_progress,
    reload_completed,
    GET_MODE_NAME (mode));
  flare32_debug_fprintf (stderr, "\n");
  //if (did_inner_print)
  {
    flare32_debug_fprintf (stderr, "\n");
  }
  //if (GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
  {
    emit_insn (gen_rtx_SET (operands[0], operands[1]));
  }
  //else if (GET_MODE_SIZE (mode) == UNITS_PER_WORD * 2)
  //{
  //  emit_insn (gen_rtx_SET
  //    (gen_lowpart (SImode, operands[0]),
  //    gen_lowpart (SImode, operands[1])));
  //  emit_insn (gen_rtx_SET
  //    (gen_highpart (SImode, operands[0]),
  //    gen_highpart (SImode, operands[1])));
  //}
  //else
  //{
  //  gcc_unreachable ();
  //}
}
//void
//flare32_expand_movsi (rtx dst, rtx src)
//{
//  //if (!(reload_in_progress || reload_completed))
//  if (can_create_pseudo_p ())
//  {
//    if (MEM_P (dst))
//    {
//      // If this is a store, force the value into a register.
//      //if (!GET_CODE (src) == REG)
//      {
//        src = force_reg (SImode, src);
//        flare32_debug_fprintf (stderr, "movsi 0: %i\n", REGNO (src));
//      }
//      if (MEM_P (XEXP (dst, 0))
//        || GET_CODE (XEXP (dst, 0)) == SYMBOL_REF
//        || GET_CODE (XEXP (dst, 0)) == LABEL_REF
//        || GET_CODE (XEXP (dst, 0)) == CONST
//        || GET_CODE (XEXP (dst, 0)) == CONST_INT
//      )
//      {
//        if (can_create_pseudo_p ())
//        {
//          rtx tmp = force_reg (SImode, XEXP (dst, 0));
//          flare32_debug_fprintf (stderr, "movsi 1: %i\n", REGNO (tmp));
//          dst = gen_rtx_MEM (SImode, tmp);
//        }
//      }
//    }
//    else 
//    {
//      // If this is a store, force the value into a register.
//      if (MEM_P (src)
//          && (
//            MEM_P (XEXP (src, 0))
//            || GET_CODE (XEXP (src, 0)) == SYMBOL_REF
//            || GET_CODE (XEXP (src, 0)) == LABEL_REF
//            || GET_CODE (XEXP (src, 0)) == CONST
//            || GET_CODE (XEXP (src, 0)) == CONST_INT
//          )
//        )
//      {
//        {
//          rtx tmp = force_reg (SImode, XEXP (src, 0));
//          flare32_debug_fprintf (stderr, "movsi 2: %i\n", REGNO (tmp));
//          src = gen_rtx_MEM (SImode, tmp);
//        }
//        //src = force_reg (SImode, src);
//      }
//      //else if (
//      //  GET_CODE (src) == SYMBOL_REF
//      //  || GET_CODE (src) == LABEL_REF
//      //  || GET_CODE (src) == CONST
//      //  || GET_CODE (src) == CONST_INT
//      //)
//      //{
//      //  src = force_reg
//      //}
//    }
//  }
//}
//bool
//flare32_expand_movdi (rtx dst, rtx src)
//{
//  // cpy !REG, !REG
//  if (!GET_CODE (dst) == REG && !GET_CODE (src) == REG && can_create_pseudo_p ())
//  {
//    auto tmp = gen_reg_rtx (DImode);
//    emit_insn (gen_movdi_raw (tmp, src));
//    emit_insn (gen_movdi_raw (dst, tmp));
//    return true;
//  }
//
//  if (flare32_reg_cpyspec (dst)
//    && ((!GET_CODE (src) == REG) || (!flare32_reg_general (src)))
//    && can_create_pseudo_p ())
//  {
//    auto tmp = gen_reg_rtx (DImode);
//    emit_insn (gen_movdi_raw (tmp, src));
//    emit_insn (gen_movdi_raw (dst, tmp));
//    return true;
//  }
//
//  // cpy REG, REG
//  if (GET_CODE (dst) == REG && GET_CODE (src) == REG)
//  {
//    emit_insn (gen_movdi_raw (dst, src));
//    return true;
//  }
//
//  // cpyi REG, immediate
//  if (GET_CODE (dst) == REG && flare32_absolute (src))
//  {
//    emit_insn (gen_movdi_raw (dst, src));
//    return true;
//  }
//
//  if (MEM_P (dst) && (MEM_P (src) || CONST_INT_P (src))
//    && can_create_pseudo_p ())
//  {
//    rtx tmp;
//
//    if (MEM_P (src))
//    {
//      tmp = gen_reg_rtx (GET_MODE (src));
//    }
//    else if (CONST_INT_P (src))
//    {
//      tmp = gen_reg_rtx (DImode);
//    }
//    else
//    {
//      return false;
//    }
//
//    emit_insn (gen_movdi_raw (tmp, src));
//    emit_insn (gen_movdi_raw (dst, tmp));
//    return true;
//  }
//
//  if (MEM_P (dst) && can_create_pseudo_p ()
//    && flare32_absolute (XEXP (dst, 0)))
//  {
//    auto tmp = gen_reg_rtx (GET_MODE (dst));
//    emit_insn (gen_movdi_raw (tmp, XEXP (dst, 0)));
//    emit_insn (gen_movdi_raw (dst, tmp));
//    return true;
//  }
//
//  if (MEM_P (src) && can_create_pseudo_p ()
//    && flare32_absolute (XEXP (src, 0)))
//  {
//    auto tmp = gen_reg_rtx (GET_MODE (src));
//    emit_insn (gen_movdi_raw (tmp, XEXP (src, 0)));
//    emit_insn (gen_movdi_raw (src, tmp));
//    return true;
//  }
//
//  return false;
//  //return true;
//}

// idea borrowed from the MIPS port
//static rtx_insn*
//flare32_emit_move (rtx dst, rtx src)
//{
//  return (can_create_pseudo_p ()
//    ? emit_move_insn (dst, src)
//    : emit_move_insn_1 (dst, src));
//  return emit_move_insn (dst, src)
//}

//bool
//flare32_legitimize_movdi (rtx dst, rtx src)
//{
//  // idea borrowed from the MIPS port
//  if (!register_operand (dst, DImode)
//    && !register_operand (src, DImode)
//    && can_create_pseudo_p ())
//  {
//    //flare32_emit_move (dst, force_reg (DImode, src));
//    emit_move_insn (dst, force_reg (DImode, src));
//    return true;
//  }
//
//  //if (CONSTANT_P (src)
//  //  //&& !flare32_general_movdi_src_operand (src, DImode)
//  //)
//  //{
//  //}
//
//  return false;
//}
//const char*
//flare32_output_movdi (rtx dst, rtx src)
//{
//  enum rtx_code dst_code = GET_CODE (dst);
//  enum rtx_code src_code = GET_CODE (src);
//  //flare32_debug_fprintf (stderr, "testificate\n");
//
//  // =r, r
//  if (flare32_reg_general (dst)
//    && flare32_reg_general (src))
//  {
//    return "cpy %L0, %L1\n\tcpy %H0, %H1\t\t// *movdi: =r, r";
//    //return "cpy %L0, %L1\n\tcpy %H0, %H1\t\t// *movdi";
//  }
//
//  // =r, i
//  else if (flare32_reg_general (dst)
//    //&& flare32_absolute_w_ci (src)
//    && (
//      src_code == SYMBOL_REF
//      || src_code == LABEL_REF
//      || src_code == CONST
//      || src_code == CONST_INT
//    ))
//  {
//    //return "cpy %L0, #(%1) >> 32\n\tcpy %L1, #(%1) & 0xffffffff"
//    //  "\t\t// *movdi: =r, i";
//    //return "cpy %L0, #(%1) >> 32\n\tcpy %L1, #(%1) & 0xffffffff"
//    //  "\t\t// *movdi: =r, i";
//    return "cpy %L0, #%1\n\tcpy %L1, #0\t\t// *movdi: =r, i";
//  }
//
//  // =r, Q
//  else if (flare32_reg_general (dst)
//    && src_code == CONST_WIDE_INT)
//  {
//    return "cpy %L0, #(%1) >> 32\n\tcpy %L1, #(%1) & 0xffffffff"
//      "\t\t// *movdi: =r, Q";
//  }
//
//  // =r, B
//  else if (flare32_reg_general (dst)
//    && src_code == MEM
//    && flare32_32_bit_offset_address_reg_plus_reg_p (src)
//    )
//  {
//    return "ldr %L0, [%L1, #4]\n\tldr %H0, [%L1, #0]\t\t// *movdi: =r, B";
//  }
//  // =r, B
//  else if (dst_code == REG
//    && src_code == MEM
//    && (
//      flare32_32_bit_offset_address_reg_plus_imm_p (src)
//      || flare32_32_bit_offset_address_reg_plus_reg_plus_imm_p (src)
//    ))
//  {
//    return "ldr %L0, [%L1+4]\n\tldr %H0, [%L1+0]\t\t// *movdi: =r, B";
//  }
//
//  // =B, r
//  else if (dst_code == MEM
//    && flare32_32_bit_offset_address_reg_plus_reg_p (dst)
//    && flare32_reg_general (src))
//  {
//    return "str %L1, [%L0, #4]\n\tstr %H1, [%L0, #0]\t\t// *movdi: =B, r";
//  }
//  // =B, r
//  else if (dst_code == MEM
//    && (
//      flare32_32_bit_offset_address_reg_plus_imm_p (dst)
//      || flare32_32_bit_offset_address_reg_plus_reg_plus_imm_p (dst)
//    )
//    && flare32_reg_general (src)
//    )
//  {
//    return "str %L1, [%L0+4]\n\tstr %H0, [%L0+0]\t\t// *movdi: =B, r";
//  }
//
//  // =r, W
//  else if (flare32_reg_general (dst)
//    && src_code == MEM
//      && flare32_reg_general (XEXP (src, 0)))
//  {
//    return "ldr %L0, [%L1, #4]\n\tldr %H0, [%L1, #0]\t\t// *movdi: =r, W";
//  }
//  // =W, r
//  else if (dst_code == MEM
//      && flare32_reg_general (XEXP (dst, 0))
//    && flare32_reg_general (src) )
//  {
//    return "str %L1, [%L0, #4]\n\tstr %H1, [%L0, #0]\t\t// *movdi: =W, r";
//  }
//
//  else
//  {
//    gcc_unreachable ();
//  }
//}
//void
//flare32_dbg_call_value (rtx retop, rtx target, rtx op2)
//{
//  //gcc_assert (MEM_P (target));
//  flare32_debug_fprintf (stderr, "flare32_dbg_call_value () start:\n");
//  flare32_debug_rtx (retop);
//  flare32_debug_rtx (target);
//  flare32_debug_rtx (op2);
//  //emit_call_insn (gen_rtx_CALL (VOIDmode, retop, target));
//  flare32_debug_fprintf (stderr, "\n");
//}
// Implement RETURN_ADDR_RTX.  Note, we do not support moving
// back to a previous frame.
rtx
flare32_get_return_address (int count)
{
  if (count != 0)
  {
    return NULL_RTX;
  }

  return get_hard_reg_initial_val (Pmode, FLARE32_LR);
}
//int
//flare32_stack_pointer_offset ()
//{
//  int ret;
//
//  flare32_compute_frame ();
//
//  ret =;
//
//  return ret;
//}
//int
//flare32_incoming_frame_sp_offset ()
//{
//  int ret;
//
//  flare32_compute_frame ();
//
//  ret
//    = 
//    ;
//
//  return ret;
//}

static bool
flare32_hard_regno_mode_ok (unsigned int regno, machine_mode mode)
{
  if (GET_MODE_CLASS (mode) == MODE_CC)
  {
    return (regno == FLARE32_FLAGS);
  }
  if (regno == FLARE32_FLAGS && GET_MODE_CLASS (mode) != MODE_CC)
  {
    return false;
  }

  //if (regno < FLARE32_LR
  //  && GET_MODE_SIZE (mode) == UNITS_PER_WORD * 2)
  //{
  //  return true;
  //}
  if (regno <= FLARE32_LR
    && GET_MODE_SIZE (mode) <= UNITS_PER_WORD)
  {
    return true;
  }
  if (regno == HARD_FRAME_POINTER_REGNUM || regno == STACK_POINTER_REGNUM
    || regno == FRAME_POINTER_REGNUM || regno == ARG_POINTER_REGNUM)
  {
    return (GET_MODE_SIZE (mode) <= UNITS_PER_WORD);
  }
  ////if (regno == FLARE32_FAKE_FP || regno == FLARE32_FAKE_AP)
  ////{
  ////  return GET_MODE_CLASS (mode) == MODE_INT;
  ////}
  //flare32_debug_fprintf (stderr,
  //  "flare32_hard_regno_mode_ok (): dbg: %d %d\n",
  //  regno, GET_MODE_SIZE (mode));
  //return false;

  if (regno < FLARE32_LR
    && GET_MODE_SIZE (mode) > UNITS_PER_WORD)
  {
    const unsigned int
      num_consec_regs = ((GET_MODE_SIZE (mode)) / UNITS_PER_WORD);

    //flare32_debug_fprintf (stderr,
    //  "flare32_hard_regno_mode_ok (): %u %u %u %d\n",
    //  regno,
    //  num_consec_regs,
    //  regno % num_consec_regs,
    //  (regno % num_consec_regs) == 0);
    //return ((regno % num_consec_regs) == 0);
    if ((regno % num_consec_regs) == 0)
    {
      return true;
    }
  }
  return false;
}
static void
flare32_setup_incoming_varargs
  (cumulative_args_t cum_v,
  const function_arg_info& arg ATTRIBUTE_UNUSED,
  int *pretend_args_size, int no_rtl)
{
  CUMULATIVE_ARGS* cum = get_cumulative_args (cum_v);
  int nregs = FLARE32_NUM_ARG_REGS - (*cum);
  
  *pretend_args_size = (nregs < 0) ? 0 : (GET_MODE_SIZE (SImode) * nregs);
  
  if (no_rtl)
  {
    return;
  }
  
  for (int regno=*cum; regno<FLARE32_NUM_ARG_REGS; ++regno)
  {
    rtx reg = gen_rtx_REG (SImode, regno);
    rtx slot = gen_rtx_PLUS
      (Pmode,
      gen_rtx_REG (SImode, ARG_POINTER_REGNUM),
      GEN_INT (UNITS_PER_WORD * regno));
    
    emit_move_insn (gen_rtx_MEM (SImode, slot), reg);
  }
}
//--------
//void
//flare32_gen_cmpdi_reg (rtx x, rtx y)
//{
//  //machine_mode mode;
//  //rtx cc_reg;
//
//  rtx x_lo
//    = simplify_gen_subreg (SImode, x, DImode,
//      subreg_lowpart_offset (SImode, DImode));
//  rtx x_hi
//    = simplify_gen_subreg (SImode, x, DImode,
//      subreg_highpart_offset (SImode, DImode));
//  rtx y_lo
//    = simplify_gen_subreg (SImode, y, DImode,
//      subreg_lowpart_offset (SImode, DImode));
//  rtx y_hi
//    = simplify_gen_subreg (SImode, y, DImode,
//      subreg_highpart_offset (SImode, DImode));
//  rtx scratch = gen_reg_rtx (SImode);
//  emit_insn (gen_cmpsi (x_lo, y_lo));
//  emit_insn (gen_flare32_cmpsi3_carry_in (scratch, x_hi, y_hi));
//}
//--------
//#undef COMPAT_SIMODE_TRAPPING_ARITHMETIC
//#undef L_addvsi3
//#undef L_addvdi3
//
//#undef L_subvsi3
//#undef L_subvdi3
//
//#undef L_mulvsi3
//#undef L_mulvdi3
//
//#undef L_divvsi3
//#undef L_divvdi3
//
//#undef L_udivvsi3
//#undef L_udivvdi3
//
//#undef L_ffsdi2
// --------
// Initialize the GCC target structure.

#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_const_tree_true

//#undef TARGET_PROMOTE_FUNCTION_MODE
//#define TARGET_PROMOTE_FUNCTION_MODE
//  default_promote_function_mode_always_promote

#undef TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY flare32_return_in_memory
#undef TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size
//#undef TARGET_MUST_PASS_IN_STACK
//#define TARGET_MUST_PASS_IN_STACK flare32_must_pass_in_stack
//#define TARGET_MUST_PASS_IN_STACK must_pass_in_stack_var_size
#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE flare32_pass_by_reference
#undef  TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES flare32_arg_partial_bytes
//#define TARGET_ARG_PARTIAL_BYTES hook_bool_void_false
#undef TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG flare32_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE flare32_function_arg_advance

#undef TARGET_LRA_P
//#define TARGET_LRA_P hook_bool_void_false
#define TARGET_LRA_P hook_bool_void_true

#undef  TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P flare32_legitimate_address_p

#undef  TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS flare32_setup_incoming_varargs

//#undef TARGET_SECONDARY_RELOAD
//#define TARGET_SECONDARY_RELOAD flare32_secondary_reload

#undef TARGET_FIXED_CONDITION_CODE_REGS
#define TARGET_FIXED_CONDITION_CODE_REGS flare32_fixed_condition_code_regs

//#define TARGET_SPILL_CLASS flare32_target_spill_class

// Define this to return an RTX representing the place where a
// function returns or receives a value of data type RET_TYPE, a tree
// node representing a data type.
#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE flare32_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE flare32_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P flare32_function_value_regno_p

#undef TARGET_FRAME_POINTER_REQUIRED
//#define TARGET_FRAME_POINTER_REQUIRED hook_bool_void_true
#define TARGET_FRAME_POINTER_REQUIRED hook_bool_void_false

//#undef TARGET_STATIC_CHAIN
//#define TARGET_STATIC_CHAIN flare32_static_chain
//#undef TARGET_ASM_TRAMPOLINE_TEMPLATE
//#define TARGET_ASM_TRAMPOLINE_TEMPLATE flare32_asm_trampoline_template
//#undef TARGET_TRAMPOLINE_INIT
//#define TARGET_TRAMPOLINE_INIT flare32_trampoline_init

#undef TARGET_HARD_REGNO_MODE_OK 
#define TARGET_HARD_REGNO_MODE_OK flare32_hard_regno_mode_ok


//#undef TARGET_RTX_COSTS
//#define TARGET_RTX_COSTS flare32_target_rtx_costs
//
//#undef TARGET_MEMORY_MOVE_COST
//#define TARGET_MEMORY_MOVE_COST flare32_target_memory_move_cost

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE flare32_option_override

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND flare32_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS flare32_print_operand_address

#undef  TARGET_CONSTANT_ALIGNMENT
#define TARGET_CONSTANT_ALIGNMENT constant_alignment_word_strings

//#undef TARGET_PREFERRED_RELOAD_CLASS
//#define TARGET_PREFERRED_RELOAD_CLASS flare32_preferred_reload_class

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-flare32.h"
