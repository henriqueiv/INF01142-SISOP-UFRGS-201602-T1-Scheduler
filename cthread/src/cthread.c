//
//  cthread.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include <stdio.h>
#include "cthread.h"

int ccreate (void *(*start)(void *), void *arg) {
    return 1;
}

int cyield() {
    return CYIELD_SUCCESS;
}

int cjoin(int tid) {
    return 1;
}

int csem_init (csem_t *sem, int count) {
    return 1;
}

int cwait (csem_t *sem) {
    return 1;
}

int csignal (csem_t *sem) {
    return 1;
}

int cidentify (char *name, int size) {
    return 1;
}
