//
//  cthread.h
//  cthread
//
//  Created by Henrique Valcanaia on 15/09/16.
//  Copyright © 2016 Henrique Valcanaia. All rights reserved.
//

#ifndef cthread_h
#define cthread_h

#define CYIELD_SUCCESS 0
#define CYIELD_ERROR -1

//typedef int CCREATE_STATUS;

enum CCREATE_STATUS {
    ERROR = -1
};

#include "cdata.h"

/*!
 @brief Efetua cedencia voluntária de CPU
 @param start ponteiro para a função que a thread executará.
 @param arg um parâmetro que pode ser passado para a thread na sua criação. (Obs.: é um único parâmetro. Se for necessário passar mais de um valor deve-se empregar um ponteiro para uma struct)
 @return Quando executada corretamente retorna um valor positivo, que representa o identificador da thread criada, caso contrário, retorna CCREATE_ERROR (valor negativo)
 */
enum CCREATE_STATUS ccreate (void *(*start)(void *), void *arg);


/*!
 @brief Efetua cedencia voluntária de CPU
 @return Quando executada corretamente retorna CYIELD_SUCCESS (0), caso contrário, retorna CYIELD_ERROR (-1)
 */
int cyield(void);


/*!
 @brief Bloqueia a execução de uma thread aguardando o término da thread com id indicado no parametro 'tid'
 @discussion Exemplo para geração de um tid válido
 
 @code
 int id0, id1, i;
 id0 = ccreate(func0, (void *)&i);
 id1 = ccreate(func1, (void *)&i);
 
 printf("Eu sou a main após a criação de ID0 e ID1\n");
 
 cjoin(id0);
 cjoin(id1);
 
 printf("Eu sou a main voltando para terminar o programa\n");
 @endcode
 
 
 @param tid identificador da thread cujo término está sendo aguardado. Caso seja informado um tid inválido a função retornará um valor negativo indicando erro.
 @return Quando executada corretamente retorna CJOIN_SUCCESS (0 zero). Caso contrário, retorna CJOIN_ERROR (um valor negativo)
 */
int cjoin(int tid);


/*!
 @param sem ponteiro para uma variável do tipo csem_t. Aponta para uma estrutura de dados que representa a variável semáforo. 
 @param count valor a ser usado na inicialização do semáforo. Representa a quantidade de recursos controlador pelo semáforo.
 @return Quando executada corretamente: retorna 0 (zero) Caso contrário, retorna um valor negativo.
 */
int csem_init (csem_t *sem, int count);

/*!
 @param sem ponteiro para uma variável do tipo semáforo.
 @return Quando executada corretamente retorna CWAIT_SUCCESS (0 zero), caso contrário, retorna CWAIT_ERROR (um valor negativo)
 */
int cwait (csem_t *sem);

/*!
 @param sem ponteiro para uma variável do tipo semáforo. 
 @return Quando executada corretamente retorna CSIGNAL_SUCCESS (0 zero), caso contrário, retorna CSIGNAL_ERROR (um valor negativo)
 */
int csignal (csem_t *sem);

/*!
 @param name ponteiro para uma área de memória onde deve ser escrito um string que contém os nomes dos componentes do grupo e seus números de cartão. Deve ser uma linha por componente.
 @param size quantidade máxima de caracteres que podem ser copiados para o string de identificação dos componentes do grupo.
 @return Quando executada corretamente retorna CIDENTIFY_SUCCESS (0 zero), caso contrário, retorna CIDENTIFY_ERROR(um valor negativo)
*/
int cidentify (char *name, int size);


#endif /* cthread_h */
