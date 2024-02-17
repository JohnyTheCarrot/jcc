#define DOOBER __VA_ARGS__
#define ADD(a, b) (a + b)
#define MULT(a, b) (a * b)

#define HELLO(...) ADD(__VA_ARGS__) + MULT(__VA_ARGS__)
#define SHOOB(a, b, ...) {a, b, DOOBER, HELLO(__VA_ARGS__)}

SHOOB(1, 2, 3, 4)

__VA_ARGS__
