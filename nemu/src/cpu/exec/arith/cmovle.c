#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "cmovle-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "cmovle-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "cmovle-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(cmovle_r2rm)
