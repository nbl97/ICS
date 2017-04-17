#include "cpu/exec/template-start.h"

make_helper(concat(cmps_, SUFFIX)) {
	int num = 0;
	if(cpu.EFLAGS.DF == 0) {
		num = (int)DATA_BYTE;
	}
	else {
		num = -(int)DATA_BYTE;
	}
	cpu.esi += num;
	cpu.edi += num;
	DATA_TYPE m = MEM_R(cpu.esi);
	DATA_TYPE n = MEM_R(cpu.edi);
	DATA_TYPE answer = m - n;
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

	print_asm("cmps");
	return 1;
}

#include "cpu/exec/template-end.h"
