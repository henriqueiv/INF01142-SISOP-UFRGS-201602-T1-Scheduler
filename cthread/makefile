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

all: objetos mv lib mv2

objetos: $(SRC_DIR)/cthread.c $(INC_DIR)/cdata.h $(INC_DIR)/cthread.h $(INC_DIR)/fila2.h
	$(CC) -c $(SRC_DIR)/cthread.c -Wall

mv:
	mv *.o $(BIN_DIR)

lib: $(BIN_DIR)/cthread.o
	ar crs libcthread.a $(BIN_DIR)/*.o 

mv2:
	mv *.a $(LIB_DIR)

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/cthread.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


