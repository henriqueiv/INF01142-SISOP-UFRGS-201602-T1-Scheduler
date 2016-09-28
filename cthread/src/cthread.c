//
//  cthread.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "../include/cthread.h"

#define CCREATE_ERROR -1
#define CYIELD_ERROR -1
#define MAIN_THREAD_ID 0

#define DEBUG 3

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
__FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

// ================ LOCAL VARS ================

TCB_t* running_thread;

FILA2 ready;
FILA2 blocked;

/*!
 @brief Partiremos do 1 pois a 0 será a main
 */
int thread_id = 1;


// ================ AUXILIAR FUNCTIONS ================

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

int get_next_thread(TCB_t* next_thread) {
    int random_ticket = generate_ticket();
    if (get_thread_closest_to_ticket(random_ticket, next_thread) == 0) {
        return  0;
    } else {
        return -1;
    }
}

/*!
 @brief Gera um novo identificador de thread
 @discussion Este identificador é sequencial e não é reutilizado. Use com sabedoria.
 */
int generate_thread_id() {
    return thread_id++;
}

/*!
 @brief Adiciona um TCB a uma fila
 */
int add_thread_to_queue(TCB_t thread, FILA2 queue) {
    int result = AppendFila2(&queue, (void*) &thread);
    return result;
}

/*!
 @brief Adiciona um TCB a fila de aptos
 */
int add_thread_to_ready_queue(TCB_t thread) {
    int result = add_thread_to_queue(thread, ready);
    return result;
}

/*!
 @brief Adiciona um TCB a fila de bloqueados
 */
int add_thread_to_blocked_queue(TCB_t thread) {
    int result = add_thread_to_queue(thread, blocked);
    return result;
}

/*!
 @brief Procura um TCB na fila "queue" com o thread id "tid"
 */
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

// ================ SCHEDULE ================

void schedule() {
    DEBUG_PRINT("SCHEDULE\n");
    if (FirstFila2(&ready) != 0) {
        DEBUG_PRINT("ERRO OU FILA VAZIA\n");
        return;
    }
    
    TCB_t* next_thread = GetAtIteratorFila2(&ready);
    DEBUG_PRINT("next_thread: tid(%d)\n", next_thread->tid);
    
    running_thread = next_thread;
    DeleteAtIteratorFila2(&ready);
    running_thread->state = THREAD_STATE_RUNNING;
    
    setcontext(&running_thread->context);
}

// ================ CTHREAD ================

int initialized = 0;
ucontext_t scheduler;
TCB_t main_thread;

char ss_sp_scheduler[SIGSTKSZ];
void init_scheduler() {
    getcontext(&scheduler);
    scheduler.uc_link = &(main_thread.context);
    scheduler.uc_stack.ss_sp = ss_sp_scheduler;
    scheduler.uc_stack.ss_size = SIGSTKSZ;
    makecontext(&scheduler, (void (*)(void))schedule, 0);
}

void create_main_tcb() {
    main_thread.tid = MAIN_THREAD_ID;
    main_thread.state = THREAD_STATE_RUNNING;
    main_thread.ticket = generate_ticket();
    getcontext(&main_thread.context);
    
    running_thread = &main_thread;
}

void init_queues() {
    CreateFila2(&ready);
    CreateFila2(&blocked);
}

int ccreate (void *(*start)(void *), void *arg) {
    if (!initialized) {
        init_scheduler();
        create_main_tcb();
        init_queues();
        initialized = 1;
    }
    
    ucontext_t context;
    if (getcontext(&context) == 0) {
        char* stack = (char*) malloc(SIGSTKSZ);
        if (stack == NULL) {
            DEBUG_PRINT("Recursos insuficientes para criar nova thread!");
            return CCREATE_ERROR;
        }
        
        context.uc_link = &scheduler;
        context.uc_stack.ss_sp = stack;
        context.uc_stack.ss_size = SIGSTKSZ;
        makecontext(&context, (void (*)(void)) start, 1, arg);
        
        TCB_t* tcb = (TCB_t*) malloc(sizeof(TCB_t));
        tcb->state = THREAD_STATE_CREATION;
        tcb->ticket = generate_ticket();
        tcb->tid = generate_thread_id();
        tcb->context = context;
        if (AppendFila2(&ready, (void*) tcb) == 0) {
            return tcb->tid;
        } else {
            return CCREATE_ERROR;
        }
    } else {
        return CCREATE_ERROR;
    }
}

int cyield() {
    DEBUG_PRINT("Running: %d\n", running_thread->tid);
    
    TCB_t* thread;
    thread = running_thread;
    thread->state = THREAD_STATE_READY;
    
    running_thread = NULL;
    
    swapcontext(&thread->context, &scheduler);
    
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
        // join_list[tid] = &running_thread;
        // add_thread_to_blocked_queue(running_thread);
        
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
    printf("Henrique Valcanaia - 240501\nPietro Degrazia - 243666\n");
    return 1;
}
