;; Machine description for Flare32
;; Copyright (C) 2023 Free Software Foundation, Inc.

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.


(include "iterators.md")
(include "constants.md")
(include "constraints.md")
(include "predicates.md")



;; Most instructions are two bytes long
;;(define_attr "length" "" (const_int 2))

(define_insn "nop"
  [(const_int 0)]
  "true"
  "add r0, #0\\t\\t// nop")

;; -----------------------
;; Arithmetic Instructions
;; -----------------------
(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r,r,r,r,r")
    (plus:SI
      (match_operand:SI 1 "register_operand" "0,t,q,0,d,t,q")
      (match_operand:SI 2 "general_operand" "r,r,r,i,i,i,i")))]
  ""
  "@
  add %0, %2\\t\\t// addsi3: =r, 0, r
  add %0, %1, %2\\t\\t// addsi: =r, t, r
  add %0, %1, %2\\t\\t// addsi: =r, q, r
  add %0, #%2\\t\\t// addsi: =r, 0, i
  add %0, %1, #%2\\t\\t// addsi: =r, d, i
  add %0, %1, #%2\\t\\t// addsi: =r, t, i
  add %0, %1, #%2\\t\\t// addsi: =r, q, i"
)

;; --------
;;(define_insn "adddi3"
;;  [(parallel
;;    [(set (match_operand:DI 0 "register_operand" "=r")
;;      (plus:DI
;;        (match_operand:DI 1 "register_operand" "%0")
;;        (match_operand:DI 2 "register_operand" "r")))
;;    (clobber (reg:CC REG_FLAGS))])]
;;  ""
;;  "add.f %L0, %L2\n\tadc.f %H0, %H2\\t\\t// adddi3: =r, 0, r"
;;  ;;[(set_attr "type" "multiple")]
;;)
;; --------
;; Use this one
(define_expand "adddi3"
  [(parallel
    [(set (match_operand:DI 0 "register_operand")
      (plus:DI
        (match_operand:DI 1 "register_operand")
        (match_operand:DI 2 "general_operand")))
    (clobber (reg:CC REG_FLAGS))])]
  ""
{
  if (!REG_P (operands[2]))
  {
    operands[2] = force_reg (DImode, operands[2]);
  }
})
(define_insn "*adddi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
    (plus:DI
      (match_operand:DI 1 "register_operand" "0")
      (match_operand:DI 2 "register_operand" "r")))
    (clobber (reg:CC REG_FLAGS))]
  ""
  ;;"fake_add64 %0, %2"
  "add.f %L0, %L2\n\tadc.f %H0, %H2\\t\\t// adddi3: =r, %0, r"
)

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r,r,r")
    (minus:SI
      (match_operand:SI 1 "register_operand" "0,0,d,t,q")
      (match_operand:SI 2 "general_operand" "r,i,i,i,i")))]
  ""
  "@
  sub %0, %2\\t\\t// subsi3: =r, 0, r
  add %0, #-%2\\t\\t// subsi3: =r, 0, i
  add %0, %1, #-%2\\t\\t// subsi3: =r, d, i
  add %0, %1, #-%2\\t\\t// subsi3: =r, t, i
  add %0, %1, #-%2\\t\\t// subsi3: =r, q, i"
)

;; Use this one
(define_expand "subdi3"
  [(parallel
    [(set (match_operand:DI 0 "register_operand")
      (minus:DI
        (match_operand:DI 1 "register_operand")
        (match_operand:DI 2 "general_operand")))
    (clobber (reg:CC REG_FLAGS))])]
  ""
{
  if (!REG_P (operands[2]))
  {
    operands[2] = force_reg (DImode, operands[2]);
  }
})
(define_insn "*subdi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
    (minus:DI
      (match_operand:DI 1 "register_operand" "%0")
      (match_operand:DI 2 "register_operand" "r")))
    (clobber (reg:CC REG_FLAGS))]
  ""
  ;;"fake_add64 %0, %2"
  "sub.f %L0, %L2\n\tsbc.f %H0, %H2\\t\\t// subdi3: =r, %0, r"
)

;;(define_expand "subdi3"
;;  [(parallel
;;    [(set (match_operand:DI 0 "register_operand" "=&r")
;;      (plus:DI
;;        (match_operand:DI 1 "register_operand" "%0")
;;        (match_operand:DI 2 "general_operand" "")))
;;      (clobber (reg:CC REG_FLAGS))])]
;;  ""
;;{
;;  if (!REG_P (operands[2]))
;;  {
;;    operands[2] = force_reg (DImode, operands[2]);
;;  }
;;})
;;
;;(define_insn "*subdi3"
;;  [(set (match_operand:DI 0 "register_operand" "=&r")
;;    (plus:DI
;;      (match_operand:DI 1 "register_operand" "%0")
;;      (match_operand:DI 2 "register_operand" "r")))
;;    (clobber (reg:CC REG_FLAGS))]
;;  ""
;;  ;;"fake_sub64 %0, %2"
;;  "sub.f %L0, %L2\n\tsbc.f %H0, %H2\\t\\t// subdi3: =&r, %0, r"
;;)

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (mult:SI
      (match_operand:SI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "mul %0, %2")
;; --------

;;(define_expand "umulsidi3"
;;  [(set (match_operand:DI 0 "register_operand" "")
;;    (mult:DI
;;      (match_operand:DI 1 "register_operand" "")
;;      (match_operand:DI 2 "register_operand" "")))]
;;  ""
;;{
;;  if (REGNO (operands[0]) != FLARE32_R0)
;;  {
;;    auto tmp_r0 = gen_rtx_REG (DImode, FLARE32_R0);
;;    emit_insn (gen_movdi (tmp_r0, operands[1]));
;;    emit_insn (gen_umulsidi3_raw (tmp_r0, tmp_r0, operands[2]));
;;    emit_insn (gen_movdi (operands[0], tmp_r0));
;;    //emit_insn (gen_blockage ());
;;    DONE;
;;  }
;;  else
;;  {
;;    emit_insn (gen_umulsidi3_raw (operands[0], operands[1], operands[2]));
;;    DONE;
;;  }
;;  ;;emit_insn (gen_umulsidi3_raw (operands[0], operands[1], operands[2]));
;;})
;;(define_insn "umulsidi3_raw"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (mult:DI
;;      (match_operand:DI 1 "register_operand" "%0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "lumul %H2, %L2"
;;)

;;(define_insn "umulsidi3"
;;  [(set (match_operand:DI 0 "flare32_full_product_result_reg" "=r")
;;    (mult:DI
;;      (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
;;      (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
;;  ""
;;  "lumul %1, %2")

;;(define_insn "mulsidi3"
;;  [(set (match_operand:DI 0 "flare32_full_product_result_reg" "=r")
;;    (mult:DI
;;      (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
;;      (sign_extend:DI (match_operand:SI 2 "register_operand" "r"))))]
;;  ""
;;  "lsmul %1, %2")

;; Use this one
;;(define_expand "umulsi3_highpart"
;;  [(parallel
;;    [(set (match_operand:SI 0 "register_operand" "=r")
;;    (truncate:SI
;;      (lshiftrt:DI
;;        (mult:DI
;;          (zero_extend:DI (match_operand 1 "register_operand" "r"))
;;          (zero_extend:DI (match_operand 2 "register_operand" "r")))
;;        (const_int 32))))
;;      (clobber (reg:SI REG_R1))])]
;;  ""
;;{
;;})
;;(define_insn "*umulsi3_highpart"
;;  [(set (match_operand:SI 0 "flare32_full_product_high_reg" "=r")
;;    (truncate:SI
;;      (lshiftrt:DI
;;        (mult:DI
;;          (zero_extend:DI (match_operand 1 "register_operand" "r"))
;;          (zero_extend:DI (match_operand 2 "register_operand" "r")))
;;        (const_int 32))))
;;      (clobber (reg:SI REG_R1))]
;;  ""
;;  "lumul %1 %2"
;;)
;;
;;(define_expand "mulsi3_highpart"
;;  [(parallel
;;    [(set (match_operand:SI 0 "register_operand" "=r")
;;    (truncate:SI
;;      (lshiftrt:DI
;;        (mult:DI
;;          (sign_extend:DI (match_operand 1 "register_operand" "r"))
;;          (sign_extend:DI (match_operand 2 "register_operand" "r")))
;;        (const_int 32))))
;;      (clobber (reg:SI REG_R1))])]
;;  ""
;;{
;;})
;;(define_insn "*mulsi3_highpart"
;;  [(set (match_operand:SI 0 "flare32_full_product_high_reg" "=r")
;;    (truncate:SI
;;      (lshiftrt:DI
;;        (mult:DI
;;          (sign_extend:DI (match_operand 1 "register_operand" "r"))
;;          (sign_extend:DI (match_operand 2 "register_operand" "r")))
;;        (const_int 32))))
;;      (clobber (reg:SI REG_R1))]
;;  ""
;;  "lsmul %1 %2"
;;)
;;
(define_expand "<Us>mulsidi3"
  [(set (match_operand:DI 0 "register_operand")
    (mult:DI
      (EXTEND:DI (match_operand:SI 1 "register_operand"))
      (EXTEND:DI (match_operand:SI 2 "register_operand"))))]
  ""
{
  emit_insn (gen_l<US>mul (gen_lowpart (SImode, operands[0]),
    gen_highpart (SImode, operands[0]),
    operands[1], operands[2]));
  DONE;
})
(define_expand "<Us>mulsi3_highpart"
  [(parallel
    [(set (match_operand:SI 0 "register_operand")
      (truncate:SI
        (lshiftrt:DI
          (mult:DI
            (EXTEND:DI (match_operand:SI 1 "register_operand"))
            (EXTEND:DI (match_operand:SI 2 "register_operand")))
          (const_int 32))))
      (clobber (match_operand:SI 3 "register_operand"))])]
  ""
  ""
)

(define_insn "*l<US>mul_high"
  [(set (match_operand:SI 0 "flare32_full_product_high_reg" "=x")
    (truncate:SI
      (lshiftrt:DI
        (mult:DI
          (EXTEND:DI (match_operand:SI 1 "register_operand" "r"))
          (EXTEND:DI (match_operand:SI 2 "register_operand" "r")))
        (const_int 32))))
    (clobber (match_operand:SI 3 "flare32_full_product_low_reg" "=y"))]
  ""
  "l<US>mul %1, %2\\t\\t// *l<US>mul_high"
)
(define_insn "l<US>mul"
  [(set (match_operand:SI 0 "flare32_full_product_high_reg" "=x")
    (mult:SI
      (match_operand:SI 2 "register_operand" "r")
      (match_operand:SI 3 "register_operand" "r")))
  (set (match_operand:SI 1 "flare32_full_product_low_reg" "=y")
    (truncate:SI
      (lshiftrt:DI
        (mult:DI (EXTEND:DI (match_dup 2)) (EXTEND:DI (match_dup 3)))
        (const_int 32))))]
  ""
  "l<US>mul %1, %2\\t\\t// <US>mul"
)

;; --------
(define_insn "udivsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (udiv:SI
      (match_operand:SI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "udiv %0, %2")

(define_insn "divsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (div:SI
      (match_operand:SI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "sdiv %0, %2")

(define_insn "umodsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (umod:SI
      (match_operand:SI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "umod %0, %2")

(define_insn "modsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (mod:SI
      (match_operand:SI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "smod %0, %2")

;; --------
;;(define_expand "udivdi3"
;;  [(set (match_operand:DI 0 "register_operand" "")
;;    (udiv:DI
;;      (match_operand:DI 1 "register_operand" "")
;;      (match_operand:DI 2 "register_operand" "")))]
;;  ""
;;{
;;  if (REGNO (operands[0]) != FLARE32_R0)
;;  {
;;    auto tmp_r0 = gen_rtx_REG (DImode, FLARE32_R0);
;;    emit_insn (gen_movdi (tmp_r0, operands[1]));
;;    emit_insn (gen_udivdi3_raw (tmp_r0, tmp_r0, operands[2]));
;;    emit_insn (gen_movdi (operands[0], tmp_r0));
;;    emit_insn (gen_blockage ());
;;    DONE;
;;  }
;;  else
;;  {
;;    emit_insn (gen_udivdi3_raw (operands[0], operands[1], operands[2]));
;;    DONE;
;;  }
;;  emit_insn (gen_udivdi3_raw (operands[0], operands[1], operands[2]));
;;})
;;(define_insn "udivdi3_raw"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (udiv:DI
;;      (match_operand:DI 1 "register_operand" "%0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "udiv64 %H2, %L2"
;;)

;; Use this one
(define_insn "udivdi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
    (udiv:DI
      (match_operand:DI 1 "register_operand" "0")
      (match_operand:DI 2 "register_operand" "r")))]
  ""
  "udiv64 %H0, %H2"
)

;;(define_expand "divdi3"
;;  [(set (match_operand:DI 0 "register_operand" "")
;;    (div:DI
;;      (match_operand:DI 1 "register_operand" "")
;;      (match_operand:DI 2 "register_operand" "")))]
;;  ""
;;{
;;  if (REGNO (operands[0]) != FLARE32_R0)
;;  {
;;    auto tmp_r0 = gen_rtx_REG (DImode, FLARE32_R0);
;;    emit_insn (gen_movdi (tmp_r0, operands[1]));
;;    emit_insn (gen_divdi3_raw (tmp_r0, tmp_r0, operands[2]));
;;    emit_insn (gen_movdi (operands[0], tmp_r0));
;;    emit_insn (gen_blockage ());
;;    DONE;
;;  }
;;  else
;;  {
;;    emit_insn (gen_divdi3_raw (operands[0], operands[1], operands[2]));
;;    DONE;
;;  }
;;  emit_insn (gen_divdi3_raw (operands[0], operands[1], operands[2]));
;;})
;;(define_insn "divdi3_raw"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;    (div:DI
;;      (match_operand:DI 1 "register_operand" "%0")
;;      (match_operand:DI 2 "register_operand" "r")))]
;;  ""
;;  "sdiv64 %H2, %L2"
;;)
;; Use this one

(define_insn "divdi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
    (div:DI
      (match_operand:DI 1 "register_operand" "0")
      (match_operand:DI 2 "register_operand" "r")))]
  ""
  "sdiv64 %H0, %H2"
)

;; Use this one
(define_insn "umoddi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
    (umod:DI
      (match_operand:DI 1 "register_operand" "0")
      (match_operand:DI 2 "register_operand" "r")))]
  ""
  "umod64 %H0, %H2"
)

;; Use this one
(define_insn "moddi3"
  [(set (match_operand:DI 0 "register_operand" "=r")
    (mod:DI
      (match_operand:DI 1 "register_operand" "0")
      (match_operand:DI 2 "register_operand" "r")))]
  ""
  "lmod %H0, %L0"
)

;; --------
;;(define_insn "mulsi3"
;;  [(set (match_operand:SI 0 "register_operand" "=l")
;;    (mult:SI
;;      (match_operand:SI 1 "register_operand" "h")
;;      (match_operand:SI 2 "register_operand" "l")))
;;    (clobber (match_operand:SI 3 "register_operand" ""))]
;;  ""
;;{
;;  //if (REGNO_REG_CLASS (REGNO (operands[1])) != HI_REGS)
;;  //{
;;  //  auto hi = gen_rtx_REG (SImode, REG_HI);
;;  //  emit_insn (gen_movsi (operands[3], hi));
;;
;;  //}
;;
;;  //else
;;  //{
;;  //  return "lumul";
;;  //}
;;  emit_insn (gen_movsi (operands[3], operands[1]));
;;  return "lumul\ncpyspec hi, %3";
;;})


(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
  (ashift:SI (match_operand:SI 1 "register_operand" "0,0")
  (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  lsl %0, %2
  lsl %0, #%2")

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (lshiftrt:SI (match_operand:SI 1 "register_operand" "0,0")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  lsr %0, %2
  lsr %0, #%2")
(define_insn "lshrhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
    (lshiftrt:HI (match_operand:HI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "lsrh %0, %2")
(define_insn "lshrqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
    (lshiftrt:QI (match_operand:QI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "lsrb %0, %2")

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (ashiftrt:SI (match_operand:SI 1 "register_operand" "0,0")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  asr %0, %2
  asr %0, #%2")
(define_insn "ashrhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
    (ashiftrt:HI (match_operand:HI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "asrh %0, %2")
(define_insn "ashrqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
    (ashiftrt:QI (match_operand:QI 1 "register_operand" "0")
      (match_operand:SI 2 "register_operand" "r")))]
  ""
  "asrb %0, %2")

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (and:SI (match_operand:SI 1 "register_operand" "0,0")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  and %0, %2
  and %0, #%2")

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (ior:SI (match_operand:SI 1 "register_operand" "0,0")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  orr %0, %2
  orr %0, #%2")

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
    (xor:SI (match_operand:SI 1 "register_operand" "0,0")
      (match_operand:SI 2 "general_operand" "r,i")))]
  ""
  "@
  xor %0, %2
  xor %0, #%2")

(define_expand "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (not:SI (match_operand:SI 1 "register_operand" "0")))]
  ""
{
  auto tmp_const = GEN_INT (-1);
  emit_insn (gen_xorsi3 (operands[0], operands[0], tmp_const));
  DONE;
})


;; Push a register onto the stack
(define_insn "movsi_push"
  [(set (mem:SI (post_dec:SI (reg:SI REG_SP)))
    (match_operand:SI 0 "register_operand" "r"))]
  ""
  "push %0")

;; Pop a register from the stack
(define_insn "movsi_pop"
  [(set (match_operand:SI 0 "register_operand" "=r")
    (mem:SI (pre_inc:SI (reg:SI REG_SP))))]
  ""
  "pop %0")


;;(define_expand "movsi"
;;  [(set (match_operand:SI 0 "nonimmediate_operand" "")
;;    (match_operand:SI 1 "general_operand" ""))]
;;  ""
;;
;;{
;;  if (MEM_P (operands[0]) && (MEM_P (operands[1])
;;    || CONST_INT_P (operands[1])))
;;  {
;;    if (can_create_pseudo_p ())
;;    {
;;      rtx tmp;
;;      
;;      if (MEM_P (operands[1]))
;;      {
;;        tmp = gen_reg_rtx (GET_MODE (operands[1]));
;;      }
;;      else if (CONST_INT_P (operands[1]))
;;      {
;;        tmp = gen_reg_rtx (SImode);
;;      }
;;      else
;;      {
;;        //fprintf (stderr, "\nTest\n");
;;        //abort ();
;;        FAIL;
;;      }
;;
;;      emit_insn (gen_movsi (tmp, operands[1]));
;;      operands[1] = tmp;
;;    }
;;  }
;;  //else if (MEM_P (operands[0]) && flare32_absolute (operands[0]))
;;  else if (MEM_P (operands[0]))
;;  {
;;    if (can_create_pseudo_p ())
;;    {
;;      if (flare32_absolute (XEXP (operands[0], 0)))
;;      {
;;        auto tmp = gen_reg_rtx (GET_MODE (operands[0]));
;;        emit_insn (gen_movsi (tmp, XEXP (operands[0], 0)));
;;        operands[0] = tmp;
;;      }
;;
;;      //if (flare32_reg_cpyspec (operands[1]))
;;      //{
;;      //  auto tmp = gen_reg_rtx (GET_MODE (operands[1]));
;;      //  emit_insn (gen_movsi (tmp, operands[1]));
;;      //  operands[1] = tmp;
;;      //}
;;    }
;;  }
;;  //else if (MEM_P (operands[1]) && flare32_absolute (operands[1]))
;;  else if (MEM_P (operands[1]))
;;  {
;;    if (can_create_pseudo_p ())
;;    {
;;      if (flare32_absolute (XEXP (operands[1], 0)))
;;      {
;;        auto tmp = gen_reg_rtx (GET_MODE (operands[1]));
;;        emit_insn (gen_movsi (tmp, XEXP (operands[1], 0)));
;;        operands[1] = tmp;
;;      }
;;      //if (flare32_reg_cpyspec (operands[0]))
;;      //{
;;      //  auto tmp = gen_reg_rtx (GET_MODE (operands[0]));
;;      //  emit_insn (gen_movsi (tmp, operands[0]));
;;      //  operands[0] = tmp;
;;      //}
;;    }
;;  }
;;
;;  // cpy specreg, #immediate
;;  // cpy specreg, other_specreg
;;  else if (flare32_reg_cpyspec (operands[0]))
;;  {
;;    if ((!REG_P (operands[1])) || flare32_reg_cpyspec (operands[1]))
;;    {
;;      if (can_create_pseudo_p ())
;;      {
;;        auto tmp = gen_reg_rtx (GET_MODE (operands[1]));
;;        emit_insn (gen_movsi (tmp, operands[1]));
;;        operands[1] = tmp;
;;      }
;;    }
;;  }
;;
;;})

(define_expand "mov<mode>"
  [(set (match_operand:MOV32 0 "general_operand" "")
    (match_operand:MOV32 1
      "general_operand"
      ""))]
  ""
{
  flare32_emit_mov (operands[0], operands[1], <MODE>mode);
  DONE;
})


(define_insn "*mov32"
  [(set (match_operand:MOV32 0
    "nonimmediate_operand"
    "=r,r,r,B,r,W,r"
    )
    (match_operand:MOV32 1 
      "flare32_general_mov32_src_operand"
      "r,i,B,r,W,r,d"
      ))]

  "register_operand (operands[0], <MODE>mode)
    || register_operand (operands[1], <MODE>mode)"
  ;;""
  "@
  cpy %0, %1\\t\\t// *mov32: =r, r
  cpy %0, #%1\\t\\t// *mov32: =r, i
  ldr %0, [%1]\\t\\t// *mov32: =r, B
  str %1, [%0]\\t\\t// *mov32: =B, r
  ldr %0, [%1]\\t\\t// *mov32: =r, W
  str %1, [%0]\\t\\t// *mov32: =W, r
  add %0, %1, #0x0\\t\\t// *mov32: =r, d"

  ;;cpy %0, %1\\t\\t// *mov32: =r, S
  ;;cpy %0, %1\\t\\t// *mov32: =S, r
  ;;cpy %0, %1\\t\\t// *mov32: =S, S
)


;; --------

;; Use this one
(define_expand "mov<mode>"
  [(set (match_operand:MOV64 0 "nonimmediate_operand" "")
    (match_operand:MOV64 1 "general_operand" ""))]
  ""
{
  flare32_emit_mov (operands[0], operands[1], <MODE>mode);
  DONE;
})
;;(define_expand "mov<mode>"
;;  [(set (match_operand:MOV64 0 "nonimmediate_operand" "")
;;    (match_operand:MOV64 1 "general_operand" ""))]
;;  ""
;;{
;;  //if (flare32_legitimize_movdi (operands[0], operands[1]))
;;  //{
;;  //  DONE;
;;  //}
;;  //if (!(reload_in_progress || reload_completed))
;;  if (can_create_pseudo_p ())
;;  {
;;    if (MEM_P (operands[0]))
;;    {
;;      if (can_create_pseudo_p ())
;;      {
;;        //if (MEM_P (operands[1]))
;;        //{
;;        //  if (
;;        //    (
;;        //      MEM_P (XEXP (operands[1], 0))
;;        //      //|| flare32_abs_pred (XEXP (operands[1], 0), <MODE>mode)
;;        //      || GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF
;;        //      || GET_CODE (XEXP (operands[1], 0)) == LABEL_REF
;;        //      || GET_CODE (XEXP (operands[1], 0)) == CONST
;;        //      || GET_CODE (XEXP (operands[1], 0)) == CONST_INT
;;        //      //|| GET_CODE (XEXP (operands[1], 0)) == CONST_WIDE_INT
;;        //      //|| GET_CODE (XEXP (operands[1], 0)) == CONST_DOUBLE
;;        //    )
;;        //  )
;;        //  {
;;        //    operands[1] = gen_rtx_MEM (<MODE>mode,
;;        //      force_reg (SImode,
;;        //        gen_lowpart (<MODE>mode, XEXP (operands[1], 0))));
;;        //  }
;;        //  else
;;        //  {
;;        //    operands[1] = force_reg (<MODE>mode, operands[1]);
;;        //  }
;;        //}
;;        //else
;;        {
;;          if (can_create_pseudo_p ())
;;          {
;;            operands[1] = force_reg (<MODE>mode, operands[1]);
;;            //operands[1] = force_reg (<MODE>mode,
;;            //  gen_lowpart (<MODE>mode, operands[1])); 
;;          }
;;        }
;;        //rtx tmp;
;;
;;        //if (MEM_P (operands[1]))
;;        //{
;;        //}
;;        //else if (
;;        //  GET_CODE (operands[1]) == SYMBOL_REF
;;        //  || GET_CODE (operands[1]) == LABEL_REF
;;        //  || GET_CODE (operands[1]) == CONST
;;        //  || GET_CODE (operands[1]) == CONST_INT
;;        //)
;;        //{
;;        //}
;;      }
;;      if (MEM_P (XEXP (operands[0], 0))
;;        //|| flare32_abs_pred (XEXP (operands[0], 0), <MODE>mode)
;;        || GET_CODE (XEXP (operands[0], 0)) == SYMBOL_REF
;;        || GET_CODE (XEXP (operands[0], 0)) == LABEL_REF
;;        || GET_CODE (XEXP (operands[0], 0)) == CONST
;;        || GET_CODE (XEXP (operands[0], 0)) == CONST_INT
;;        //|| GET_CODE (XEXP (operands[0], 0)) == CONST_WIDE_INT
;;        //|| GET_CODE (XEXP (operands[0], 0)) == CONST_DOUBLE
;;      )
;;      {
;;        if (can_create_pseudo_p ())
;;        {
;;          //operands[0] = gen_rtx_MEM (<MODE>mode,
;;          //  force_reg (Pmode,
;;          //    gen_lowpart (<MODE>mode, XEXP (operands[0], 0))));
;;          operands[0] = gen_rtx_MEM (<MODE>mode,
;;            force_reg (<MODE>mode, XEXP (operands[0], 0)));
;;        }
;;      }
;;    }
;;    else // if (!MEM_P (operands[0]))
;;    {
;;      if (MEM_P (operands[1]))
;;      {
;;        if (
;;          (
;;            MEM_P (XEXP (operands[1], 0))
;;            //|| flare32_abs_pred (XEXP (operands[1], 0), <MODE>mode)
;;            || GET_CODE (XEXP (operands[1], 0)) == SYMBOL_REF
;;            || GET_CODE (XEXP (operands[1], 0)) == LABEL_REF
;;            || GET_CODE (XEXP (operands[1], 0)) == CONST
;;            || GET_CODE (XEXP (operands[1], 0)) == CONST_INT
;;            //|| GET_CODE (XEXP (operands[1], 0)) == CONST_WIDE_INT
;;            //|| GET_CODE (XEXP (operands[1], 0)) == CONST_DOUBLE
;;          )
;;        )
;;        {
;;          if (can_create_pseudo_p ())
;;          {
;;            //operands[1] = gen_rtx_MEM (<MODE>mode,
;;            //  force_reg (Pmode,
;;            //    gen_lowpart (Pmode, XEXP (operands[1], 0))));
;;
;;            operands[1] = gen_rtx_MEM (<MODE>mode,
;;              force_reg (<MODE>mode, XEXP (operands[1], 0)));
;;          }
;;        }
;;        //else if (GET_CODE (XEXP (operands[1], 0)) == CONST_WIDE_INT)
;;        //{
;;        //  operands[1] = gen_rtx_MEM (<MODE>mode,
;;        //    force_reg (Pmode, XEXP (operands[1], 0)));
;;        //}
;;      }
;;      //else if (
;;      //  GET_CODE (operands[1]) == SYMBOL_REF
;;      //  || GET_CODE (operands[1]) == LABEL_REF
;;      //  || GET_CODE (operands[1]) == CONST
;;      //  || GET_CODE (operands[1]) == CONST_INT
;;      //)
;;      //{
;;      //  if (can_create_pseudo_p ())
;;      //  {
;;      //    operands[1] = 
;;      //  }
;;      //}
;;    }
;;  }
;;})
(define_insn "*mov64"
  [(set (match_operand:MOV64 0
    "nonimmediate_operand"
    "=r,r,r,B,r,W")
    (match_operand:MOV64 1
    "flare32_general_mov64_src_operand"
    ;;"general_operand"
    "r,i,B,r,W,r"))]
  "register_operand (operands[0], <MODE>mode)
  || register_operand (operands[1], <MODE>mode)"
{
  //return flare32_output_mov64 (operands[0], operands[1]);
  switch (which_alternative)
  {
    case 0: // =r, r
      return "cpy %L0, %L1\n\tcpy %H0, %H1\\t\\t// *mov64: =r, r";
    case 1: // =r, i
      //return "cpy %L0, #%1\n\tcpy %H0, #0\\t\\t// *mov64: =r, i";
      //return "cpy %L0, #(%1) >> 32\n\tcpy %H0, #(%1) & 0xffffffff"
      //  "\\t\\t// *mov64: =r, i";
    {
      //if (!CONST_DOUBLE_P (operands[1]))
      {
        return "cpy %L0, #(%1) & 0xffffffff\n\tcpy %H0, #(%1) >> 32"
          "\\t\\t// *mov64: =r, i";
      }
      //else // if (CONST_DOUBLE_P (operands[1]))
      //{
      //  return "cpy64 %H0, %L0, #%1\\t\\t// *mov64: =r, const_double";
      //}
    }
    //case 2: // =r, Q
    //  return "cpy %L0, #(%1) >> 32\n\tcpy %H0, #(%1) & 0xffffffff"
    //    "\\t\\t// *mov64: =r, Q";
    //case 2: // =r, F
    //  //return "cpy %L0, #(%1) >> 32\n\tcpy %H0, #(%1) & 0xffffffff"
    //  //  "\\t\\t// *mov64: =r, E";
    //  return "cpy64 %H0, %L0, #%1\\t\\t// *mov64: =r, F";
    case 2: // =r, B
      if (flare32_32_bit_offset_address_reg_plus_reg_p (operands[1]))
      {
        return "ldr %L0, [%L1, #4]\n\tldr %H0, [%L1, #0]"
          "\\t\\t// *mov64: =r, B";
      }
      else
      {
        return "ldr %L0, [%L1+4]\n\tldr %H0, [%L1+0]\\t\\t// *mov64: =r, B";
      }
    case 3: // =B, r
      if (flare32_32_bit_offset_address_reg_plus_reg_p (operands[1]))
      {
        return "str %L1, [%L0, #4]\n\tstr %H1, [%L0, #0]"
          "\\t\\t// *mov64: =B, r";
      }
      else
      {
        return "str %L1, [%L0+4]\n\tstr %H1, [%L0+0]\\t\\t// *mov64: =B, r";
      }
    case 4: // =r, W
      return "ldr %L0, [%L1, #4]\n\tldr %H0, [%L1, #0]"
        "\\t\\t// *mov64: =r, W";
    case 5: // =W, r
      return "str %L1, [%L0, #4]\n\tstr %H1, [%L0, #0]"
        "\\t\\t// *mov64: =W, r";
    default:
      gcc_unreachable ();
  }
})
;; --------
(define_expand "mov<mode>"
  [(set (match_operand:MOV16 0 "nonimmediate_operand" "")
    (match_operand:MOV16 1 "flare32_general_mov8_mov16_src_operand" ""))]
  ""
{
  //// If this is a store, force the value into a register.
  //if (MEM_P (operands[0]))
  //{
  //  operands[1] = force_reg (HImode, operands[1]);
  //}
  flare32_emit_mov (operands[0], operands[1], <MODE>mode);
  DONE;
})

(define_insn "*mov16"
  [(set (match_operand:MOV16 0 "nonimmediate_operand"
    "=r,r,r,C,r,W"
    ;;"=r,r,r,W"
    )
    (match_operand:MOV16 1 "flare32_general_mov8_mov16_src_operand"
     "r,i,C,r,W,r"
     ;;"r,i,W,r"
     ))]

  "register_operand (operands[0], <MODE>mode)
  || register_operand (operands[1], <MODE>mode)"
  ;;""
  "@
  cpy %0, %1\\t\\t// *mov16: =r, r
  cpy %0, #%1\\t\\t// *mov16: =r, i
  lduh %0, [%1]\\t\\t// *mov16: =r, C
  sth %1, [%0]\\t\\t// *mov16: =C, r
  lduh %0, [%1]\\t\\t// *mov16: =r, W
  sth %1, [%0]\\t\\t// *mov16: =W, r"
  ;;cpy %0, %1\\t\\t// *mov16: =r, S
  ;;cpy %0, %1\\t\\t// *mov16: =S, r
  ;;cpy %0, %1\\t\\t// *mov16: =S, S
)
;; --------
;;(define_expand "movqi"
;;  [(set (match_operand:QI 0 "nonimmediate_operand" "")
;;    (match_operand:QI 1 "general_operand" ""))]
;;
;;  ""
;;{
;;  // If this is a store, force the value into a register.
;;  if (MEM_P (operands[0]))
;;  {
;;    operands[1] = force_reg (QImode, operands[1]);
;;  }
;;
;;  //if (MEM_P (operands[0]) && (MEM_P (operands[1])
;;  //  || CONST_INT_P (operands[1])))
;;  //{
;;  //  if (can_create_pseudo_p ())
;;  //  {
;;  //    rtx tmp;
;;  //    
;;  //    if (MEM_P (operands[1]))
;;  //    {
;;  //      tmp = gen_reg_rtx (GET_MODE (operands[1]));
;;  //    }
;;  //    else if (CONST_INT_P (operands[1]))
;;  //    {
;;  //      tmp = gen_reg_rtx (QImode);
;;  //    }
;;  //    else
;;  //    {
;;  //      //abort ();
;;  //      FAIL;
;;  //    }
;;
;;  //    emit_insn (gen_movqi (tmp, operands[1]));
;;  //    operands[1] = tmp;
;;  //  }
;;  //}
;;  //else if (MEM_P (operands[0]))
;;  //{
;;  //  if (can_create_pseudo_p ())
;;  //  {
;;  //    if (flare32_absolute (XEXP (operands[0], 0)))
;;  //    {
;;  //      auto tmp = gen_reg_rtx (GET_MODE (operands[0]));
;;  //      emit_insn (gen_movqi (tmp, XEXP (operands[0], 0)));
;;  //      operands[0] = tmp;
;;  //    }
;;  //  }
;;  //}
;;  //else if (MEM_P (operands[1]))
;;  //{
;;  //  if (can_create_pseudo_p ())
;;  //  {
;;  //    if (flare32_absolute (XEXP (operands[1], 0)))
;;  //    {
;;  //      auto tmp = gen_reg_rtx (GET_MODE (operands[1]));
;;  //      emit_insn (gen_movqi (tmp, XEXP (operands[1], 0)));
;;  //      operands[1] = tmp;
;;  //    }
;;  //  }
;;  //}
;;
;;  //// cpy specreg, #immediate
;;  //// cpy specreg, other_specreg
;;  //else if (flare32_reg_cpyspec (operands[0]))
;;  //{
;;  //  if ((!REG_P (operands[1])) || flare32_reg_cpyspec (operands[1]))
;;  //  {
;;  //    if (can_create_pseudo_p ())
;;  //    {
;;  //      auto tmp = gen_reg_rtx (GET_MODE (operands[1]));
;;  //      emit_insn (gen_movqi (tmp, operands[1]));
;;  //      operands[1] = tmp;
;;  //    }
;;  //  }
;;  //}
;;
;;})
(define_expand "mov<mode>"
  [(set (match_operand:MOV8 0 "nonimmediate_operand" "")
    (match_operand:MOV8 1 "flare32_general_mov8_mov16_src_operand" ""))]
  ""
{
  //// If this is a store, force the value into a register.
  //if (MEM_P (operands[0]))
  //{
  //  operands[1] = force_reg (HImode, operands[1]);
  //}
  flare32_emit_mov (operands[0], operands[1], <MODE>mode);
  DONE;
})

;; Use this
(define_insn "*mov8"
  [(set (match_operand:MOV8 0 "nonimmediate_operand"
    "=r,r,r,C,r,W"
    ;;"=r,r,r,W"
    )
    (match_operand:MOV8 1 "flare32_general_mov8_mov16_src_operand"
      "r,i,C,r,W,r"
      ;;"r,i,W,r"
      ))]

  "register_operand (operands[0], <MODE>mode)
  || register_operand (operands[1], <MODE>mode)"
  ;;""
  "@
  cpy %0, %1\\t\\t// *mov8: =r, r
  cpy %0, #%1\\t\\t// *mov8: =r, i
  ldub %0, [%1]\\t\\t// *mov8: =r, C
  stb %1, [%0]\\t\\t// *mov8: =C, r
  ldub %0, [%1]\\t\\t// *mov8: =r, W
  stb %1, [%0]\\t\\t// *mov8: =W, r"
  ;;cpy %0, %1\\t\\t// *mov8: =r, S
  ;;cpy %0, %1\\t\\t// *mov8, =S, r
  ;;cpy %0, %1\\t\\t// *mov8, =S, S
)
;; --------
(define_insn "zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
    (zero_extend:SI (match_operand:HI 1 "nonimmediate_operand" "0,C,W")))]
  ""
  "@
  ze %0, #16
  lduh %0, [%1]\\t\\t// zero_extendhisi2
  lduh %0, [%1]\\t\\t// zero_extendhisi2"
)

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
    (sign_extend:SI (match_operand:HI 1 "nonimmediate_operand" "0,C,W")))]
  ""
  "@
  se %0, #16
  ldsh %0, [%1]\\t\\t// sign_extendhisi2
  ldsh %0, [%1]\\t\\t// sign_extendhisi2"
)


(define_insn "zero_extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
    (zero_extend:SI (match_operand:QI 1 "nonimmediate_operand" "0,C,W")))]
  ""
  "@
  ze %0, #8
  ldub %0, [%1]\\t\\t// zero_extendqisi2
  ldub %0, [%1]\\t\\t// zero_extendqisi2"
)

(define_insn "sign_extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
    (sign_extend:SI (match_operand:QI 1 "nonimmediate_operand" "0,C,W")))]
  ""
  "@
  se %0, #8
  ldsb %0, [%1]\\t\\t// sign_extendqisi2
  ldsb %0, [%1]\\t\\t// sign_extendqisi2"
)



;; Use this one
(define_insn "cmpdi"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:DI 0 "register_operand" "r")
      (match_operand:DI 1 "register_operand" "r")))]
  ""
  "cmp %L0, %L1\\n\\tcmpbc %H0, %H1\\t\\t// cmpdi: r, r"
)

;;(define_expand "cmpdi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 0 "register_operand" "r")
;;      (match_operand:DI 1 "register_operand" "r")))]
;; ""
;;{
;;  ;;flare32_gen_cmpdi_reg (operands[0], operands[1]);
;;  ;;DONE;
;;})
;;(define_insn "flare32_cmpsi3_carry_in"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC
;;      (zero_extend:DI (match_operand:SI 1 "register_operand" "r"))
;;      (zero_extend:DI (match_operand:SI 2 "register_operand" "r"))))
;;  (clobber (match_scratch:SI 0 "=r"))]
;;  ""
;;  "cmpbc %0, %1, %2")

;;(define_expand "cmpdi"
;;  [(parallel
;;    [(set (reg:CC REG_FLAGS)
;;      (compare:CC (match_operand:DI 0 "register_operand" "r")
;;        (match_operand:DI 1 "register_operand" "r")))
;;    ;;(clobber (match_scratch:SI 2 "=r"))
;;    (clobber (match_scratch:DI 2 "=r"))
;;    ])]
;;  ""
;;{
;;  emit_insn (gen_movdi (operands[2], operands[0]));
;;  //emit_insn (gen_movsi (operands[2], gen_lowpart (SImode, operands[0])));
;;})
;;
;;(define_insn "*cmpdi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 0 "register_operand" "r")
;;      (match_operand:DI 1 "register_operand" "r")))
;;  ;;(clobber (match_scratch:SI 2 "=r"))
;;  (clobber (match_scratch:DI 2 "=r"))]
;;  ""
;;  "sub.f %L2, %L1\n\tsbc.f %H2, %H1\\t\\t// cmpdi"
;;  ;;"cmp %2, %L1\n\tsbc.f %H0, %H1\\t\\t// cmpdi"
;;)

;;(define_expand "cmpdi"
;;  ;;[(parallel
;;    [
;;    ;;(set (match_operand:DI 0 "register_operand" "=r")
;;    ;;  (minus:DI (match_operand:DI 1 "register_operand" "%0")
;;    ;;    (match_operand:DI 2 "register_operand" "r")))
;;    ;;(set (reg:CC REG_FLAGS)
;;    ;;  (compare:CC (match_dup 0)
;;    ;;    (match_dup 2)))
;;    (set (reg:CC REG_FLAGS)
;;      (compare:CC (match_operand:DI 0 "register_operand" "=r")
;;        (match_operand:DI 1 "register_operand" "r")))
;;    ]
;;  ;;)]
;;  ""
;;{
;;  //operands[2]
;;  //emit_insn (gen_movdi (operands[2], operands[0]));
;;  //emit_insn (gen_movsi (operands[2], gen_lowpart (SImode, operands[0])));
;;  emit_insn (gen_subdi3 (operands[0], operands[0], operands[1]));
;;  DONE;
;;})
;;(define_insn "*flare32_cmpdi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 0 "register_operand" "r")
;;      (match_operand:DI 1 "register_operand" "r")))
;;  )]
;;  ""
;;  "sub.f %L2, %L1\n\tsbc.f %H2, %H1\\t\\t// cmpdi"
;;  ;;"cmp %2, %L1\n\tsbc.f %H0, %H1\\t\\t// cmpdi"
;;)

;;(define_insn "cmpdi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 0 "register_operand" "=r")
;;      (match_operand:DI 1 "register_operand" "r")))]
;;  ""
;;  "sub.f %L0, %L1\n\tsbc.f %H0, %H1\\t\\t// cmpdi"
;;)

;;(define_expand "cmpdi"
;;  [(parallel
;;    [(set (match_operand:DI 0 "register_operand")
;;      (minus:DI
;;        (match_operand:DI 1 "register_operand")
;;        (match_operand:DI 2 "register_operand")))
;;    (set (reg:CC REG_FLAGS)
;;      (compare:CC (match_dup 0) (match_dup 2)))])]
;;  ""
;;{
;;})
;;(define_insn "*cmpdi"
;;  [(set (match_operand:DI 0 "register_operand" "=r")
;;      (minus:DI
;;        (match_operand:DI 1 "register_operand" "%0")
;;        (match_operand:DI 2 "register_operand" "r")))
;;    (set (reg:CC REG_FLAGS)
;;      (compare:CC (match_dup 0) (match_dup 2)))]
;;  ""
;;  "sub.f %L0, %L2\n\tsbc.f %H0, %21\\t\\t// cmpdi"
;;)

;;(define_insn "cmpdi"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 0 "register_operand" "r")
;;      (match_operand:DI 1 "nonmemory_operand" "r")))]
;;  ""
;;  "cmpd %0, %1\\t\\t// cmpdi: r r")
(define_insn "cmpsi"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:SI 0 "register_operand" "r,r")
      (match_operand:SI 1 "nonmemory_operand" "r,i")))]
  ""
  "@
  cmp %0, %1\\t\\t// cmpsi: r, r
  cmp %0, #%1\\t\\t// cmpsi: r, i")
(define_insn "cmphi"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:HI 0 "register_operand" "r")
      (match_operand:HI 1 "register_operand" "r")))]
  ""
  "cmph %0, %1\\t\\t// cmphi: r, r")
(define_insn "cmpqi"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:QI 0 "register_operand" "r")
      (match_operand:QI 1 "register_operand" "r")))]
  ""
  "cmpb %0, %1\\t\\t// cmpqi: r, r")

;;(define_expand "cbranchdi4"
;;  [(set (match_operand:DI 1 "register_operand" "=r")
;;    (minus:DI
;;      (match_operand:DI 2 "register_operand" "%0")
;;      (match_operand:DI 3 "register_operand" "r")))
;;  (set (reg:CC REG_FLAGS)
;;    (compare:CC (match_dup 1) (match_dup 3)))
;;  (set (pc)
;;    (if_then_else (match_operator 0 "comparison_operator"
;;      [(reg:CC REG_FLAGS) (const_int 0)])
;;      (label_ref (match_operand 4 "" ""))
;;      (pc)))]
;;  ""
;;  ""
;;)

;;(define_expand "cbranchdi4"
;;  [(set (reg:CC REG_FLAGS)
;;    (compare:CC (match_operand:DI 1 "register_operand" "r")
;;      (match_operand:DI 2 "register_operand" "r")))
;;    (set (pc)
;;      (if_then_else (match_operator 0 "comparison_operator"
;;        [(reg:CC REG_FLAGS) (const_int 0)])
;;        (label_ref (match_operand 3 "" ""))
;;        (pc)))]
;;  ""
;;  ""
;;)

(define_expand "cbranchdi4"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:DI 1 "register_operand" "r")
      (match_operand:DI 2 "register_operand" "r")))
    (set (pc)
      (if_then_else (match_operator 0 "comparison_operator"
        [(reg:CC REG_FLAGS) (const_int 0)])
        (label_ref (match_operand 3 "" ""))
        (pc)))]
  ""
  ""
)
(define_expand "cbranchsi4"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:SI 1 "register_operand" "r,r")
      (match_operand:SI 2 "nonmemory_operand" "r,i")))
    (set (pc)
      (if_then_else (match_operator 0 "comparison_operator"
        [(reg:CC REG_FLAGS) (const_int 0)])
        (label_ref (match_operand 3 "" ""))
        (pc)))]
  ""
{
  // Force the things being compared into registers (or an immediate)
  //if ((GET_CODE (operands[1]) != REG)
  //  || (REGNO_REG_CLASS (REGNO (operands[1])) != GENERAL_REGS))
  //{
  //  //operands[1] = force_reg (SImode, operands[1]);
  //}
  ////if (GET_CODE (operands[2]) != REG)
  ////{
  ////  operands[2] = force_reg (SImode, operands[2]);
  ////}
  //if ((GET_CODE (operands[2]) != REG)
  //  || (GET_CODE (operands[2]) != CONST_INT))
  //{
  //  operands[2] = force_reg (SImode, operands[2]);
  //}
})
(define_expand "cbranchhi4"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:HI 1 "register_operand" "r")
      (match_operand:HI 2 "register_operand" "r")))
    (set (pc)
      (if_then_else (match_operator 0 "comparison_operator"
        [(reg:CC REG_FLAGS) (const_int 0)])
        (label_ref (match_operand 3 "" ""))
        (pc)))]
  ""
  ""
)
(define_expand "cbranchqi4"
  [(set (reg:CC REG_FLAGS)
    (compare:CC (match_operand:QI 1 "register_operand" "r")
      (match_operand:QI 2 "register_operand" "r")))
    (set (pc)
      (if_then_else (match_operator 0 "comparison_operator"
        [(reg:CC REG_FLAGS) (const_int 0)])
        (label_ref (match_operand 3 "" ""))
        (pc)))]
  ""
  ""
)


(define_insn "b<cond:code>"
  [(set (pc)
    (if_then_else (cond (reg:CC REG_FLAGS) (const_int 0))
      (label_ref (match_operand 0 "" ""))
      (pc)))]
  ""
  "b<CC> %l0")


(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "register_operand" "r"))]
  ""
  "jmp %0")

(define_insn "jump"
  [(set (pc) (label_ref (match_operand 0)))]
  ""
  "bra %l0")


;;(define_expand "call"
;;  [(parallel [(call (mem:HI (match_operand 0 "nonmemory_operand" ""))
;;      (match_operand 1 ""))
;;    ;;(match_operand 2 "")
;;    (clobber (reg:SI REG_LR))])]
;;  ""
;;{
;;  gcc_assert (MEM_P (operands[0]));
;;})
(define_expand "call"
  [(parallel [(call (match_operand:HI 0 "memory_operand" "")
      (match_operand 1 ""))
    ;;(match_operand 2 "")
    ;;(use (match_operand 2 ""))
    (clobber (reg:SI REG_LR))])]
  ""
{
  gcc_assert (MEM_P (operands[0]));
})
(define_insn "*call"
  [(call (mem:HI (match_operand:SI 0 "nonmemory_operand" "i,r"))
      (match_operand 1 ""))
    ;;(match_operand 2 "")
    ;;(use (match_operand 2 ""))
    (clobber (reg:SI REG_LR))]
  ""
  "@
  bl %0\\t\\t// *call: i
  jl %0\\t\\t// *call: r")


(define_expand "call_value"
  [(parallel [(set (match_operand 0 "register_operand" "")
    (call (match_operand:HI 1 "memory_operand" "")
      (match_operand 2 "")))
    ;;(match_operand 3 "")
    ;;(use (match_operand 3 ""))
    (clobber (reg:SI REG_LR))])]
  ""
{
  gcc_assert (MEM_P (operands[1]));
  //flare32_dbg_call_value (operands[0], operands[1], operands[2]);
  //DONE;
})
;;
;;(define_expand "call_value"
;;  [(parallel [(set (match_operand 0 "" "")
;;    (call (mem:HI (match_operand 1 "nonmemory_operand" ""))
;;      (match_operand 2 "")))
;;    (clobber (reg:SI REG_LR))])]
;;  ""
;;{
;;  gcc_assert (MEM_P (operands[1]));
;;  //if (MEM_P (XEXP (operands[1], 0)))
;;  //{
;;  //  if (can_create_pseudo_p ())
;;  //  {
;;  //    rtx tmp = force_reg (GET_MODE (XEXP (operands[1], 0)),
;;  //      XEXP (operands[1], 0));
;;  //    operands[1] = gen_rtx_MEM (GET_MODE (operands[1])
;;  //  }
;;  //}
;;})

;;(define_insn "*call_value"
;;  [(set (match_operand 0 "register_operand" "=r,r")
;;    (call (mem:HI (match_operand 1 "nonmemory_operand" "i,r"))
;;      (match_operand 2 "")))
;;    ;;(match_operand 3 "")
;;    (clobber (reg:SI REG_LR))]
;;  ""
;;  "@
;;  bl %1\\t\\t// *call_value: =r, i
;;  jl %1\\t\\t// *call_value: =r, i")
(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r,r")
    (call (mem:HI (match_operand:SI 1 "address_operand" "i,r"))
      (match_operand 2 "")))
    ;;(match_operand 3 "")
    ;;(use (match_operand 3 ""))
    (clobber (reg:SI REG_LR))]
  ""
  "@
  bl %1\\t\\t// *call_value: =r, i
  jl %1\\t\\t// *call_value: =r, r"
)

;;(define_insn "*call_value_indirect"
;;  [(set (match_operand 0 "register_operand" "=r")
;;    (call (mem:HI (match_operand:SI 1 "register_operand" "r"))
;;      (match_operand 2 "" "")))
;;      (clobber (reg:SI REG_LR))]
;;  ""
;;  "jl %1\\t\\t// *call_value_indirect: =r, r")

;; borrowed from RISC-V
;;(define_insn "stack_tie<mode>"
;;  [(set (mem:BLK (scratch))
;;    (unspec:BLK [(match_operand:X 0 "register_operand" "r")
;;      (match_operand:X 1 "register_operand" "r")]
;;      UNSPEC_TIE))]
;;  ""
;;  ""
;;  [(set_attr "length" "0")]
;;)

;; --------------
;; Prologue
;; --------------
(define_expand "prologue"
  [(clobber (const_int 0))]
  ""
  "
{
  flare32_expand_prologue ();
  DONE;
}")

;; --------------
;; Epilogue
;; --------------
(define_expand "epilogue"
  [(return)]
  ""
  "
{
  flare32_expand_epilogue ();
  DONE;
}")

(define_insn
  "returner"
  [(return)]
  "reload_completed"
  "jmp lr")
;;(define_insn
;;  "return"
;;  [(return)]
;;  "reload_completed"
;;  "jmp lr")
;;(define_insn
;;  "simple_return"
;;  [(return)]
;;  "reload_completed"
;;  "jmp lr")
