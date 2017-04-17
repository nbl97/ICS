#include "cpu/exec/template-start.h"

make_helper(concat(movs_, SUFFIX)) {
	int num = 0;
	DATA_TYPE a = MEM_R(cpu.esi);
	MEM_W(cpu.edi, a);
	if(cpu.EFLAGS.DF == 0) {
		num = (int)DATA_BYTE;
	}
	else {
		num = -(int)DATA_BYTE;
	}
	cpu.esi += num;
	cpu.edi += num;
	print_asm("movs");
	return 1;
}

#include "cpu/exec/template-end.h"
