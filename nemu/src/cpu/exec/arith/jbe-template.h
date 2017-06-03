#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute() {
	if(cpu.EFLAGS.CF == 1 || cpu.EFLAGS.ZF == 1) {
		cpu.eip += (DATA_TYPE_S)op_src->val;
		if(DATA_BYTE == 2) {
			cpu.eip = cpu.eip & 0x0000ffff;
		}
	}
	print_asm_template1();
}

make_instr_helper(i)
#if DATA_BYTE == 1 || DATA_BYTE == 4
	make_instr_helper(si)
#endif

#include "cpu/exec/template-end.h"
