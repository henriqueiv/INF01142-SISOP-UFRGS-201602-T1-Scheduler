
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
    DEBUG_PRINT("inicio thread 1.\n");
    if (cwait(&mutex) != 0) {
        DEBUG_PRINT("T1 Erro: CWAIT\n");
        return 0;
    }
    DEBUG_PRINT("T1 NA ZONA CRITICA. yield.\n");
    cyield();
    if (csignal(&mutex) != 0) {
        DEBUG_PRINT("T1 Erro: CSIGNAL\n");
        return 0;
    }
    DEBUG_PRINT("T1 FORA DA ZONA\n");
    // ccreate(func2, NULL);
    DEBUG_PRINT("Thread 1: criou thread 2 e vai dar yield.\n");
    // cyield();
    DEBUG_PRINT("fim da thread UM\n");
    return 0;
}

// void* func2(void *arg) {
//     DEBUG_PRINT("inicio thread 2\n");
//     ccreate(func3, NULL);
//     DEBUG_PRINT("Thread 2: criou thread 3 e vai dar yield.\n");
//     cyield();
//     DEBUG_PRINT("fim da thread DOIS\n");
//     return 0;
// }

void* t2() {
    DEBUG_PRINT("Executando...\n");
    DEBUG_PRINT("Finalizada.\n");
}

void* t1() {
    DEBUG_PRINT("Antes de criar thread 2\n");
    int id2 = ccreate(t2, NULL);
    DEBUG_PRINT("Depois de criar thread 2, tid: %d\n", id2);
    cjoin(id2);
    DEBUG_PRINT("Depois do join na t2..\n");
}

struct tupla {
    int x;
    int tid;
};

void print_tupla(struct tupla* par) {
    printf("Tupla { x: %d, tid: %d }\n", par->x, par->tid);
}

void* print(void* x) {
    struct tupla* par = (struct tupla*) x;
    print_tupla(par);
    int id = ccreate(print, (void*) &par);
    if (par->x == 0) {
        DEBUG_PRINT("X == 0\n");
        printf("%d\n", par->x);
    }
    
    DEBUG_PRINT("PASSOU\n");
    cjoin(id);
    print(par->x-1);
}

void* printi(void* x) {
    printf("%d", *(int*) x);
}

void* hprint() {
    DEBUG_PRINT("print eu\n");
}

#define SIZE 10

int main(int argc, char *argv[]) {
    DEBUG_PRINT("Execussstando\n");
    
    int threads[SIZE], i;
    
    for (i = 0; i < SIZE; i++) {
        threads[i] = ccreate(printi, (void*) &i);
    }
    
    for (i = 0; i < SIZE; i++) {
        cjoin(threads[i]);
    }
    
    
    /*
     
    struct tupla* par = (struct tupla*) malloc(sizeof(struct tupla));
    if (par == NULL) {
        printf("nao tao me dando memoria\n");
    }
    
    par->tid = 0; // main
    par->x = 100;
    print(&par);
    
     DEBUG_PRINT("Criando t1\n");
     int id0 = ccreate(t1, NULL);
     DEBUG_PRINT("Thread 1 criada com tid %d", id0);
     
     DEBUG_PRINT("Efetuando join na t1 de tid %d\n", id0);
     cjoin(id0);
     */
    
    DEBUG_PRINT("Finalizando...\n");
    
    return 0;
    
    /*
     if (csem_init(&mutex, 1) != 0){
     DEBUG_PRINT("Erro ao criar MUTEX\n");
     return 0;
     }
     
     ccreate(func1, NULL);
     
     if (cwait(&mutex) != 0) {
     DEBUG_PRINT("Main Erro: CWAIT\n");
     return 0;
     }
     
     DEBUG_PRINT("Main: ZONA CRITICA. yield.\n");
     cyield();
     
     if (csignal(&mutex) != 0) {
     DEBUG_PRINT("Main Erro: CSIGNAL\n");
     return 0;
     }
     
     DEBUG_PRINT("MAIN FORA DA ZONA. yield.\n");
     cyield();
     */
    
    return 0;
}
