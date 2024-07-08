#include <cstdlib>
#include "genFloat.h"

float randomFloat();
int   randomInt(int, int);

float genFloat(int min, int max)
{
    if (min > max)
        return genFloat(max, min);
    if (min == max)
        return (float)max;
    return (float)randomInt(min, max) + randomFloat();
}

float randomFloat() { return (float)(rand()) / (float)(RAND_MAX); }

int   randomInt(int a, int b)
{
    if (a > b)
        return randomInt(b, a);
    if (a == b)
        return a;
    return a + (rand() % (b - a));
}
