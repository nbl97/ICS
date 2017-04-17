#include "cpu/exec/template-start.h"

#define instr cltd

make_helper(concat(cltd_, SUFFIX)) {
	if(DATA_BYTE == 2) {
		if((DATA_TYPE_S)REG(R_AX) < 0) {
			cpu.edx = 0x0ffff;
		}
		else {
			cpu.edx = 0;
		}
	}
	else if(DATA_BYTE == 4) {
		if((DATA_TYPE_S)cpu.eax < 0) {
			cpu.edx = 0xffffffff;
		}
		else {
			cpu.edx = 0;
		}
	}
	print_asm("cltd");
	return 1;
}

#include "cpu/exec/template-end.h"
