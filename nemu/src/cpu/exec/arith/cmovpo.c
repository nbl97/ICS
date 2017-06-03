#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "cmovpo-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "cmovpo-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmovpo-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(cmovpo_r2rm)
