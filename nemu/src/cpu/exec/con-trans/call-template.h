#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	if(op_src->type == OP_TYPE_IMM) {
		cpu.esp -= DATA_BYTE;
		if(DATA_BYTE == 2) {
			MEM_W(cpu.esp, cpu.ip + DATA_BYTE + 1);
			cpu.eip = (cpu.eip + op_src->val) & 0x0000ffff;
		}
		else if(DATA_BYTE == 4) {
			MEM_W(cpu.esp, cpu.eip + DATA_BYTE + 1);
			cpu.eip += op_src->val;
		}
	}
	print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
