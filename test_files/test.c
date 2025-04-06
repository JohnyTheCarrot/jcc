#define POG(...) {__VA_ARGS__}
#define CHAMP (1)
#define HELLO(x) x

#define TEST POG CHAMP // does not invoke POG macro
#define TEST2(seq) POG HELLO(seq) // invokes POG macro

//TEST
TEST2((1))
