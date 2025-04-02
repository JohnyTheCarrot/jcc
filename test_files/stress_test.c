#define POW2(x) ((x) * (x))
#define POW3(x) (POW2(x) * (x))
#define POW5(x) (POW3(x) * POW2(x))
#define POW7(x) (POW5(x) * POW2(x))
#define POW9(x) (POW7(x) * POW2(x))
#define POW11(x) (POW9(x) * POW2(x))
#define POW13(x) (POW11(x) * POW2(x))
#define POW15(x) (POW13(x) * POW2(x))
#define POW17(x) (POW15(x) * POW2(x))
#define POW19(x) (POW17(x) * POW2(x))
#define POW21(x) (POW19(x) * POW2(x))
#define POW23(x) (POW21(x) * POW2(x))
#define POW25(x) (POW23(x) * POW2(x))
#define POW27(x) (POW25(x) * POW2(x))
#define POW29(x) (POW27(x) * POW2(x))
#define POW31(x) (POW29(x) * POW2(x))
#define POW33(x) (POW31(x) * POW2(x))
#define POW35(x) (POW33(x) * POW2(x))
#define POW37(x) (POW35(x) * POW2(x))
#define POW39(x) (POW37(x) * POW2(x))
#define POW41(x) (POW39(x) * POW2(x))
#define POW43(x) (POW41(x) * POW2(x))
#define POW45(x) (POW43(x) * POW2(x))
#define POW47(x) (POW45(x) * POW2(x))
#define POW49(x) (POW47(x) * POW2(x))
#define POW51(x) (POW49(x) * POW2(x))
#define POW53(x) (POW51(x) * POW2(x))
#define POW55(x) (POW53(x) * POW2(x))
#define POW57(x) (POW55(x) * POW2(x))
#define POW59(x) (POW57(x) * POW2(x))
#define POW61(x) (POW59(x) * POW2(x))
#define POW63(x) (POW61(x) * POW2(x))
#define POW65(x) (POW63(x) * POW2(x))
#define EL(x) (((x) - 1.) / ((x) + 1.))
#define LN(x) (2. * (EL(x) + 1. / 3. * POW3(EL(x)) + 1. / 5. * POW5(EL(x)) + \
                   1. / 7. * POW7(EL(x)) + 1. / 9. * POW9(EL(x)) +           \
                   1. / 11. * POW11(EL(x)) + 1. / 13. * POW13(EL(x)) +       \
                   1. / 15. * POW15(EL(x)) + 1. / 17. * POW17(EL(x)) +       \
                   1. / 19. * POW19(EL(x)) + 1. / 21. * POW21(EL(x)) +       \
                   1. / 23. * POW23(EL(x)) + 1. / 25. * POW25(EL(x)) +       \
                   1. / 27. * POW27(EL(x)) + 1. / 29. * POW29(EL(x)) +       \
                   1. / 31. * POW31(EL(x)) + 1. / 33. * POW33(EL(x)) +       \
                   1. / 35. * POW35(EL(x)) + 1. / 37. * POW37(EL(x)) +       \
                   1. / 39. * POW39(EL(x)) + 1. / 41. * POW41(EL(x)) +       \
                   1. / 43. * POW43(EL(x)) + 1. / 45. * POW45(EL(x)) +       \
                   1. / 47. * POW47(EL(x)) + 1. / 49. * POW49(EL(x)) +       \
                   1. / 51. * POW51(EL(x)) + 1. / 53. * POW53(EL(x)) +       \
                   1. / 55. * POW55(EL(x)) + 1. / 57. * POW57(EL(x)) +       \
                   1. / 59. * POW59(EL(x)) + 1. / 61. * POW61(EL(x)) +       \
                   1. / 63. * POW63(EL(x)) + 1. / 65. * POW65(EL(x))         \
                ))
#define LOG2(x) (LN(x) / LN(2.))
#define FLOOR(x) (int) (x)
#define NUM_BITS(x) (FLOOR(LOG2(x)) + 1)

NUM_BITS(255)
