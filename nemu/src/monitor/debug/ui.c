#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "cpu/reg.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args) {
	char *pSteps = strtok(NULL, " ");
	if (pSteps == NULL)	cpu_exec(1);
	else {
		int nSteps = atoi(pSteps);
		if (nSteps == 0)	return 0;
		else if (nSteps == -1 || nSteps > 0)	cpu_exec(nSteps);
		else	printf("invalid input!\n");
	}
	return 0;
}

static int cmd_info(char *args) {
	char *pOpt = strtok(NULL, " ");
	if (pOpt == NULL || ( strcmp(pOpt, "r") && strcmp(pOpt,"w"))) {
		printf("invalid input!\n");
	}
	else if(!strcmp(pOpt, "r")){
		int i;
		for (i = 0; i < 8; i++)
			printf("%s        0x%08x        %d\n", regsl[i], reg_l(i), reg_l(i));
		printf("--------------------------------------------\n");
		for (i = 0; i < 8; i++)
			printf("%s        0x%08x        %d\n", regsw[i], reg_w(i), reg_w(i));
		printf("--------------------------------------------\n");
		for (i = 0; i < 8; i++)
			printf("%s        0x%08x        %d\n", regsb[i], reg_b(i), reg_b(i));
		printf("--------------------------------------------\n");
		printf("eip        0x%08x        %d\n", cpu.eip, cpu.eip);
	}else{
		list_watchpoint();
	}
	return 0;
}

static int cmd_x(char *agrs) {
	char *pn, *paddr;
	pn = strtok(NULL, " ");
	paddr = strtok(NULL, " ");
	if (pn != NULL && paddr != NULL) {
		int n = atoi(pn), addr = sscanf(paddr, "0x%x", &addr);
		printf("start addr:0x%8x,len = %d\n", addr, n);
		int i;
		for (i = 0; i < n; i++) {
			if (!(i % 5))	printf("\n0x%08x:", addr + i);
			printf("  %02x", swaddr_read(addr + i, 1));
		}
		puts("");
	}
	return 0;
}

static int cmd_p(char *args) {
	int ans;
	bool success = true;
	ans = expr(args, &success);
	if (success == false) {
		printf("Invalid Expression.\n");
	} else {
		printf("%d\n", ans);
	}
	return 0;
}

static int cmd_w(char* args) {
	if (args == NULL) {
		printf("Invalid input.\n");
		return 0;
	}
	set_watchpoint(args);
	return 0;
}

static int cmd_d(char* args) {
	if (args == NULL) {
		printf("Invalid input.\n");
		return 0;
	}
	if(!strcmp(args,"a") || !strcmp(args,"all")){
		init_wp_list();
		printf("Delete all watchpoints successfully.\n");
		return 0;
	}
	delete_watchpoint(atoi(args));
	return 0;
}
static int cmd_bt(char *args){
	typedef struct {
		swaddr_t prev_ebp;
		swaddr_t ret_addr;
		uint32_t args[4];
	} PartOfStackFrame;
	PartOfStackFrame posf;
	int j;
	if(cpu.eip == 0x100000){
		printf("No Stack!\n");
		return 0;
	}
	posf.ret_addr = cpu.ebp;
	posf.prev_ebp = swaddr_read(posf.ret_addr,4);
	for(j=0;j<4;j++){
		posf.args[j] = swaddr_read(posf.ret_addr + 8 + j * 4,4);
	}
	int i = nr_symtab_entry-1;
	while(i>=0){
		if(symtab[i].st_info == 0x12){
			if(posf.ret_addr != 0){
				printf("0x%x  ",posf.ret_addr);
				printf("%s ",strtab+symtab[i].st_name);
				printf("(");
				for(j=0;j<4;j++){
					printf(" 0x%x ",posf.args[j]);
				}
				printf(")\n");
				posf.ret_addr = posf.prev_ebp;
				posf.prev_ebp = swaddr_read(posf.ret_addr,4);
			}
		}
		i--;
	}
	return 0;
}
static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Single Step", cmd_si },
	{ "info", "Print Program Status", cmd_info },
	{ "x", "Scan Memory", cmd_x },
	{ "p", "Calc the Expression", cmd_p },
	{ "w", "Set a watchpoint", cmd_w },
	{ "d", "Delete a watchpoint", cmd_d },
	{ "bt", "Print Stack Frame Stain", cmd_bt}
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if (arg == NULL) {
		/* no argument given */
		for (i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for (i = 0; i < NR_CMD; i ++) {
			if (strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while (1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if (cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if (args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for (i = 0; i < NR_CMD; i ++) {
			if (strcmp(cmd, cmd_table[i].name) == 0) {
				if (cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
