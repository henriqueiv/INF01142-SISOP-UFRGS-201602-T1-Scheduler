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
        DEBUG_PRINT("Fila vazia\n");
        return;
    }
    
    TCB_t* currentTCB;
    do {
        currentTCB = GetAtIteratorFila2(&queue);
        if (currentTCB == NULL) {
            break;
        }
        
        DEBUG_PRINT("tid(%d) -- ticket(%d)\n", currentTCB->tid, currentTCB->ticket);
    } while (NextFila2(&queue) == 0);
}

void print_joins() {
    if(FirstFila2(&joins) != 0) {
        DEBUG_PRINT("-\n");
        return;
    }
    
    join_t* join;
    do {
        join = GetAtIteratorFila2(&joins);
        if (join == NULL)
            return;
        DEBUG_PRINT("(%d) (%d)\n", join->blocked_thread->tid, join->target_thread->tid);
    } while (NextFila2(&joins) == 0);
    return;
}

void print_all_queues() {
    DEBUG_PRINT("Ready\n");
    print_queue(ready);
    DEBUG_PRINT("Blocked\n");
    print_queue(blocked);
    DEBUG_PRINT("Joins\n");
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
 @brief Retorna um identificador e incrementa o contador sequencial
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
        DEBUG_PRINT("Buscando TCB: Fila Vazia ou erro\n");
        return NULL;
    }
    
    TCB_t* thread;
    do {
        thread = GetAtIteratorFila2(queue);
        if (thread == NULL)
            return NULL;
        if (thread->tid == tid)
            return thread;
    } while (NextFila2(queue) == 0);
    return NULL;
}

void destroy_join(join_t* join) {
    DEBUG_PRINT("vai destruir join entre %d e %d\n",join->blocked_thread->tid, join->target_thread->tid);
    if(FirstFila2(&blocked) != 0) {
        DEBUG_PRINT("Fila de Aptos vazia ou ERRO\n");
        return;
    }
    
    TCB_t* thread;
    do {
        thread = GetAtIteratorFila2(&blocked);
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
        DEBUG_PRINT("SCHD: Fila de Joins vazia ou ERRO\n");
        return;
    }
    
    join_t* join;
    do {
        join = GetAtIteratorFila2(&joins);
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
#define WINNER_SHOULD_CHANGE 1
#define WINNER_SHOULD_NOT_CHANGE 0
int winner_should_change(TCB_t* winner, TCB_t* contestant, int ticket) {
    int winner_to_ticket = abs(winner->ticket - ticket);
    int contestant_to_ticket = abs(contestant->ticket - ticket);
    
    if (contestant_to_ticket > winner_to_ticket) {
        return WINNER_SHOULD_NOT_CHANGE;
    }
    
    if (winner_to_ticket > contestant_to_ticket) {
        return WINNER_SHOULD_CHANGE;
    }
    
    if (winner->tid > contestant-> tid) {
        return WINNER_SHOULD_CHANGE;
    }
    
    return WINNER_SHOULD_NOT_CHANGE;
}

TCB_t* find_next_thread() {
    if (FirstFila2(&ready) != 0) {
        DEBUG_PRINT("Fila de Aptos vazia. Ou Erro.\n");
        return NULL;
    }
    
    int ticket = generate_ticket();
    DEBUG_PRINT("TICKET = %d \n", ticket);
    TCB_t* winner = GetAtIteratorFila2(&ready);
    TCB_t* contestant;
    
    while (NextFila2(&ready) == 0) {
        contestant = GetAtIteratorFila2(&ready);
        // necessario testar o tid pois a lib support não está retornando NULL no ultimo
        if (contestant == NULL || contestant->tid == thread_id-1) break;
        if (winner_should_change(winner, contestant, ticket)) {
            winner = contestant;
        }
    }
    return winner;
}

#define REMOVE_THREAD_SUCCESS 0
#define REMOVE_THREAD_ERROR_OR_EMPTY_QUEUE -1
#define REMOVE_THREAD_TID_NOT_FOUND -2
int remove_thread(int tid, FILA2 queue) {
    if(FirstFila2(&queue) != 0) {
        DEBUG_PRINT("Fila vazia ou ERRO\n");
        return REMOVE_THREAD_ERROR_OR_EMPTY_QUEUE;
    }
    
    TCB_t* thread;
    do {
        thread = GetAtIteratorFila2(&queue);
        if (thread == NULL) {
            DEBUG_PRINT("tid %d not found", tid);
            return REMOVE_THREAD_TID_NOT_FOUND;
        }
        
        if (thread->tid == tid) {
            DEBUG_PRINT("REMOVTHREAD: target = %d, found = %d\n",tid, thread->tid );
            int success = DeleteAtIteratorFila2(&queue);
            DEBUG_PRINT("Delete: %d\n", success);
            return REMOVE_THREAD_SUCCESS;
        }
    } while (NextFila2(&queue) == 0);
    
    return REMOVE_THREAD_SUCCESS;
}

void schedule() {
    DEBUG_PRINT("***************** SCHEDULE *****************\n");
    print_all_queues();
    
    //se running thread nula, quer dizer que foi yield. logo, se não nula devemos assumir que a thread encerrou
    if (running_thread != NULL) {
        DEBUG_PRINT("Running Thread Existe; Assume que a thread encerrou\n");
        release_threads_from_tid(running_thread->tid);
        running_thread = NULL;
    }
    
    if (FirstFila2(&ready) != 0) {
        DEBUG_PRINT("Fila de Aptos vazia. Ou Erro.\n");
        return;
    }
    
    TCB_t* next_thread = find_next_thread();
    DEBUG_PRINT("next_thread: tid(%d)\n", next_thread->tid);
    print_all_queues();
    running_thread = next_thread;
    if (remove_thread(next_thread->tid, ready) != 0) {
        DEBUG_PRINT("problema ao deletar thread corrente da fila de aptos\n");
    }
    running_thread->state = THREAD_STATE_RUNNING;
    print_all_queues();
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
    DEBUG_PRINT("CCREATE chamado por: %d\n", running_thread->tid);
    
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
            DEBUG_PRINT("Criou %d\n", tcb->tid);
            return tcb->tid;
        } else {
            DEBUG_PRINT("Erro ao adicionar na fila");
            return CCREATE_ERROR;
        }
    } else {
        DEBUG_PRINT("Erro ao obter context");
        return CCREATE_ERROR;
    }
}

int cyield() {
    DEBUG_PRINT("CYIELD chamado por: %d\n", running_thread->tid);
    
    TCB_t* thread;
    thread = running_thread;
    thread->state = THREAD_STATE_READY;
    
    if(FirstFila2(&ready) != 0) {
        DEBUG_PRINT("Fila de Aptos Vazia, cyield negado!\n");
        return CYIELD_ERROR;
    }
    
    if (AppendFila2(&ready, (void*) thread) != 0) {
        DEBUG_PRINT("Erro adicionando thread a fila de aptos. cyield negado.\n");
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
        DEBUG_PRINT("CJOIN: Fila de Joins vazia ou ERRO\n");
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
    DEBUG_PRINT("CJOIN chamado por: %d\n Para esperar: %d\n", running_thread->tid, tid);
    print_all_queues();
    
    if(tid == MAIN_THREAD_ID) {
        DEBUG_PRINT("não é permitido dar join na main. cjoin negado. \n");
        return CJOIN_FAIL;
    }
    //achar tcb do tid
    TCB_t* target_thread = get_thread_with_id(tid, &ready);
    if (target_thread == NULL) {
        DEBUG_PRINT("Thread alvo não encontrada.\n");
        return CJOIN_FAIL;
    }
    //checar se thread alvo já tem alguem no aguardo
    if (is_thread_targeted(tid) == TARGETED) {
        DEBUG_PRINT("Já há uma thread esperando por está\n");
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
        DEBUG_PRINT("Erro ao alocar fila para o semaforo");
        return CSEM_INIT_ERROR_CREATE_QUEUE;
    }
    
    return CSEM_INIT_SUCCESS;
}

#define CWAIT_SUCCESS 0
#define CWAIT_ERROR_CREATE_QUEUE -1
int cwait (csem_t *sem) {
    DEBUG_PRINT("***************** CWAIT *****************\n");
    if (!initialized) {
        init();
    }
    
    if (sem->fila == NULL) {
        sem->fila = (PFILA2) malloc(sizeof(FILA2));
        if (CreateFila2(sem->fila) != 0) {
            DEBUG_PRINT("\n");
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

#define MOVE_THREAD_SUCCESS 0
#define MOVE_THREAD_ERROR_OR_EMPTY_QUEUE -1
#define MOVE_THREAD_TID_NOT_FOUND -2
int move_thread(int tid, FILA2 queue, FILA2 dest) {
    if(FirstFila2(&queue) != 0) {
        DEBUG_PRINT("Fila de Origem vazia ou ERRO\n");
        return MOVE_THREAD_ERROR_OR_EMPTY_QUEUE;
    }
    TCB_t* thread;
    do {
        thread = GetAtIteratorFila2(&queue);
        if (thread == NULL)
            return MOVE_THREAD_TID_NOT_FOUND;
        
        if (thread->tid == tid) {
            DEBUG_PRINT("MOVTHREAD: target = %d, found = %d\n",tid, thread->tid );
            DEBUG_PRINT("Append: %d\n", AppendFila2(&dest, &thread));
            DEBUG_PRINT("Delete:%d\n", DeleteAtIteratorFila2(&queue));
            return MOVE_THREAD_SUCCESS;
        }
    } while (NextFila2(&queue) == 0);
    
    return MOVE_THREAD_SUCCESS;
}

#define CSIGNAL_SUCCESS 0
#define CSIGNAL_ERROR_UNINITIALIZED -1
#define CSIGNAL_ERROR_REMOVE_THREAD_FROM_BLOCKED -2
int csignal (csem_t *sem) {
    DEBUG_PRINT("************** CSIGNAL **************\n");
    if (!initialized) {
        init();
    }
    
    if (sem->fila == NULL) {
        DEBUG_PRINT("Signal antes do wait ou semaforo nao inicializado");
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
    print_all_queues();
    DEBUG_PRINT("SIGNAL VAI LIBERAR THREAD: %d\n", thread->tid);
    DeleteAtIteratorFila2(sem->fila);
    
    if (move_thread(thread->tid, blocked, ready) != 0) {
        DEBUG_PRINT("Erro ao remover thread(%d) da fila de bloqueados", thread->tid);
        return CSIGNAL_ERROR_REMOVE_THREAD_FROM_BLOCKED;
    }
    print_all_queues();
    return CSIGNAL_SUCCESS;
}

#define CIDENTIFY_SUCCESS 0
int cidentify (char *name, int size) {
    DEBUG_PRINT("Henrique Valcanaia - 240501\nPietro Degrazia - 243666\n");
    return CIDENTIFY_SUCCESS;
}
