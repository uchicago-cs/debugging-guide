#include <stdio.h>
#include <math.h>

double distance(double x1, double y1, double x2, double y2)
{
    return sqrt( pow(x1 + x2, 2) + pow(y1 + y2, 2) );
}

int main(int argc, char* argv[])
{
    if(argc != 5)
    {
        printf("USAGE: %s X1 Y1 X2 Y2", argv[0]);
        return 1;
    }

    double x1, y1, x2, y2, d;

    sscanf(argv[1], "%lf", &x1);
    sscanf(argv[2], "%lf", &y1);
    sscanf(argv[3], "%lf", &x2);
    sscanf(argv[4], "%lf", &y2);

    d = distance(x1, y1, x2, y2);

    printf("The distance from (%.2lf, %.2lf) to (%.2lf, %.2lf) is %.2lf\n", x1, y1, x2, y2, d);

    return 0;
}
