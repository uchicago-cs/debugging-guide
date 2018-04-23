#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int add_array(int *a, int length)
{
    assert(a != NULL);

    int sum = 0;
    for(int i=0; i<length; i++)
        sum += a[i];

    return sum;
}

int main(int argc, char* argv[])
{
    int *a1, *a2;

    a1 = malloc(4);
    a1[0] = 10;
    a1[1] = 20;
    a1[2] = 30;
    a1[3] = 40;

    printf("The sum of a1 is %i\n", add_array(a1, 4));

    a2 = NULL;

    printf("The sum of a2 is %i\n", add_array(a2, 4));

    return 0;
}
