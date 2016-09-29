#
# Makefile ESQUELETO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "fila2.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TST_DIR=./teste

all: objetos mvObj libcthread.a mvLib main test

objetos: $(SRC_DIR)/cthread.c $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(INC_DIR)/support.h
	$(CC) -c $(SRC_DIR)/cthread.c -Wall

mvObj:
	mv *.o $(BIN_DIR)

libcthread.a: $(BIN_DIR)/cthread.o
	ar crs libcthread.a $(BIN_DIR)/*.o

mvLib:
	mv *.a $(LIB_DIR)

main:
	$(CC) -o myprog main.c -L$(LIB_DIR) -lcthread -Wall

testeCreate:
	$(CC) -o $(TST_DIR)/testeCreate $(TST_DIR)/test_ccreate.c -L$(LIB_DIR) -lcthread -Wall

test:
	$(CC) -o $(TST_DIR)/test_cjoin $(TST_DIR)/test_cjoin.c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf myprog $(LIB_DIR)/*.a $(BIN_DIR)/cthread.o $(TST_DIR)/*.o $(TST_DIR)/testeCreate $(SRC_DIR)/*~ $(INC_DIR)/*~ *~