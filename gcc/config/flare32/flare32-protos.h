
extern int flare32_initial_elimination_offset (int from, int to);
extern bool flare32_32_bit_offset_address_p (rtx x);

extern bool flare32_32_bit_offset_address_reg_plus_reg_p (rtx x);
extern bool flare32_32_bit_offset_address_reg_plus_imm_p (rtx x);
extern bool flare32_32_bit_offset_address_reg_plus_reg_plus_imm_p (rtx x);
extern bool flare32_8_or_16_bit_offset_address_p (rtx x);
extern bool flare32_non_offset_address_p (rtx x);
//extern bool flare32_non_offset_address_p (rtx x);

extern bool flare32_absolute (rtx x);
extern bool flare32_absolute_ex (rtx x);
//extern bool flare32_absolute_plus_ci (rtx x);

//extern bool flare32_reg_cpyspec (rtx x);
//extern bool flare32_reg_class_cpyspec (reg_class x);
extern bool flare32_reg_general (rtx x);
extern bool flare32_reg_class_general (reg_class x);


extern bool flare32_regno_actually_callee_saved (int regno);
//extern void flare32_compute_frame ();

extern void flare32_expand_prologue ();
extern void flare32_expand_epilogue ();

//extern void flare32_expand_mov32 (rtx& dst, rtx& src, machine_mode mode);
//extern bool flare32_expand_mov64 (rtx& dst, rtx& src, machine_mode mode);
//extern bool flare32_legitimize_movdi (rtx dst, rtx src);
//extern const char* flare32_output_movqi (rtx dst, rtx src);
//extern const char* flare32_output_movhi (rtx dst, rtx src);
//extern void flare32_emit_mov8 (rtx dst, rtx src, machine_mode mode);
//extern void flare32_emit_mov16 (rtx dst, rtx src, machine_mode mode);
//extern void flare32_emit_mov32 (rtx dst, rtx src, machine_mode mode);
extern void flare32_emit_mov (rtx dst, rtx src, machine_mode mode);
//extern const char* flare32_output_movdi (rtx dst, rtx src);
//extern void flare32_dbg_call_value (rtx retop, rtx target, rtx op2);
extern void flare32_gen_cmpdi_reg (rtx x, rtx y);
extern rtx flare32_get_return_address (int count);

//extern int flare32_stack_pointer_offset ();
//extern int flare32_incoming_frame_sp_offset ();
