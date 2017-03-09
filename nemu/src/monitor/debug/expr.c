#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, UEQ, LS, RS, 
	AND, OR, GOE, LOE, HEX, DEC, REG
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
	{"&", '&'},                     //与
	{"\\|", '|'},                   //或
	{"~", '~'},                     //非
	{"\\^", '^'},                   //异或
	{"<<", LS},                     //左移
	{">>", RS},                     //右移
	{"&&", AND},                    //逻辑与
	{"\\|\\|", OR},                 //逻辑或
	{"!", '!'},                     //逻辑非
	{">", '>'},                     //大于
	{">=", GOE},                    //大于等于
	{"<", '<'},                     //小于
	{"<=", LOE},                    //小于等于
	{"0[xX][0-9a-fA-F]+", HEX},    //十六进制数
	{"[0-9]+", DEC},                 //十进制数
	{"\\$[a-z]{2,4}", REG}          //寄存器
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
		for (int i = p;i <= q;i++) {
			if (level == 0 && i != p)	return false;
			if (tokens[i].type == '(')	level++;
			else if (tokens[i].type == ')')	level--;
		}
		return true;
	}
	return false;
}

bool isOperator(int type) {
	if (type == '!' || type == '~' || type =='*' || type == '/'
		|| type == '%' || type == '+' || type == '-' || type == LS
		|| type == RS || type == '>' || type == '<' || type == GOE
		|| type == LOE || type == EQ || type == UEQ || type == '&'
		|| type == '^' || type == '|' || type == AND || type == OR)
		return true;
	return false;
}
static int dominant_operator(int p,int q) {
	int weight[300], i, j,ans=1000,ret;
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
			i=j+1;
			if(i>q)	break;
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

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return 0;
}
