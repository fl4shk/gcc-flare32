;; Machine description for SuperFX
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

;;(define_code_iterator cond [eq ne geu ltu gtu leu ge lt gt le])
;;(define_code_attr CC [(eq "eq") (ne "ne") (geu "geu") (ltu "ltu")
;;  (gtu "gtu") (leu "leu") (ge "ges") (lt "lts") (gt "gts") (le "les")])

(define_mode_iterator MOV8 [QI])
(define_mode_iterator MOV16 [HI])
;;(define_mode_iterator MOV32 [SI SQ USQ SA USA SF SD])
;;(define_mode_iterator MOV32 [SI SF])
(define_mode_iterator MOV32 [SI])
;;(define_mode_iterator MOV64 [DI DF DD DQ UDQ])
;;(define_mode_iterator MOV64 [DI DF])
(define_mode_iterator MOV64 [DI])

(define_code_attr US [(sign_extend "s") (zero_extend "u")])
(define_code_attr Us [(sign_extend "") (zero_extend "u")])
(define_code_iterator EXTEND [sign_extend zero_extend])
