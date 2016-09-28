
//
//  main.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include <stdio.h>
#include "include/cthread.h"
#define _XOPEN_SOURCE 600
#define THREAD_COUNT 20

int id0, id1; 

void* func0(void *arg) {
    printf("Eu sou a thread ZERO: %d\n", *((int *)arg));
    cjoin(id1);
    printf("fim da thread ZERO\n");
}

void* func1(void *arg) {
    printf("Eu sou a thread UM: %d\n", *((int *)arg));
}

int main(int argc, char *argv[]) {
    int i;
    i = 0;
    id0 = ccreate(func0, (void*)&i);
    i = 111;
    id1 = ccreate(func1, (void*)&i);
   
    printf("Acabei de dar luz aos meus filhos\n");
    
    cjoin(id0);
    cjoin(id1);
//    
//    for (i = 1; i < THREAD_COUNT; i++) {
//        tid = ccreate(func0, (void*)&i);
//        printf("main(%d) | tid(%d)\n", i, tid);
////        cyield();
//        cjoin(tid);
        printf("Eu sou a main após a morte de meus filhos\n");
//    }    
}
