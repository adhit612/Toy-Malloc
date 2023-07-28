#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "mymalloc.h"

double testCaseA() {

    __clock_t start, end;
    double exec_time = 0.0;
    start = clock();

    for(int i = 0; i < 120; i++) {
        char* p = malloc(1);
        free(p);
    }

    end = clock();
    exec_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    return exec_time;

}

double testCaseB() {

    __clock_t start, end;
    double exec_time = 0.0;
    start = clock();
    
    char* arr[120];
    for(int i = 0; i < 120; i++){
	    arr[i] = (char*) malloc(1);   
    }
    for(int i = 0; i < 120; i++){   
        free(arr[i]);
    }

    end = clock();
    exec_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    return exec_time;

}

double testCaseC() {

    __clock_t start, end;
    double exec_time = 0.0;
    start = clock();
    
    srand(time(NULL));
    char* arr[120];
    int num_chunks = 0;
    for (int i = 0; i < 120; i++) {
        if (num_chunks == 0 || rand() % 2 == 0) {
            char* ptr = malloc(1);
            if (ptr == NULL) {
                printf("Error: malloc() failed\n");
                exit(1);
            }
            arr[num_chunks++] = ptr;
        } else {
            int index = rand() % num_chunks;
            free(arr[index]);
            arr[index] = arr[--num_chunks];
        }
    }
    for (int i = 0; i < num_chunks; i++) {
        free(arr[i]);
    }
    
    end = clock();
    exec_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    return exec_time;
}

double testCaseD() { 
    __clock_t start, end;
    double exec_time = 0.0;
    start = clock();
    
    char* arr[120];
    for(int i = 0; i < 120; i+=2){
	    arr[i] = (char*) malloc(1);   
    }

    for(int i = 0; i < 120; i += 2){
        free(arr[i]);
    }

    end = clock();
    exec_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    return exec_time;
}

double testCaseE() { 
    __clock_t start, end;
    double exec_time = 0.0;
    start = clock();

    for(int i = 0; i < 4087; i++) {
        char *p = malloc(1);
        int *q = (int*)malloc(1); 
        free(p);
        free(q);
    }

    end = clock();
    exec_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    return exec_time;

}

int main(int argc, char **argv) {
    double timeA = 0.0;
    double timeB = 0.0;
    double timeC = 0.0;
    double timeD = 0.0;
    double timeE = 0.0;

    for(int i = 0; i < 50; i++) {
        timeA += testCaseA();
        timeB += testCaseB();
        timeC += testCaseC();
        timeD += testCaseD();
        timeE += testCaseE();
    }
    printf("Average time for TestCase A: %f\n", timeA/50);
    printf("Average time for TestCase B: %f\n", timeB/50);
    printf("Average time for TestCase C: %f\n", timeC/50);
    printf("Average time for TestCase D: %f\n", timeD/50);
    printf("Average time for TestCase E: %f\n", timeE/50);
}
