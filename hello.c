#include <stdio.h>

void C(int a, int b, int c)
{
    printf("%d %d %d\n", a, b, c);
}

void B(int a, int b)
{
    int c = 30;
    C(a, b, c);
}


void A(int a)
{
    int b = 20;
    B(a, b);
}

int main(int argc, char** argv)
{
    int num = 10;
    A(num);

    return 0;
}