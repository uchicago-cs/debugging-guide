#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char s;

    s = malloc(100);

    strcpy(s, "Hello, world!");

    printf("The string is %s", s);

    free(s);
}