
//
//  main.c
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#include <stdio.h>
#include "include/cthread.h"
#define _XOPEN_SOURCE 600
#define THREAD_COUNT 20

void* func1(void *arg);
// void* func2(void *arg);
csem_t mutex;


void* func1(void *arg) {
    printf("inicio thread 1.\n");
    if (cwait(&mutex) != 0) {
        printf("T1 Erro: CWAIT\n");
        return 0;
    }
    printf("T1 NA ZONA CRITICA. yield.\n");
    cyield();
    if (csignal(&mutex) != 0) {
        printf("T1 Erro: CSIGNAL\n");
        return 0;
    }
    printf("T1 FORA DA ZONA\n");
    // ccreate(func2, NULL);
    // printf("Thread 1: criou thread 2 e vai dar yield.\n");
    // cyield();
    // printf("fim da thread UM\n");
    return 0;
}

// void* func2(void *arg) {
//     printf("inicio thread 2\n");
//     ccreate(func3, NULL);
//     printf("Thread 2: criou thread 3 e vai dar yield.\n");
//     cyield();
//     printf("fim da thread DOIS\n");
//     return 0;
// }

void* t2() {
    printf("2 rodando\n");
    printf("2 acabou\n");
}

void* t1() {
    printf("t1 antes de criar 2"\n);
    int id2 = ccreate(t2, NULL);
    printf("t1 depois de criar %d\n", id2);
    exit(0);
    cjoin(id2);
    printf("t1 depois do join na t2\n");
}

int main(int argc, char *argv[]) {
    int id0 = ccreate(t1, NULL);
    cjoin(id0);
    
    // cria 1
    // main join 1
    // 1 cria 2
    // 1 join 2
    // 1 print
    // print
    
    
    return 0;
    
    /*
    if (csem_init(&mutex, 1) != 0){
        printf("Erro ao criar MUTEX\n");
        return 0;
    }
    
    ccreate(func1, NULL);
    
    if (cwait(&mutex) != 0) {
        printf("Main Erro: CWAIT\n");
        return 0;
    }
    
    printf("Main: ZONA CRITICA. yield.\n");
    cyield();
    
    if (csignal(&mutex) != 0) {
        printf("Main Erro: CSIGNAL\n");
        return 0;
    }
    
    printf("MAIN FORA DA ZONA. yield.\n");
    cyield();
    */
    
    return 0;
}
