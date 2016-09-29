//
//  cthread.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "../include/cthread.h"

#define FREE(x) do { if ((x) != NULL) {free(x); x = NULL;} } while(0)

#define CCREATE_ERROR -1
#define CYIELD_ERROR -1
#define MAIN_THREAD_ID 0

// ================ LOCAL VARS ================

TCB_t* running_thread;

FILA2 ready;
FILA2 blocked;
FILA2 joins;

/*!
 @brief Partiremos do 1 pois a 0 será a main
 */
int thread_id = 1;

// ================ AUXILIAR FUNCTIONS ================

void print_queue(FILA2 queue) {
    if (FirstFila2(&queue) != 0) {
        printf("-\n");
        return;
    }
    
    TCB_t* currentTCB = (TCB_t*) malloc(sizeof(TCB_t));
    int i = 0;
    do {
        currentTCB = GetAtIteratorFila2(&queue);
        if (currentTCB == NULL)
            break;

        printf("tid(%d)\n", currentTCB->tid);
        i++;
    } while (NextFila2(&queue) == 0);
}

void print_joins() {
     if(FirstFila2(&joins) != 0) {
        printf("-\n");
        return;
    }
    do {
        join_t* join = GetAtIteratorFila2(&joins);
        if (join == NULL)
            return;
        printf("(%d) (%d)\n",join->blocked_thread->tid, join->target_thread->tid);
    } while (NextFila2(&joins) == 0);
    return;
}

void print_all_queues() {
    printf("Ready\n");
    print_queue(ready);
    printf("Blocked\n");
    print_queue(blocked);
    printf("Joins\n");
    print_joins();
}

/*!
 @brief Partiremos do 1 pois a 0 será a main
 */
int get_thread_closest_to_ticket(int ticket, TCB_t *next_thread) {
    return 1;
}

/*!
 @brief Gera um numero aleatorio inteiro de 1 byte
 @discussion Utiliza a funcao Random2() da biblioteca support.o disponibilizada pelo professor. O
 @return Inteiro aleatorio de 1 byte
 */
int generate_ticket() {
    int ticket = Random2() % 256;
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
 @brief Retorna um TCB na fila "queue" com o thread id "tid"
 */
TCB_t* get_thread_with_id(int tid, PFILA2 queue) {
    if (FirstFila2(queue) != 0) {
        printf("Buscando TCB: Fila Vazia ou erro\n");
        return NULL;
    }
    do {
        TCB_t* thread = GetAtIteratorFila2(queue);
        if (thread == NULL)
            return NULL;
        if (thread->tid == tid)
            return thread;
    } while (NextFila2(queue) == 0);
    return NULL;
}

void destroy_join(join_t* join) {
    printf("vai destruir join entre %d e %d\n",join->blocked_thread->tid, join->target_thread->tid);
    if(FirstFila2(&blocked) != 0) {
        printf("Fila de Aptos vazia ou ERRO\n");
        return;
    }
    do {
        TCB_t* thread = GetAtIteratorFila2(&blocked);
        if (thread == NULL)
            return;
        if (thread == join->blocked_thread) {
            thread->state = THREAD_STATE_READY;
            DeleteAtIteratorFila2(&blocked);
            AppendFila2(&ready, (void*) thread);
            return;
        }
    } while (NextFila2(&blocked) == 0);
    return;
}

void release_threads_from_tid(int tid) {
    if(FirstFila2(&joins) != 0) {
        printf("SCHD: Fila de Joins vazia ou ERRO\n");
        return;
    }
    do {
        join_t* join = GetAtIteratorFila2(&joins);
        if (join == NULL)
            return;
        if (join->target_thread->tid == tid) {
            destroy_join(join);
            return;
        }
    } while (NextFila2(&joins) == 0);
    return;
}

// ================ SCHEDULE ================

void schedule() {
    printf("SCHEDULE*********************\n");
    print_all_queues();
    
    //se running thread nula, quer dizer que foi yield. logo, se não nula devemos assumir que a thread encerrou
    if (running_thread != NULL) {
        printf("Running Thread Existe; Assume que a thread encerrou\n");
        release_threads_from_tid(running_thread->tid);
        running_thread = NULL;
    }
    
    if (FirstFila2(&ready) != 0) {
        printf("ERRO OU FILA VAZIA\n");
        return;
    }
    
    TCB_t* next_thread = GetAtIteratorFila2(&ready);
    printf("next_thread: tid(%d)\n", next_thread->tid);
    
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
    CreateFila2(&joins);
}

void init() {
    init_scheduler();
    create_main_tcb();
    init_queues();
    initialized = 1;
}

int ccreate (void *(*start)(void *), void *arg) {
    if (!initialized) {
        init();
    }
    printf("CCRIATE chamado por: %d\n", running_thread->tid);

    ucontext_t context;
    if (getcontext(&context) == 0) {
        char tcb_stack[SIGSTKSZ];
        context.uc_link = &scheduler;
        context.uc_stack.ss_sp = tcb_stack;
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
    printf("CYIELD chamado por: %d\n", running_thread->tid);
    
    TCB_t* thread;
    thread = running_thread;
    thread->state = THREAD_STATE_READY;
    
    if(FirstFila2(&ready) != 0) {
        printf("Fila de Aptos Vazia, cyield negado!\n");
        return CYIELD_ERROR;
    }
    
    if (AppendFila2(&ready, (void*) thread) != 0) {
        printf("Erro adicionando thread a fila de aptos. cyield negado.\n");
        return CYIELD_ERROR;
    }
    
    running_thread = NULL;
    
    swapcontext(&thread->context, &scheduler);
    
    return CYIELD_SUCCESS;
}

#define NOT_TARGETED 0
#define TARGETED 1
int is_thread_targeted(int tid) {
    if(FirstFila2(&joins) != 0) {
        printf("CJOIN: Fila de Joins vazia ou ERRO\n");
        return NOT_TARGETED;
    }
    do {
        join_t* join = GetAtIteratorFila2(&joins);
        if (join == NULL)
            return NOT_TARGETED;
        if (join->target_thread->tid == tid)
            return TARGETED;
    } while (NextFila2(&joins) == 0);
    return NOT_TARGETED;
}

#define CJOIN_SUCCESS 0
#define CJOIN_THREAD_FINISHED -1
#define CJOIN_THREAD_ALREADY_JOINED -2
#define CJOIN_FAIL -3
int cjoin(int tid) {
    printf("CJOIN chamado por: %d\nPara esperar: %d", running_thread->tid, tid);
    print_all_queues();
    
    if(tid == MAIN_THREAD_ID) {
        printf("não é permitido dar join na main. cjoin negado. \n");
        return CJOIN_FAIL;
    }
    //achar tcb do tid
    TCB_t* target_thread = get_thread_with_id(tid, &ready);
    if (target_thread == NULL) {
        printf("Thread alvo não encontrada.\n");
        return CJOIN_FAIL;
    }
    //checar se thread alvo já tem alguem no aguardo
    if (is_thread_targeted(tid) == TARGETED) {
        printf("Já há uma thread esperando por está\n");
        return CJOIN_THREAD_ALREADY_JOINED;
    }
    //cria o join
    join_t* join = (join_t*) malloc(sizeof(join_t));
    join->blocked_thread = running_thread;
    join->target_thread = target_thread;
    AppendFila2(&joins, (void*) join);
    
    TCB_t* calling_thread = running_thread;
    calling_thread->state = THREAD_STATE_BLOCKED;
    
    AppendFila2(&blocked, (void*)calling_thread);
    running_thread = NULL;
    swapcontext(&calling_thread->context, &scheduler);
    return CJOIN_SUCCESS;
}

#define CSEM_INIT_SUCCESS 0
#define CSEM_INIT_ERROR_CREATE_QUEUE -1
int csem_init (csem_t *sem, int count) {
    sem->count = count;
    sem->fila = (PFILA2) malloc(sizeof(FILA2));
    
    if (CreateFila2(sem->fila) != 0) {
        printf("Erro ao alocar fila para o semaforo");
        return CSEM_INIT_ERROR_CREATE_QUEUE;
    }
    
    return CSEM_INIT_SUCCESS;
}

#define CWAIT_SUCCESS 0
#define CWAIT_ERROR_CREATE_QUEUE -1
int cwait (csem_t *sem) {
    if (!initialized) {
        init();
    }
    
    if (sem->fila == NULL) {
        sem->fila = (PFILA2) malloc(sizeof(FILA2));
        if (CreateFila2(sem->fila) != 0) {
            printf("\n");
            return CWAIT_ERROR_CREATE_QUEUE;
        }
    }
    
    sem->count--;
    // sem recursos disponivel
    if (sem->count < 0) {
        TCB_t* thread;
        thread = running_thread;
        thread->state = THREAD_STATE_BLOCKED;
        
        AppendFila2(&blocked, (void*) thread);
        AppendFila2(sem->fila, (void*) thread);
        
        running_thread = NULL;
        
        swapcontext(&thread->context, &scheduler);
    }
    
    return CWAIT_SUCCESS;
}

#define REMOVE_THREAD_SUCCESS 0
#define REMOVE_THREAD_ERROR_OR_EMPTY_QUEUE -1
#define REMOVE_THREAD_TID_NOT_FOUND -2
int remove_thread(int tid, FILA2 queue) {
    if(FirstFila2(&queue) != 0) {
        printf("Fila de Aptos vazia ou ERRO\n");
        return REMOVE_THREAD_ERROR_OR_EMPTY_QUEUE;
    }
    
    do {
        TCB_t* thread = GetAtIteratorFila2(&queue);
        if (thread == NULL)
            return REMOVE_THREAD_TID_NOT_FOUND;
        
        if (thread->tid == tid) {
            DeleteAtIteratorFila2(&queue);
            return REMOVE_THREAD_SUCCESS;
        }
    } while (NextFila2(&blocked) == 0);
    
    return REMOVE_THREAD_SUCCESS;
}

#define CSIGNAL_SUCCESS 0
#define CSIGNAL_ERROR_UNINITIALIZED -1
#define CSIGNAL_ERROR_REMOVE_THREAD_FROM_BLOCKED -2
int csignal (csem_t *sem) {
    if (!initialized) {
        init();
    }
    
    if (sem->fila == NULL) {
        printf("Signal antes do wait ou semaforo nao inicializado");
        return CSIGNAL_ERROR_UNINITIALIZED;
    }
    
    if (FirstFila2(sem->fila) != 0) {
        FREE(sem->fila);
        return CSIGNAL_SUCCESS;
    }
    
    sem->count++;
    TCB_t* thread;
    thread = (TCB_t*) GetAtIteratorFila2(sem->fila);
    thread->state = THREAD_STATE_READY;
    DeleteAtIteratorFila2(sem->fila);
    
    if (remove_thread(thread->tid, blocked) != 0) {
        printf("Erro ao remover thread(%d) da fila de bloqueados", thread->tid);
        return CSIGNAL_ERROR_REMOVE_THREAD_FROM_BLOCKED;
    }
    
    return CSIGNAL_SUCCESS;
}

#define CIDENTIFY_SUCCESS 0
int cidentify (char *name, int size) {
    printf("Henrique Valcanaia - 240501\nPietro Degrazia - 243666\n");
    return CIDENTIFY_SUCCESS;
}
