//
//  cdata.h
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#ifndef cdata_h
#define cdata_h
#include "support.h"
#include <ucontext.h>

enum THREAD_STATE {
    CREATION = 0,
    READY = 1,
    EXEC = 2,
    BLOCKED = 3,
    FINISH = 4
};

/*!
 @struct s_TCB
 @abstract Struct que representa um TCB(Thread Control Block)
 @field tid Thread id
 @field state Estado atual da thread (THREAD_STATE)
 @field ticket Ticket sorteado pelo escalonador
 @field context ucontext_t
 @discussion Struct utilizada para definicao do TCB. É importante ressaltar que o tipo ucontex_t NÃO é mais utilizado em versões recentes de UNIX/POSIX.
 */
typedef struct s_TCB {
    int tid;
    int state;
    int ticket;
    ucontext_t context;
} TCB_t;

typedef struct s_sem {
    int count; // indica se recurso está ocupado ou não (livre > 0, ocupado ≤ 0)
    PFILA2 fila; // ponteiro para uma fila de threads bloqueadas no semáforo
} csem_t;

struct TCB_t* join_list[999];

#endif /* cdata_h */
