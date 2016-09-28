
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

int id3;

void* func3(void *arg) {
    printf("inicio thread TRES\n");

    printf("fim da thread TRES\n");
}

void* func1(void *arg) {
    printf("inicio thread UM\n");
    id3 = ccreate(func3, NULL);
    cjoin(id3);
    printf("fim da thread UM\n");
    int iiii = 0;
}

void* func2(void *arg) {
    printf("inicio thread DOIS\n");
    cjoin(id3);
    printf("fim da thread DOIS\n");
}

int main(int argc, char *argv[]) {
    int i, id1, id2;
    i = 0;
    id1 = ccreate(func1, (void*)&i);
    print_all_queues();
    id2 = ccreate(func2, (void*)&i);
    printf("Main criou 1 e 2\n");
    cjoin(id1);
    cjoin(id2);
//    
//    for (i = 1; i < THREAD_COUNT; i++) {
//        tid = ccreate(func0, (void*)&i);
//        printf("main(%d) | tid(%d)\n", i, tid);
////        cyield();
//        cjoin(tid);
        printf("Eu sou a main após a morte de meus filhos\n");
//    }    
}
