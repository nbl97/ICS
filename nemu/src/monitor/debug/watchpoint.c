#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head = NULL, *free_ = NULL;

void init_wp_list() {
	int i;
	for (i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}
/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
	WP* p;
	if (free_ == NULL) {
		printf("Fail to creat a new watchpoint.\n");
		assert(0);
	} else {
		p = free_;
		free_ = free_ -> next;
	}
	return p;
}

void free_wp(WP *wp) {
	if(head == wp){
		head = head ->next;
	}else{
		WP *pre = head;
		while(pre->next!=wp && pre!=NULL){
			pre = pre->next;
		}
		if(pre->next == wp){
			pre->next = wp->next;
		}
	}	
	wp->next = free_;
	free_ = wp;
	return;
}
void set_watchpoint(char *args){
	if (head == NULL && free_ == NULL)	init_wp_list();
	WP *new = new_wp();
	bool success = true;
	new->newValue = new->oldValue = expr(args, &success);
	strcpy(new->expr, args);
	new->next = head;
	head = new;
	return;
}
void delete_watchpoint(int k){
	if (k < 0 || k >= NR_WP) {
		printf("Invalid input.\n");
		return;
	}
	WP* phead = head;
	while(phead!=NULL){
		if(phead->NO == k){
			break;
		}
		phead=phead->next;
	}
	if(phead==NULL){
		printf("Can't find %d watchpoint in the current watchpoints.\n",k);
		return;
	}
	WP* del = &wp_list[k];
	free_wp(del);
	return;
}
int check_watchpoints(){
	int flag=0;
	WP *p = head;
	while(p!=NULL){
		bool success = true;
		p->newValue = expr(p->expr,&success);
		if(p->newValue != p->oldValue){
			flag=1;
			printf("Hardware watchpoint %d : %s\n",p->NO,p->expr);
			printf("Old Value = %d\nNew Value = %d\n",p->oldValue,p->newValue);
			p->oldValue = p->newValue;
		}
		p = p->next;
	}
	return flag;
}
void list_watchpoint(){
	if(head==NULL)	printf("No watchpoints.\n");
	else{
		printf("NUM    EXPR    VALUE\n");
		WP *p=head;
		while(p!=NULL){
			printf("%-4d%6s    %-d\n",p->NO,p->expr,p->newValue);
			p=p->next;
		}
	}
	return;
}