#include "cpu/exec/template-start.h"

make_helper(concat(stos_, SUFFIX)) {
	if(DATA_BYTE == 1) {
		MEM_W(cpu.edi, REG(R_AL));
	}
	else {
		MEM_W(cpu.edi, cpu.eax);
	}
	if(cpu.EFLAGS.DF == 0) {
		cpu.edi += DATA_BYTE;
	}
	else {
		cpu.edi -= DATA_BYTE;
	}
	print_asm("stos");
	return 1;
}

#include "cpu/exec/template-end.h"
