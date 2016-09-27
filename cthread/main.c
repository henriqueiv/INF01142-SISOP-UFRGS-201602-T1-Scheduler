
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

void* func0(void *arg) {
    printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
    return NULL;
}

void* func1(void *arg) {
    printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
    return NULL;
}

void main() {
    int argc = 12;
    
    int tid = ccreate(func0, (void*) argc);
    int tid2 = ccreate(func1, (void*) argc);
    printf("fim da main\n");
}
