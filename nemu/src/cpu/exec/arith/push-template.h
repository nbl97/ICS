#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	if(DATA_BYTE == 1) {
		cpu.esp -= 4;
		uint32_t a = op_src->val;
		swaddr_write(cpu.esp, 4, a);
	}
	else if(DATA_BYTE == 2) {
		cpu.esp -= 2;
		swaddr_write(cpu.esp, 2, op_src->val);
	}
	else if(DATA_BYTE == 4){
		cpu.esp -= 4;
		swaddr_write(cpu.esp, 4, op_src->val);
	}
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)
make_instr_helper(i)

#include "cpu/exec/template-end.h"
