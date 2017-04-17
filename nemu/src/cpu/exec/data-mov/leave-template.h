#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat(leave_, SUFFIX)) {
	cpu.esp = cpu.ebp;
	if(DATA_BYTE == 2) {
		cpu.ebp = MEM_R(cpu.esp) & 0x0000ffff;
	}
	else if(DATA_BYTE == 4) {
		cpu.ebp = MEM_R(cpu.esp);
	}
	cpu.esp += DATA_BYTE;
	printf("~~~%x\n",cpu.eip);
	print_asm("leave");
	return 1;
}

#include "cpu/exec/template-end.h"
