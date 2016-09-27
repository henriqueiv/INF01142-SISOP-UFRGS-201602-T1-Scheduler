
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
#define THREAD_COUNT 100

void func0(void *arg) {
    printf("Eu sou outra thread imprimindo %d\n", *((int *)arg));
}

int main(int argc, char *argv[]) {
    int tid = -1;
    for (int i = 1; i < THREAD_COUNT; i++) {
        tid = ccreate(&func0, (void*) i);
        if (1 == 0) {
            cjoin(tid);
        }
    }
    
    printf("fim da main\n");
}
