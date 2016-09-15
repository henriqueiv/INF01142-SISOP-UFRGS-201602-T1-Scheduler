//
//  cthread.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "cthread.h"

int ccreate (void *(*start)(void *), void *arg) {
    pthread_t thread;
    pthread_attr_t attr_t;
    
    pthread_create(&thread, &attr_t, start, arg);
    
    TCB_t tcb;
    tcb.state = CREATION;
    tcb.tid = generateThreadId();
    tcb.ticket = generateTicket();
    getcontext(&tcb.context);
    
    addThreadToReadyQueue(&tcb);
    
    return tcb.tid;
}

int cyield() {
    return CYIELD_SUCCESS;
}

int cjoin(int tid) {
    return -1;
}

int csem_init (csem_t *sem, int count) {
    return -1;
}

int cwait (csem_t *sem) {
    return -1;
}

int csignal (csem_t *sem) {
    return -1;
}

int cidentify (char *name, int size) {
    const char* strNames = "Henrique Valcanaia - 240501\nPietro Degrazia - 243666\0";
    if (size >= strlen(strNames)) {
        strcpy(name, strNames);
        return 0;
    } else {
        return -1;
    }
}
