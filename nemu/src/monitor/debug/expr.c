#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
enum {
	NOTYPE = 256, EQ, UEQ, LS, RS, 
	AND, OR, GOE, LOE, HEX, DEC, REG,
	DEREF, NEG, VAR
	/* TODO: Add more token types */
};
static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"==", EQ},						// equal
	{"!=", UEQ},                    //不等
	{"-", '-'},                     //减
	{"\\*", '*'},                   //乘
	{"/", '/'},                     //除
	{"%", '%'},                     //取余
	{"\\(", '('},                   //左括号
	{"\\)", ')'},                   //右括号
	{"&&", AND},                    //逻辑与
	{"&", '&'},                     //与
	{"\\|\\|", OR},                 //逻辑或
	{"\\|", '|'},                   //或
	{"!", '!'},                     //逻辑非
	{"~", '~'},                     //非
	{"\\^", '^'},                   //异或
	{"<<", LS},                     //左移
	{">>", RS},                     //右移
	{">=", GOE},                    //大于等于
	{">", '>'},                     //大于
	{"<=", LOE},                    //小于等于
	{"<", '<'},                     //小于
	{"0[xX][0-9a-fA-F]+", HEX},    //十六进制数
	{"[0-9]+", DEC},                 //十进制数
	{"\\$[a-z]{2,4}", REG},          //寄存器
	{"[A-Za-z_][A-Za-z0-9_]+", VAR} //变量
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ''tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				assert(substr_len < 32);
				if (rules[i].token_type != NOTYPE) {
					tokens[nr_token].type = rules[i].token_type;
					strncpy(tokens[nr_token].str,substr_start,substr_len);
					tokens[nr_token].str[substr_len] = '\0';
					nr_token++;
				}
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

static bool check_parentheses(int p, int q) {
	//先检测语法上括号匹配是否正确
	int level = 0, i;
	for (i = p;i <= q;i++) {
		if (level < 0) {
			printf("Invalid expression.\n");
			assert(0);
		}
		if (tokens[i].type == '(')	level++;
		else if (tokens[i].type == ')')	level--;
	}
	if (level != 0) {
		printf("Invalid expression.\n");
		assert(0);
	}
	//再检测表达式是否被一对括号括住
	if(tokens[p].type == '(' && tokens[q].type == ')'){
		level = 0;
		for (i = p;i <= q;i++) {
			if (level == 0 && i != p)	return false;
			if (tokens[i].type == '(')	level++;
			else if (tokens[i].type == ')')	level--;
		}
		return true;
	}
	return false;
}

bool isOperator(int type) {
	if (type =='*' || type == '/'|| type == '%' || type == '+' 
		|| type == '-' || type == LS|| type == RS || type == '>'
		|| type == '<' || type == GOE || type == LOE || type == EQ
		|| type == UEQ || type == '&' || type == '^' || type == '|'
		|| type == AND || type == OR)
		return true;
	return false;
}
static int dominant_operator(int p,int q) {
	int weight[300], i, j,ans=1000,ret = p;
	memset(weight,0x3f,sizeof(weight));
	weight['!'] = weight['~'] = 10;
	weight['*'] = weight['/'] = weight['%'] = 9;
	weight['+'] = weight['-'] = 8;
	weight[LS] = weight[RS] = 7;
	weight['>'] = weight['<'] = weight[GOE] = weight[LOE] = 6;
	weight[EQ] = weight[UEQ] = 5;
	weight['&'] = 4;
	weight['^'] = 3;
	weight['|'] = 2;
	weight[AND] = 1;
	weight[OR] = 0;
	for (i = p;i <= q;i++) {
		if (tokens[i].type == '(') {
			for(j=i;;j++){
				if(tokens[j].type == ')'){
					break;
				}
			}
			i=j;
			if(i>=q)	break;
			continue;
		}
		if (isOperator(tokens[i].type)) {
			if(weight[tokens[i].type] <= ans){
				ans = weight[tokens[i].type];
				ret = i;
			}
		}
	}
	return ret;
}
static uint32_t eval(int p,int q) {
	int op;
	uint32_t val1, val2;
	if (p > q) {
		printf("Invalid Expression.\n");
		assert(0);
	}else if (p == q) {
		if (tokens[p].type == DEC) {
			return atoi(tokens[p].str);
		}else if (tokens[p].type == HEX) {
			uint32_t a;
			sscanf(tokens[p].str,"%x",&a);
			return a;
		}else if (tokens[p].type == REG) {
			int i;
			for(i = 0;i < 8;i++) {
				if(strcmp(tokens[p].str + 1,regsl[i]) == 0)	return reg_l(i);
				if(strcmp(tokens[p].str + 1,regsw[i]) == 0)	return reg_w(i);
				if(strcmp(tokens[p].str + 1,regsb[i]) == 0)	return reg_b(i);
			}
			if(strcmp(tokens[p].str + 1, "eip") == 0)	return cpu.eip;
		}else if(tokens[p].type == VAR){
			int ret;
			ret = get_var(tokens[p].str);
			if(ret==-1){
				printf("not found the var:%s\n",tokens[p].str);
				assert(0);
				return 0;
			}
			return ret;
		}
	}else if (check_parentheses(p,q) == true) {
		return eval(p+1,q-1);
	}else {
		op = dominant_operator(p,q);
		if (tokens[op].type == NEG)	return -eval(op+1,q);
		if (tokens[op].type == DEREF)	return swaddr_read(eval(op + 1,q),1);
		if (tokens[op].type == '~')	return ~eval(op + 1,q);
		if (tokens[op].type == '!')	return !eval(op + 1,q);
		val1 = eval(p, op - 1);
		val2 = eval(op + 1,q);
		switch(tokens[op].type) {
			case '+' : return val1 + val2;
			case '-' : return val1 - val2;
			case '*' : return val1 * val2;
			case '/' : return val1 / val2;
			case '%' : return val1 % val2;
			case EQ  : return val1 == val2;
			case UEQ : return val1 != val2;
			case '&' : return val1 & val2;
			case '|' : return val1 | val2;
			case '^' : return val1 ^ val2;
			case LS  : return val1 << val2;
			case RS  : return val1 >> val2;
			case AND : return val1 && val2;
			case OR  : return val1 || val2;
			case '>' : return val1 > val2;
			case GOE : return val1 >= val2;
			case '<' : return val1 < val2;
			case LOE : return val1 <= val2;
			default  : assert(0);
		}
	}
	return 0;
}
uint32_t expr(char *e, bool *success) {
	int i;
	if (!make_token(e)) {
		*success = false;
		return 0;
	}
	for (i = 0;i < nr_token;i++) {
		if (tokens[i].type == '-' && (i==0 || isOperator(tokens[i - 1].type) || 
			tokens[i - 1].type == '(' || tokens[i - 1].type == NEG)) {
			tokens[i].type = NEG;
		}
		if (tokens[i].type == '*' && (i==0 || isOperator(tokens[i - 1].type) || 
			tokens[i - 1].type == '(' || tokens[i - 1].type == NEG)) {
			tokens[i].type = DEREF;
		}
		
	}
	/* TODO: Insert codes to evaluate the expression. */
	return eval(0,nr_token-1);
}
