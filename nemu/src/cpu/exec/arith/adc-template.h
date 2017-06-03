#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute() {
	uint32_t t, answer;
	t = op_src->val + cpu.EFLAGS.CF;
	answer = op_dest->val + t;

	if((MSB(op_dest->val) == MSB(t)) && (MSB(t) != MSB(answer))) {
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
	if((MSB(op_dest->val) == 1) && (MSB(t) == 1)) {
		cpu.EFLAGS.CF = 1;
	}
	else if(((MSB(op_dest->val) ^ MSB(t)) == 1) && (MSB(answer) == 0)) {
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
