#include <stdlib.h>  // for qsort

int compareInts(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);   // returns -1, 0, or 1
}

int main(void) {
    int arr[] = {5, 2, 8, 1, 9};

    qsort(arr, 5, sizeof(int), compareInts);   // sorts arr in place, works for any type
                                                // if you write the right compare function

    return 0;
}