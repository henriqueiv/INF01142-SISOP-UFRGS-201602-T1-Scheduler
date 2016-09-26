//
//  scheduler.h
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#ifndef scheduler_h
#define scheduler_h

#include "support.h"
#include "cdata.h"

/*!
 @brief Usada para selecionar o próximo processo a ir para a CPU
 @return Retorna o tid da thread que ganhará a CPU. Em caso de não existir nenhuma thread para ser executada retornará SCHEDULE_ERROR(-1).
 */
int schedule();

int generateThreadId();

int generateTicket();

int addThreadToReadyQueue(TCB_t* thread);

int addThreadToBlockedQueue(TCB_t* thread);

int isReady(int tid);

int isBlocked(int tid);

TCB_t* get_running_thread();

#endif /* scheduler_h */
