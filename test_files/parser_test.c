
int *(*(*(**x[])(char *, float *(*)(char *)))[])(char **, char *(*)());

char *(*(*(**x[])())[])();

int main() {
    bool x = true;
    if (x)
        return 1;
}