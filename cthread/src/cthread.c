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

FILA2 ready;
FILA2 exec;
FILA2 blocked;

// -------------- AUX FUNC -------------

void* fimDaMain(void *arg) {
    printf("Eu sou o fim da main \n");
}

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
int add_thread_to_queue(TCB_t* thread, FILA2 queue) {
    int result = AppendFila2(&queue, (void*) thread);
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

int find_thread_with_id(int tid, FILA2 queue) {
    struct sFilaNode2 *node = (struct sFilaNode2*) malloc(sizeof(struct sFilaNode2));
    node = queue.first;
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

int intialized = 0;
ucontext_t scheduler;
TCB_t main_thread;
void create_main_context() {
    main_thread.tid = 0;
    main_thread.state = THREAD_STATE_READY;
    main_thread.ticket = generate_ticket();
    getcontext(&(main_thread.context));

    main_thread.context.uc_stack.ss_sp = malloc(SIGSTKSZ);
    main_thread.context.uc_stack.ss_size = SIGSTKSZ;
    main_thread.context.uc_link = NULL;
    makecontext(&main_thread.context, (void (*)(void))fimDaMain, 0);
    running_thread = &main_thread;
}

void init_queues() {
    CreateFila2(&ready);
    CreateFila2(&exec);
    CreateFila2(&blocked);
}

char ss_sp_scheduler[SIGSTKSZ];

void init_scheduler() {
    getcontext(&scheduler);
    scheduler.uc_link = &main_thread.context;
    scheduler.uc_stack.ss_sp = ss_sp_scheduler;
    scheduler.uc_stack.ss_size = SIGSTKSZ;
    makecontext(&scheduler, (void (*)(void))schedule, 0);
}

int ccreate (void *(*start)(void *), void *arg) {
    if (!intialized) {
        create_main_context();
        init_scheduler();
        init_queues();
        intialized = 1;
    }
    
    TCB_t* tcb = (TCB_t*) malloc(sizeof(TCB_t));
    tcb->state = THREAD_STATE_CREATION;
    tcb->ticket = generate_ticket();
    tcb->tid = generate_thread_id();
    if (getcontext(&tcb->context) == 0) {
        tcb->context.uc_link = &scheduler;
        tcb->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
        tcb->context.uc_stack.ss_size = SIGSTKSZ;
        makecontext(&(tcb->context), (void (*)(void)) start, 1, &arg);
        if (add_thread_to_ready_queue(tcb) == 0) {
            setcontext(&tcb->context);
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
