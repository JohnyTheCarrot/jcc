#ifndef JCC_HEADER_TEST_H
#define JCC_HEADER_TEST_H

#define MACRO_FN(a, b) (a + b)

#define SOME_MACRO {         \
		int hello = NUMBER;  \
		printf("%d", hello); \
}

char tooBig = 'lol';

void SomeFunction(void);

int CalculateThing(int a, int b);

#endif//JCC_HEADER_TEST_H
