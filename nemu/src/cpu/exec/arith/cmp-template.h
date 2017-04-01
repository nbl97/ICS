#include "cpu/exec/template-start.h"
#define instr cmp
static void do_execute() {
	DATA_TYPE res = op_dest->val - op_src->val;
	cpu.EFLAGS.CF = (op_dest->val < op_src->val) ? 1 : 0;
	if((op_dest->val > 0 && op_src->val < 0 && res < 0) || 
	   (op_dest->val < 0 && op_src->val > 0 && res > 0))
		cpu.EFLAGS.OF = 1;
	else
		cpu.EFLAGS.OF = 0;
	cpu.EFLAGS.PF = !(res & 1);
	cpu.EFLAGS.ZF = (res == 0) ? 1 : 0;
	cpu.EFLAGS.SF = (res >= 0) ? 1 : 0;
	
	print_asm_template2();
}
make_instr_helper(si2rm)
#include "cpu/exec/template-end.h"