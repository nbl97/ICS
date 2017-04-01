#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {

	DATA_TYPE res = op_dest->val & op_src->val;
	cpu.EFLAGS.CF=0;
	cpu.EFLAGS.OF=0;
	cpu.EFLAGS.SF = MSB(res);
	cpu.EFLAGS.ZF = (res == 0) ? 1 : 0;
	
	print_asm_template2();
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"