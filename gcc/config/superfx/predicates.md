;; Predicate definitions for SuperFX
;; Copyright (C) 2025 Free Software Foundation, Inc.

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

;; ---------------
;; Predicates
;; ---------------

;; adapted from "../rs6000/predicates.md"
(define_predicate "superfx_general_mov_src_operand"
  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
{
  // Any (MEM LABEL_REF) is OK. That is a pc-relative load.
  //if (MEM_P (op) && GET_CODE (XEXP (op, 0)) == LABEL_REF)
  //{
  //  return 1;
  //}
  if (MEM_P (op)
    && (
      GET_CODE (XEXP (op, 0)) == SYMBOL_REF
      || GET_CODE (XEXP (op, 0)) == LABEL_REF
      || GET_CODE (XEXP (op, 0)) == CONST
      || GET_CODE (XEXP (op, 0)) == CONST_INT
    )
  )
  {
    return 1;
  }

  if (MEM_P (op)
    && GET_CODE (XEXP (op, 0)) == PLUS
    && GET_CODE (XEXP (XEXP (op, 0), 0)) == REG
    && (
      //GET_CODE (XEXP (XEXP (op, 0), 1)) == REG
      GET_CODE (XEXP (XEXP (op, 0), 1)) == SYMBOL_REF
      || GET_CODE (XEXP (XEXP (op, 0), 1)) == LABEL_REF
      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST
      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
    )
  )
  {
    return 1;
  }

  return general_operand (op, mode);
})

;;(define_predicate "superfx_general_mov32_src_operand"
;;  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
;;  ;;(match_code "mem,const_int,const_double,reg,subreg,symbol_ref,label_ref,const")
;;  ;;(match_operand 0 "general_operand")
;;{
;;  // Any (MEM LABEL_REF) is OK. That is a pc-relative load.
;;  //if (MEM_P (op) && GET_CODE (XEXP (op, 0)) == LABEL_REF)
;;  //{
;;  //  return 1;
;;  //}
;;  if (MEM_P (op)
;;    && (
;;      GET_CODE (XEXP (op, 0)) == SYMBOL_REF
;;      || GET_CODE (XEXP (op, 0)) == LABEL_REF
;;      || GET_CODE (XEXP (op, 0)) == CONST
;;      || GET_CODE (XEXP (op, 0)) == CONST_INT
;;    )
;;  )
;;  {
;;    return 1;
;;  }
;;
;;  if (MEM_P (op)
;;    && GET_CODE (XEXP (op, 0)) == PLUS
;;    && GET_CODE (XEXP (XEXP (op, 0), 0)) == REG
;;    && (
;;      GET_CODE (XEXP (XEXP (op, 0), 1)) == REG
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == SYMBOL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == LABEL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
;;    )
;;  )
;;  {
;;    return 1;
;;  }
;;  if (MEM_P (op)
;;    && GET_CODE (XEXP (op, 0)) == PLUS
;;    && GET_CODE (XEXP (XEXP (op, 0), 0)) == PLUS
;;    && GET_CODE (XEXP (XEXP (XEXP (op, 0), 0), 0)) == REG
;;    && GET_CODE (XEXP (XEXP (XEXP (op, 0), 0), 1)) == REG
;;    && (
;;      GET_CODE (XEXP (XEXP (op, 0), 1)) == SYMBOL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == LABEL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
;;    )
;;  )
;;  {
;;    return 1;
;;  }
;;      
;;  if (MEM_P (op)
;;    && (
;;      GET_CODE (XEXP (op, 0)) == REG
;;      //|| GET_CODE (XEXP (op, 0)) == SUBREG
;;    )
;;  )
;;  {
;;    return 1;
;;  }
;;
;;
;;  //if (MEM_P (op)
;;  //  //&& superfx_absolute_w_ci (XEXP (op, 0))
;;  //  && (
;;  //    GET_CODE (op) == SYMBOL_REF
;;  //    || GET_CODE (op) == LABEL_REF
;;  //    || GET_CODE (op) == CONST
;;  //    || GET_CODE (op) == CONST_INT
;;  //  )
;;  //)
;;  //{
;;  //  //return 0;
;;  //  return 1;
;;  //}
;;  //if (CONST_DOUBLE_P (op))
;;  //{
;;  //  return 1;
;;  //}
;;
;;  return general_operand (op, mode);
;;  //return 1;
;;})

;;(define_predicate "superfx_general_mov64_src_operand"
;;  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const"))
;;(define_predicate "superfx_general_mov64_src_operand"
;;  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
;;  ;;(match_code "mem,const_int,const_double,reg,subreg,symbol_ref,label_ref,const")
;;  ;;(match_operand 0 "general_operand")
;;{
;;  if (MEM_P (op)
;;    && (
;;      GET_CODE (XEXP (op, 0)) == SYMBOL_REF
;;      || GET_CODE (XEXP (op, 0)) == LABEL_REF
;;      || GET_CODE (XEXP (op, 0)) == CONST
;;      || GET_CODE (XEXP (op, 0)) == CONST_INT
;;    )
;;  )
;;  {
;;    return 1;
;;  }
;;
;;  if (MEM_P (op)
;;    && GET_CODE (XEXP (op, 0)) == PLUS
;;    && GET_CODE (XEXP (XEXP (op, 0), 0)) == REG
;;  )
;;  {
;;    if (
;;      //superfx_mov_src_valid_index_reg (XEXP (XEXP (op, 0), 1), mode)
;;      GET_CODE (XEXP (XEXP (op, 0), 1)) == REG
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == SYMBOL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == LABEL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
;;    )
;;    {
;;      return 1;
;;    }
;;  }
;;  if (MEM_P (op)
;;    && GET_CODE (XEXP (op, 0)) == PLUS
;;    && GET_CODE (XEXP (XEXP (op, 0), 0)) == PLUS
;;    && GET_CODE (XEXP (XEXP (XEXP (op, 0), 0), 0)) == REG
;;    && GET_CODE (XEXP (XEXP (XEXP (op, 0), 0), 1)) == REG
;;    && (
;;      GET_CODE (XEXP (XEXP (op, 0), 1)) == SYMBOL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == LABEL_REF
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST
;;      || GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT
;;    )
;;  )
;;  {
;;    return 1;
;;  }
;;
;;  if (MEM_P (op)
;;    && GET_CODE (XEXP (op, 0)) == REG
;;  )
;;  {
;;    return 1;
;;  }
;;
;;  //if (CONST_WIDE_INT_P (op))
;;  //{
;;  //  return 1;
;;  //}
;;  //if (CONST_DOUBLE_P (op))
;;  //{
;;  //  //if (CONST_DOUBLE_AS_INT_P (op))
;;  //  //{
;;  //    return 1;
;;  //  //}
;;  //  //else
;;  //  //{
;;  //  //  return 0;
;;  //  //}
;;  //}
;;
;;  //if (CONST_DOUBLE_P (op))
;;  //{
;;  //  return 1;
;;  //}
;;
;;  return general_operand (op, mode);
;;  //return 1;
;;})

;;(define_predicate "superfx_general_mov8_mov16_src_operand"
;;  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
;;)
;;(define_predicate "superfx_general_mov8_mov16_src_operand"
;;  (match_code "mem,const_int,reg,subreg,symbol_ref,label_ref,const")
;;{
;;  // Any (MEM LABEL_REF) is OK. That is a pc-relative load.
;;  //if (MEM_P (op) && GET_CODE (XEXP (op, 0)) == LABEL_REF)
;;  //{
;;  //  return 1;
;;  //}
;;  if (MEM_P (op)
;;    && (
;;      GET_CODE (XEXP (op, 0)) == SYMBOL_REF
;;      || GET_CODE (XEXP (op, 0)) == LABEL_REF
;;      || GET_CODE (XEXP (op, 0)) == CONST
;;      || GET_CODE (XEXP (op, 0)) == CONST_INT
;;    )
;;  )
;;  {
;;    return 1;
;;  }
;;
;;  if (MEM_P (op)
;;    && GET_CODE (XEXP (op, 0)) == PLUS
;;    && GET_CODE (XEXP (XEXP (op, 0), 0)) == REG
;;    && GET_CODE (XEXP (XEXP (op, 0), 1)) == REG
;;  )
;;  {
;;    //if (
;;    //  (
;;    //    GET_CODE (XEXP (XEXP (op, 0), 1)) == REG
;;    //    //|| GET_CODE (XEXP (XEXP (op, 0), 1)) == SUBREG
;;    //  )
;;    //  //&& GET_MODE (XEXP (XEXP (op, 0), 1)) == Pmode
;;    //  //&& REGNO_OK_FOR_INDEX_P (REGNO (XEXP (XEXP (op, 0), 1)))
;;    //)
;;    {
;;      return 1;
;;    }
;;  }
;;
;;  if (MEM_P (op) && GET_CODE (XEXP (op, 0)) == REG)
;;  {
;;    return 1;
;;  }
;;
;;  //if (superfx_absolute_w_ci (op))
;;  //{
;;  //  return 1;
;;  //}
;;  //if (//superfx_absolute_w_ci (op)
;;  //  GET_CODE (op) == SYMBOL_REF
;;  //  || GET_CODE (op) == LABEL_REF
;;  //  || GET_CODE (op) == CONST
;;  //  || GET_CODE (op) == CONST_INT
;;  //)
;;  //{
;;  //  return 1;
;;  //}
;;
;;  return general_operand (op, mode);
;;})
