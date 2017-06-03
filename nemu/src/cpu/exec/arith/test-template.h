#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	DATA_TYPE answer;
	answer = op_dest->val & op_src->val;
	cpu.EFLAGS.CF = 0;
	cpu.EFLAGS.OF = 0;
	cpu.EFLAGS.SF = MSB(answer);
 	if(answer == 0) {
		cpu.EFLAGS.ZF = 1;
	}
	else {
		cpu.EFLAGS.ZF = 0;
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
print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(r2rm)
make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"
