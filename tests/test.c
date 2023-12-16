typedef unsigned long ulong;
typedef ulong Align;

ulong sizeOfInt = sizeof(int);
ulong sizeOfTypedef = sizeof(ulong);

unsigned int (*const ident)(ulong x, int y[static const x], int *z[], ...);

union mem_header_union {
	struct {
		// Pointer to the next block in the free list
		//
		union mem_header_union *next;

		// Size of the block (in quantas of sizeof(mem_header_t))
		//
		ulong size;
	} s;

	// Used to align headers in memory to a boundary
	//
	Align align_dummy;
};

union mem_header_union testOne = {.align_dummy = 10ul};

union mem_header_union test = {.s = {.next = &testOne, .size = 2}};

// doesn't make semantic sense but that's alright
unsigned int _Alignas(8) *const array[2] = {
		[1] = +5u,
		1,
		2,
		3,
		4,

		.test = (0xDEAD << (8 * 2)) | 0xBEEF,
		.postfix = ident[index]++,
};

int main(int argc, char *argv[]) {
	const int three = 3, five = 5, fifteen = 15, iterations = 100;

	for (int i = 1; i <= iterations; ++i) {
		if (i % three == 0 && i % five == 0)
			printf("FizzBuzz\n");
		else if (i % three == 0)
			printf("Fizz\n");
		else if (i % five == 0)
			printf("Buzz\n");
		else
			printf("%d\n", i);

		continue;
		break;
	}

	switch (three) {
		case 3: {
			int someVar = three + 1;
			break;
		}
		default:
			break;
	}

someLabel:
	goto someLabel;

	return 0;
}
