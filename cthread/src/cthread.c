//
//  cthread.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "../include/cthread.h"

#define CCREATE_ERROR -1

TCB_t* running_thread;

PFILA2 ready;
PFILA2 exec;
PFILA2 blocked;

// -------------- AUX FUNC -------------

/*!
 @brief Partiremos do 1 pois a 0 será a main
 */
int thread_id = 1;

/*!
 @brief Partiremos do 1 pois a 0 será a main
 */
int get_thread_closest_to_ticket(int ticket, TCB_t *next_thread) {
    return 1;
}

/*!
 @brief Gera um numero aleatorio inteiro de 1 byte
 @discussion Utiliza a funcao Random2() da biblioteca support.o disponibilizada pelo professor
 @return Inteiro aleatorio de 1 byte
 */
int generate_ticket() {
    int ticket = Random2() % 255;
    return ticket;
}

/*!
 @brief Gera um novo identificador de thread
 @discussion Este identificador é sequencial e não é reutilizado. Use com sabedoria.
 */
int generate_thread_id() {
    thread_id++;
    return thread_id;
}

/*!
 @brief Adiciona um TCB a uma fila
 */
int add_thread_to_queue(TCB_t* thread, PFILA2 queue) {
    int result = AppendFila2(queue, (void*) thread);
    return result;
}

/*!
 @brief Adiciona um TCB a fila de aptos
 */
int add_thread_to_ready_queue(TCB_t* thread) {
    int result = add_thread_to_queue(thread, ready);
    return result;
}

/*!
 @brief Adiciona um TCB a fila de bloqueados
 */
int add_thread_to_blocked_queue(TCB_t* thread) {
    int result = add_thread_to_queue(thread, blocked);
    return result;
}

int find_thread_with_id(int tid, PFILA2 queue) {
    struct sFilaNode2 *node = (struct sFilaNode2*) malloc(sizeof(struct sFilaNode2));
    node = queue->first;
    while (node != NULL) {
        if (((TCB_t*) node)->tid == tid) {
            return 1;
        }
        node = node->next;
    }
    
    return 0;
}

int is_ready(int tid) {
    return find_thread_with_id(tid, ready);
}

int is_blocked(int tid) {
    return find_thread_with_id(tid, blocked);
}

TCB_t* get_running_thread() {
    return running_thread;
}

int get_next_thread(TCB_t* next_thread) {
    int randomTicket = generate_ticket();
    if (get_thread_closest_to_ticket(randomTicket, next_thread) == 0) {
        return  0;
    } else {
        return -1;
    }
}

int schedule() {
    TCB_t next_thread;
    if (get_next_thread(&next_thread) == 0) {
        return 0;
    } else {
        return -1;
    }
}

// ---------- CTHREAD ----------

int first_run = 1;
ucontext_t* main_context;

void create_main_context() {
    char function_stack[SIGSTKSZ];
    main_context = (ucontext_t*) malloc(sizeof(ucontext_t));
    getcontext(main_context);
    main_context->uc_stack.ss_sp = function_stack;
    main_context->uc_stack.ss_size = sizeof(function_stack);
    main_context->uc_link = NULL;
    makecontext(main_context, NULL, 0);
    
    TCB_t main;
    main.tid = 0;
    main.state = THREAD_STATE_READY;
    main.ticket = generate_ticket();
    getcontext(&(main.context));
    running_thread = &main;
}

void init_queues() {
    CreateFila2(ready);
    CreateFila2(exec);
    CreateFila2(blocked);
}
int ccreate (void *(*start)(void *), void *arg) {
    if (first_run) {
        init_queues();
        create_main_context();
        first_run = 0;
    }

    TCB_t* tcb = (TCB_t*) malloc(sizeof(TCB_t));
    tcb->state = THREAD_STATE_CREATION;
    tcb->ticket = generate_ticket();
    if (getcontext(&tcb->context) == 0) {
        if (add_thread_to_ready_queue(tcb) == 0) {
            return tcb->tid;
        } else {
            return CCREATE_ERROR;
        }
    } else {
        return CCREATE_ERROR;
    }
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
    
    if ((is_ready(tid) == 1) || (is_blocked(tid) == 1)) {
        TCB_t* running_thread = get_running_thread();
        join_list[tid] = running_thread;
        add_thread_to_blocked_queue(running_thread);
        
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
