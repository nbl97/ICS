#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	DATA_TYPE answer;
	answer = op_dest->val - op_src->val;

	if((MSB(op_dest->val) != MSB(op_src->val)) && (MSB(op_src->val) == MSB(answer))) {
		cpu.EFLAGS.OF = 1;
	}
	else {
		cpu.EFLAGS.OF = 0;
	}
	cpu.EFLAGS.SF = MSB(answer);
	if(answer == 0) {
		cpu.EFLAGS.ZF = 1;
	}
	else {
		cpu.EFLAGS.ZF = 0;
	}
	if(op_dest->val < op_src->val) {
		cpu.EFLAGS.CF = 1;
	}
	else {
		cpu.EFLAGS.CF = 0;
	}
	uint8_t a = answer & 0x000000ff;
	int i, count = 0;
	for(i = 0; i < 8; i++) {
		if(MSB(a) == 1) {
			count++;
		}
		a = a << 1;
	}
	if(count % 2 == 0) {
		cpu.EFLAGS.PF = 1;
	}
	else {
		cpu.EFLAGS.PF = 0;
	}
	OPERAND_W(op_dest, answer);
	print_asm_template2();
}

make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
make_instr_helper(i2a)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"
