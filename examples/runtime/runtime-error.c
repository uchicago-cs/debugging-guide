#include <stdio.h>

float foo(int a, int b)
{
    return a / b;
}

float bar(int a, int b)
{
    return foo(a, b);
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        printf("USAGE: runtime-error.py NUM1 NUM2");
        return 1;
    }

    int a,b,x;

    sscanf(argv[1], "%i", &a);
    sscanf(argv[2], "%i", &b);

    x = bar(a, b);

    printf("a / b = %i\n", x);

    return 0;
}