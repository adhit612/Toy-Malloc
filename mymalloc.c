#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "mymalloc.h"

/*freeStatus: 0 is free 1 is in use*/
/*header size = 2 (first index is chunk size, second index is free status)*/

#define MEMSIZE 4096
static char memory[MEMSIZE];

void init_mem(){
    int chunkSize = sizeof(memory);
    memory[0] = chunkSize;
    memory[1] = chunkSize >> 8;
    memory[2] = chunkSize >> 16;
    memory[3] = chunkSize >> 24;

    int freeStatus = 0;
    memcpy(&memory[4],&freeStatus,sizeof(int));
}

void printMemoryArray(){
    for(int i = 0; i < MEMSIZE; i ++){
        printf("%d \n", memory[i]);
    } 
}

void *mymalloc(size_t size, char *file, int line){
    if(size > MEMSIZE) {
        printf("Too large to allocate!\n");
        exit(EXIT_FAILURE);
    }
    //add check for user allocating 0 bytes
    if(*((int *)(memory)) == 0){
        init_mem();
    }
   int chunkSize = size + 9;
   int i = 0;
    while(i < MEMSIZE) {
        if(i >= MEMSIZE) {
            printf("You cannot allocate this!");
            exit(EXIT_FAILURE);
        }
        int currChunkSize = *((int *)(memory+i));
        if(currChunkSize > chunkSize && memory[i+4] == 0){ //if chunkSize > neededSize and the chunk is free
            memory[i] = chunkSize;
            memory[i+1] = chunkSize >> 8;
            memory[i+2] = chunkSize >> 16;
            memory[i+3] = chunkSize >> 24;
            int freeStatus = 1;
            memcpy(&memory[i+4],&freeStatus,sizeof(int)); //set free status to allocated

            //setting size at chunk end
            int finalIndex = i + chunkSize - 4;
            memory[finalIndex] = chunkSize;
            memory[finalIndex + 1] = chunkSize >> 8;
            memory[finalIndex + 2] = chunkSize >> 16;
            memory[finalIndex + 3] = chunkSize >> 24;

            //setting new free chunk size to beginning
            int newFreeChunkSize = currChunkSize - chunkSize;
            int startOfNextChunkIndex = i + chunkSize;
            memory[startOfNextChunkIndex] = newFreeChunkSize;
            memory[startOfNextChunkIndex+1] = newFreeChunkSize >> 8;
            memory[startOfNextChunkIndex+2] = newFreeChunkSize >> 16;
            memory[startOfNextChunkIndex+3] = newFreeChunkSize >> 24;

            //setting new free chunk status to free
            int newFreeStatus = 0;
            memcpy(&memory[i+chunkSize+4],&newFreeStatus,sizeof(int));
            int endOfFreeChunkIndex = i+chunkSize+newFreeChunkSize-4;
            memory[endOfFreeChunkIndex] = newFreeChunkSize;
            memory[endOfFreeChunkIndex+1] = newFreeChunkSize >> 8;
            memory[endOfFreeChunkIndex+2] = newFreeChunkSize >> 16;
            memory[endOfFreeChunkIndex+3] = newFreeChunkSize >> 24;

            void* resultPointer = &memory[i+5];
            return resultPointer;
        }
        else if(currChunkSize == chunkSize && memory[i+4] == 0){ //if chunkSize = neededSize and the chunk is free
            int freeStatus = 1;
            memcpy(&memory[i+4], &freeStatus, sizeof(int));

            void *resultPointer = &memory[i+5];
            return resultPointer;
        }
        else{
            i += currChunkSize;
        }
    }

    printf("You cannot allocate, not accessible!\n");

}

void coalesceChecker(void *ptr){
    char sizeOfLeftChunk = *((int *)(ptr-9)); //get the size of the left chunk
    char freeStatusOfLeftChunk = *((int *)(ptr-5-(sizeOfLeftChunk-4))); //get the free status of the left chunk

    char sizeOfCurrChunk = *((int *)(ptr-5));
    char sizeOfRightChunk = *(char*)(ptr+(sizeOfCurrChunk-5)); //get the size of the right chunk
    char freeStatusOfRightChunk = *(char*)(ptr+(sizeOfCurrChunk-1));

    if(freeStatusOfLeftChunk == 1 && freeStatusOfRightChunk == 1 || sizeOfRightChunk == 0 && freeStatusOfLeftChunk == 1 || sizeOfLeftChunk == 0 && freeStatusOfRightChunk == 1){ //no neighbors, free just this chunk
        int freeVal = 0;
        memcpy(&(*(char*)(ptr-1)),&freeVal,sizeof(int)); //making chunk free
    }
    else if(freeStatusOfLeftChunk == 0 && freeStatusOfRightChunk == 0 && sizeOfRightChunk != 0 && sizeOfLeftChunk != 0){
        int newChunkSize = sizeOfCurrChunk + sizeOfLeftChunk + sizeOfRightChunk;
        int zeroSizeAndFreeVal = 0;

        *((int *)(ptr-5)) = 0; //set current chunk size to 0
        memcpy(&(*(char*)(ptr-1)),&zeroSizeAndFreeVal,sizeof(char)); //set current chunk free status to free
        *((int *)(ptr-9)) = 0; //set left chunk end header to 0
        *((int *)(ptr-9-(sizeOfLeftChunk-4))) = newChunkSize; //set size of left chunk beginning header to new size
        memcpy(&(*(char*)(ptr-5-(sizeOfLeftChunk-4))),&zeroSizeAndFreeVal,sizeof(char)); //set free status of left chunk to free
        *((int *)(ptr+(sizeOfCurrChunk-9))) = 0; //set current chunk end header to 0
        *((int *)(ptr+(sizeOfCurrChunk-6+1))) = 0; //set right header beginning size chunk to 0
        memcpy(&(*(char*)(ptr+(sizeOfCurrChunk-6+5))),&zeroSizeAndFreeVal,sizeof(char)); //set right chunk free status to free
        *((int *)(ptr+(sizeOfCurrChunk-6+(sizeOfRightChunk-3)))) = newChunkSize; //set right chunk end size header to new size
    }   
    else if(freeStatusOfLeftChunk == 0){ //left is free, right is in use / DNE
        int newChunkSize = sizeOfCurrChunk + sizeOfLeftChunk;
        int zeroSizeAndFreeVal = 0;

        *((int *)(ptr-5)) = 0; //set current chunk size to 0
        memcpy(&(*(char*)(ptr-1)),&zeroSizeAndFreeVal,sizeof(char)); //set current chunk free status to free
        *((int *)(ptr-9)) = 0; //set left chunk end header to 0
        *((int *)(ptr-9-(sizeOfLeftChunk-4))) = newChunkSize; //set size of left chunk beginning header to new size
        *((int *)(ptr+(sizeOfCurrChunk-9))) = newChunkSize; //set current chunk end header to newChunkSize
    }
    else if(freeStatusOfRightChunk == 0 && sizeOfRightChunk != 0){ //left is in use / DNE, right is free
        int newChunkSize = sizeOfCurrChunk + sizeOfRightChunk;
        int zeroSizeAndFreeVal = 0;

        *((int *)(ptr-5)) = newChunkSize; //set current chunk size to newChunkSize
        memcpy(&(*(char*)(ptr-1)),&zeroSizeAndFreeVal,sizeof(char)); //set current chunk free status to free
        *((int *)(ptr+(sizeOfCurrChunk-9))) = 0; //set current chunk end header to 0
        *((int *)(ptr+(sizeOfCurrChunk-6+1))) = 0; //set right header beginning size chunk to 0
        memcpy(&(*(char*)(ptr+(sizeOfCurrChunk-6+5))),&zeroSizeAndFreeVal,sizeof(char)); //set right chunk free status to free
        *((int *)(ptr+(sizeOfCurrChunk-6+(sizeOfRightChunk-3)))) = newChunkSize; //set right chunk end size header to new size
    }
}

void myfree(void *ptr, char *file, int line){
    char freeStatusOfChunk = *((char*)(ptr-1));

    if(freeStatusOfChunk == 1){//in use, can be freed
        coalesceChecker(ptr);
    }
    else {
        printf("You cannot free at this allocated chunk; line #: %d; file: %s\n", line, file);
        exit(EXIT_FAILURE);
    }
}

int main() {

}










