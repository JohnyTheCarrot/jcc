// currently the include paths are always relative to the CWD of the compiler invocation, we'll fix this later
#include "test_files/macro_sequence_for.h"

#warning hi

#define SUM(seq) SF_FOR_EACH(SF_NULL, SUM_STEP, SF_STATE, 0, seq)
#define SUM_STEP(n, d, x) d+x
int x = SUM((1)(2)(3)); // 0+1+2+3
