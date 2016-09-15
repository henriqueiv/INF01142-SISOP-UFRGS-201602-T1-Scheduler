//
//  cdata.h
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#ifndef cdata_h
#define cdata_h

typedef struct s_TCB { int tid;
    int state;
    int ticket; ucontext_t context;
} TCB_t;

typedef struct s_sem {
    int count; // indica se recurso está ocupado ou não (livre > 0, ocupado ≤ 0) PFILA2 fila; // ponteiro para uma fila de threads bloqueadas no semáforo
} csem_t;

#endif /* cdata_h */
