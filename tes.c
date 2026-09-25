#include <stdio.h>

int main() {
    //input maximum value for array size
    const int MAX;        // value cannot change after initialization

    printf("MAX:");
    scanf("%d", &MAX);
    // Note: in C (unlike C++), a `const int` is NOT a true compile-time constant —
    // you generally cannot use it as an array size without VLA support:
    int arr[MAX];                 // legal in C99+ as a Variable Length Array, but
                                   // NOT portable/standard the way C++'s constexpr is

    // const with pointers — same "read right to left" rule as C++
    const int* p1 = &MAX;
    
    printf("p1 points to: %d\n", *p1);
    // pointer to const int: *p1 = 5 is ILLEGAL
    int value;
    printf("value:");
    scanf("%d", &value);

    int* const p2 = &value;       // const pointer to int: p2 = &other is ILLEGAL
    const int* const p3 = &value; // both illegal to change

    printf("p2 points to: %d\n", *p2);
    printf("p3 points to: %d\n", *p3);

    return 0;
}