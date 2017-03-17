#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	uint32_t oldValue,newValue;
	char expr[100];
	/* TODO: Add more members if necessary */
} WP;

void init_wp_list();
WP* new_wp();
void free_wp(WP *wp);
int check_watchpoints();
void set_watchpoint(char *args);
void delete_watchpoint(int k);
void list_watchpoint();

#endif
