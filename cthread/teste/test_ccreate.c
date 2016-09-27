//
//  test_ccreate.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "test_ccreate.h"

void func0(void *arg) {
    printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
}

void func1(void *arg) {
    printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}

void main() {
    int argc = 12;
    
    enum CCREATE_STATUS tid = ccreate(&func0, (void*) argc);
    if (tid == ERROR) {
        
    } else {
    
    }
}
