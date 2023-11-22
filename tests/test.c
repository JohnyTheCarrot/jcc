unsigned int (*const ident)(int x, int y[static const x], int *z[], ...);

// doesn't make semantic sense but that's alright
unsigned int _Alignas(8) *const array[2] = {
		[1] = +5u,
		1,
		2,
		3,
		4,

		.test	 = (0xDEAD << (8 * 2)) | 0xBEEF,
		.postfix = ident[index]++,
};

int main(int argc, char *argv[]) {
	const int three = 3, five = 5, fifteen = 15, iterations = 100;


	for (int i = 1; i <= iterations; ++i) {
		if (i % three == 0 && i % five == 0) printf("FizzBuzz\n");
		else if (i % three == 0)
			printf("Fizz\n");
		else if (i % five == 0)
			printf("Buzz\n");
		else
			printf("%d\n", i);

		continue;
		break;
	}

	goto someLabel;

	return 0;
}
