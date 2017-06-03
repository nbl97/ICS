#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	if(op_src->type == OP_TYPE_IMM) {
		cpu.eip += (DATA_TYPE_S)op_src->val;
		if(DATA_BYTE == 2) {
			cpu.eip = cpu.eip & 0x0000ffff;
		}
	}
	print_asm_template1();
}

make_helper(concat(jmp_rm_, SUFFIX)) {
	concat(decode_rm_, SUFFIX)(eip + 1);
	if(DATA_BYTE == 2) {
		cpu.eip = op_src->val & 0x0000ffff;
	}
	else {
		cpu.eip = op_src->val;
	}
	print_asm_template1();
	return 0;
}

make_instr_helper(i)
#if DATA_BYTE == 1 || DATA_BYTE == 4
	make_instr_helper(si)
#endif

#include "cpu/exec/template-end.h"
