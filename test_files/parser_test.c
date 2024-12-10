#define DEBUG(x) printf(x)

const char *test = DEBUG("Hello, world!");

int *(*(*(**x[])(char *, float *(*)(char *)))[])(char **, char *(*)());

char *(*(*(**x[])())[])();

int main() {
    bool x = true;
    if (x)
        return 1;
    else
        return 0;
}