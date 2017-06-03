#include "cpu/exec/template-start.h"

#define instr cmova

static void do_execute() {
	if(cpu.EFLAGS.ZF == 0 && cpu.EFLAGS.CF == 0) {
		DATA_TYPE a = op_src->val;
		OPERAND_W(op_dest, a);
	}
	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
