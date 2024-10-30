#include <iostream>
#include <cstdio>

extern "C" {
    float func_(float *x);
}
int helping(int i);

int main() {
    float x = 1.5;
    float result = func_(&x);
    int help = helping(1);
    std::printf("Here is f = %f and i = %d \n", result, help);
    return 0;
}
