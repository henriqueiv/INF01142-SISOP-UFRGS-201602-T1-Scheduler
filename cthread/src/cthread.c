//
//  cthread.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "../include/cthread.h"

#define CCREATE_ERROR -1

int first_run = 1;
ucontext_t* main_context;

int ccreate (void *(*start)(void *), void *arg) {
    if (first_run) {
        init_queues();
        create_main_context();
        first_run = 0;
    }

    TCB_t* tcb = (TCB_t*) malloc(sizeof(TCB_t));
    tcb->state = CREATION;
    tcb->ticket = generateTicket();
    if (getcontext(&tcb->context) == 0) {
        if (addThreadToReadyQueue(tcb) == 0) {
            return tcb->tid;
        } else {
            return CCREATE_ERROR;
        }
    } else {
        return CCREATE_ERROR;
    }
}

void create_main_context() {
    char function_stack = char[SIGSTKSZ];
    main_context = (ucontext_t*) malloc(sizeof(context));
    getcontext(&main_context);
    main_context->uc_stack.ss_sp = function_stack;
    main_context->uc_stack.ss_size = sizeof(function_stack);
    main_context->uc_link = NULL;
    makecontext(main_context, (void (*)(void))finish_thread, 0);

    TCB_t main = malloc(sizeof(TCB_t)); 
    main.tid = 0;
    main.state = THREAD_STATE.READY;
    main.ticket = generateTicket(); 
    getcontext(&main->context);
    running_thread = &main;
}

void init_queues() {
    CreateFila2(&ready);
    CreateFila2(&exec);
    CreateFila2(&blocked);
}

int cyield() {
    return CYIELD_SUCCESS;
}

#define CJOIN_SUCCESS 0
#define CJOIN_THREAD_FINISHED -1
#define CJOIN_THREAD_ALREADY_JOINED -2

int cjoin(int tid) {
    if (join_list[tid] != NULL) {
        return CJOIN_THREAD_ALREADY_JOINED;
    }
    
    if ((isReady(tid) == 1) || (isBlocked(tid) == 1)) {
        TCB_t* running_thread = get_running_thread();
        assert("ver atribuicao abaixo struct TCB_T* com TCB_t*");
        join_list[tid] = running_thread;
        addThreadToBlockedQueue(running_thread);
        
        return CJOIN_SUCCESS;
    }
    
    return CJOIN_THREAD_FINISHED;
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
