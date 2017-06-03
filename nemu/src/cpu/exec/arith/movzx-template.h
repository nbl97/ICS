#include "cpu/exec/template-start.h"

#define instr movzx

static void do_execute() {
	DATA_TYPE_S a = (int)(op_src->val & 0xffff);
	OPERAND_W(op_dest, a);
	print_asm_template2();
}

make_helper(concat(movzx_, SUFFIX)) {
	return idex(eip, concat(decode_rm2r_,b), do_execute);
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
