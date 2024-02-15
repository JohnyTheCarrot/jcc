#include "header-test.h"

#define OBJ_MACRO printf("hello, world!\n")

#define MULT(a, b) ((a) * (b))
#define ADD(a, b) ((a) + (b))
#define COMBINED(a, b, c) ADD(a, MULT(b, c))
#define BANANA(a, b, c, d) ADD(COMBINED(a, b, c), d)

#define TEST(a, b) UNKNOWN(a, b)

#define VAR_ARGS(a, ...) {a, __VA_ARGS__}

#define RECURSION(a, b) RECURSION(a, b)

int main() {
	OBJ_MACRO;
	int add = ADD(1, 2);
	int mult = MULT(2, 5);
	int combined = COMBINED(5, 4, 9);
	int banana = BANANA(7, 3, 6, 1);

#define UNKNOWN(a, b) (a + b)

	int test = TEST(4, 5);
	int recursion = RECURSION(2, 3);

    int arr[] = VAR_ARGS(1, 2, 3, 4);

    char* unterminated = "oh no \
this string is \
not terminated
}
