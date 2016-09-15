//
//  scheduler.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include "scheduler.h"

PFILA2 ready;
PFILA2 exec;
PFILA2 waiting;

/*!
 @brief Partiremos do 1 pois a 0 será a main
 */
int threadId = 1;

int schedule() {
    int tid = -1;
    return tid;
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


int generateThreadId() {
    threadId++;
    return threadId;
}

/*!
 @brief Adiciona um TCB a fila de aptos
 */
void addThreadToReadyQueue(TCB_t* thread) {
    InsertAfterIteratorFila2(ready, (void*) thread);
}
