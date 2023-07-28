#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "mymalloc.h"

void testCaseOne() {
    //making sure that non adjacent chunks cannot be combined
    char* p = malloc(1015);
    char* q = malloc(1015);
    char* r = malloc(1015);
    char* s = malloc(1015);

    free(p);
    free(r);

    char* t = malloc(2030);

    //checking if 4096 chunk can be allocated
    char* w = malloc(4096);
}

void testCaseTwo() {
    //trying to free ahead of pointer
    char* p = malloc(1015);
    free(p+1);

    free(p);

    //trying to free behind pointer
    char* q = malloc(2000);
    free(q-2);
}


int main(int argc, char **argv) {
    printf("Test Case 1's Output (which should print all error message)\n");
    printf("----------\n");
    testCaseOne();

    printf("\n");

    printf("Test Case 2's Output (which should print all error message)\n");
    printf("----------\n");
    testCaseTwo();
}
