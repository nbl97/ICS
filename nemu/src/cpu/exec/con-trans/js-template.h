#include "cpu/exec/template-start.h"

#define instr js

static void do_execute() {
	if(cpu.EFLAGS.SF == 1) {
		cpu.eip += (DATA_TYPE_S)op_src->val;
		if(DATA_BYTE == 2) {
			cpu.eip = cpu.eip & 0x0000ffff;
		}
	}
	print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
