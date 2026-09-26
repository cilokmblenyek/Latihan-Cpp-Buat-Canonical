#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

int main() {

    double aneh = 3 / 2 ;

    int a = 3;
    int b = 2;
    double c = a / b; // integer division, result is 1.0
    double d = static_cast<double>(a) / b; // correct way to get a double result, result is 1.5
    cout << "a: " << a << ", b: " << b << endl;
    cout << "c: " << c << ", d: " << d << endl;
    cout << "aneh: " << aneh << endl;
}