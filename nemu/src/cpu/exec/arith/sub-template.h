#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute () {
	DATA_TYPE res = op_dest->val - op_src->val;
	op_dest -> val = res;
	if(res == 0)	cpu.EFLAGS.ZF = 1;
	else if(res >= 0){
		cpu.EFLAGS.SF = cpu.EFLAGS.CF = 0;
	}else{
		cpu.EFLAGS.SF = cpu.EFLAGS.CF = 1;
	}
	if(op_dest->val < 0 && op_src->val > 0 && res < 0)
		cpu.EFLAGS.OF = 1;
	else	cpu.EFLAGS.OF = 0;
	print_asm_template2();
}

make_instr_helper(si2rm)

#include "cpu/exec/template-end.h"