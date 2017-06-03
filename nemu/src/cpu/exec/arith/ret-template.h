#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_, SUFFIX)) {
	if(DATA_BYTE == 2) {
		cpu.ip = MEM_R(cpu.esp);
		cpu.eip = cpu.eip & 0x0000ffff;
	}
	else if(DATA_BYTE == 4) {
		cpu.eip = MEM_R(cpu.esp);
	}
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	return 0;
}

make_helper(concat(ret_i_, SUFFIX)) {
	cpu.eip = MEM_R(cpu.esp);
	cpu.esp += 4;
	cpu.esp += instr_fetch(eip+1, 2);
	print_asm("ret 0x%x", instr_fetch(eip+1, 2));
	return 0;
}

#include "cpu/exec/template-end.h"
