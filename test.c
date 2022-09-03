#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int arr[10] = { 0 };
    for (int i = 0; i < 10; i++) {
        arr[i] = 10 - i;
        printf("%d ", arr[i]);
    }
    putchar('\n');

    int* heap_arr = (int*)calloc(10, sizeof(int));
    for (int i = 0; i < 10; i++) {
        heap_arr[i] = 20 - i;
        printf("%d ", heap_arr[i]);
    }
    putchar('\n');

    free(heap_arr);

    return 0;
}