#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE answer = op_src->val + 1;

	/* TODO: Update EFLAGS. */
//	panic("please implement me");
	if((MSB(op_src->val) == MSB(1)) && (MSB(1) != MSB(answer))) {
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
	if((MSB(op_src->val) == 1) && (MSB(1) == 1)) {
		cpu.EFLAGS.CF = 1;
	}
	else if(((MSB(op_src->val) ^ MSB(1)) == 1) && (MSB(answer) == 0)) {
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

	OPERAND_W(op_src, answer);
	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
