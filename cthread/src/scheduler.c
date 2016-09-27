//
//  scheduler.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "../include/scheduler.h"

TCB_t* running_thread;

PFILA2 ready;
PFILA2 exec;
PFILA2 blocked;

/*!
 @brief Partiremos do 1 pois a 0 será a main
 */
int threadId = 1;

int schedule() {
    TCB_t nextThread;
    if (getNextThread(&nextThread) == 0) {
        return 0;
    } else {
        return -1;
    }
}

int getNextThread(TCB_t* nextThread) {
    int randomTicket = generateTicket();
    if (getThreadClosestToTicket(randomTicket, nextThread) == 0) {
        return  0;
    } else {
        return -1;
    }
}

int getThreadClosestToTicket() {
    
}

/*!
 @brief Gera um numero aleatorio inteiro de 1 byte
 @discussion Utiliza a funcao Random2() da biblioteca support.o disponibilizada pelo professor
 @return Inteiro aleatorio de 1 byte
 */
int generateTicket() {
    int ticket = Random2() % 255;
    return ticket;
}

/*!
 @brief Gera um novo identificador de thread
 @discussion Este identificador é sequencial e não é reutilizado. Use com sabedoria.
 */
int generateThreadId() {
    threadId++;
    return threadId;
}

/*!
 @brief Adiciona um TCB a uma fila
 */
int addThreadToQueue(TCB_t* thread, PFILA2 queue) {
    int result = AppendFila2(queue, (void*) thread);
    return result;
}

/*!
 @brief Adiciona um TCB a fila de aptos
 */
int addThreadToReadyQueue(TCB_t* thread) {
    int result = addThreadToQueue(thread, ready);
    return result;
}

/*!
 @brief Adiciona um TCB a fila de bloqueados
 */
int addThreadToBlockedQueue(TCB_t* thread) {
    int result = addThreadToQueue(thread, blocked);
    return result;
}

int findTid(int tid, PFILA2 queue) {
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

int isReady(int tid) {
    return findTid(tid, ready);
}

int isBlocked(int tid) {
    return findTid(tid, blocked);
}

TCB_t* findThread(tid) {
    
}

TCB_t* get_running_thread() {
    return running_thread;
}
