//
//  test_ccreate.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "../include/cthread.h"

void* func2(void *arg) {
    printf("Eu sou a thread ID2 imprimindo %d\n", *((int *)arg));
    return 0;
}

void* func1(void *arg) {
    printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
    ccreate(func2, (void*)&arg);
    return 0;
}

int main(int argc, char *argv[]) {
    argc = 10;
    cjoin(ccreate(func1, (void*)&argc));
    printf("Main chegou ao fim\n");
    return 0;
}
