#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void print(void* ptr) {
    printf("%p\n", ptr);
    if ((uint64_t)ptr % 2) {
        puts("Odd");
    } else {
        puts("Even");
    }
}


int main(void)
{
    int arr[10] = { 0 };
    for (int i = 0; i < 10; i++) {
        arr[i] = 10 - i;
        printf("%d %p\n", arr[i], &arr[i]);
    }
    putchar('\n');

    int* heap_arr = (int*)calloc(10, sizeof(int));
    for (int i = 0; i < 10; i++) {
        heap_arr[i] = 20 - i;
        printf("%d %p\n", heap_arr[i], &heap_arr[i]);
    }
    putchar('\n');

    free(heap_arr);

    return 0;
}