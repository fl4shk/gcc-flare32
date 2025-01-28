extern int superfx_initial_elimination_offset (int from, int to);
//extern bool superfx_32_bit_offset_address_p (rtx x);

//extern bool superfx_32_bit_offset_address_reg_plus_reg_p (rtx x);
extern bool superfx_offset_address_reg_plus_imm_p (rtx x);
//extern bool superfx_32_bit_offset_address_reg_plus_reg_plus_imm_p (rtx x);
//extern bool superfx_8_or_16_bit_offset_address_p (rtx x);
extern bool superfx_non_offset_address_p (rtx x);
//extern bool superfx_non_offset_address_p (rtx x);

extern bool superfx_absolute (rtx x);
extern bool superfx_absolute_ex (rtx x);
//extern bool superfx_absolute_plus_ci (rtx x);

//extern bool superfx_reg_cpyspec (rtx x);
//extern bool superfx_reg_class_cpyspec (reg_class x);
extern bool superfx_reg_general (rtx x);
extern bool superfx_reg_class_general (reg_class x);


extern bool superfx_regno_actually_callee_saved (int regno);
//extern void superfx_compute_frame ();

extern void superfx_expand_prologue ();
extern void superfx_expand_epilogue ();

//extern void superfx_expand_mov32 (rtx& dst, rtx& src, machine_mode mode);
//extern bool superfx_expand_mov64 (rtx& dst, rtx& src, machine_mode mode);
//extern bool superfx_legitimize_movdi (rtx dst, rtx src);
//extern const char* superfx_output_movqi (rtx dst, rtx src);
//extern const char* superfx_output_movhi (rtx dst, rtx src);
//extern void superfx_emit_mov8 (rtx dst, rtx src, machine_mode mode);
//extern void superfx_emit_mov16 (rtx dst, rtx src, machine_mode mode);
//extern void superfx_emit_mov32 (rtx dst, rtx src, machine_mode mode);
extern void superfx_emit_mov (rtx dst, rtx src, machine_mode mode);
//extern const char* superfx_output_movdi (rtx dst, rtx src);
//extern void superfx_dbg_call_value (rtx retop, rtx target, rtx op2);
extern void superfx_gen_cmpdi_reg (rtx x, rtx y);
extern rtx superfx_get_return_address (int count);
//extern const char* superfx_expand_conditional_branch (rtx *operands);

//extern int superfx_stack_pointer_offset ();
//extern int superfx_incoming_frame_sp_offset ();
