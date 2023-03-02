
BIN:=my_daemon
SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:.c=.o)
LIBDIR:=-L../sdk/build/lib
LIB:= -llink_core -lmiddleware_implementation -lplatform_port -lutils_modules
INC:=-I../sdk/utils -I../sdk/include -I../sdk/interface -I../sdk/libraries/coreJSON/source/include \
-I../sdk/libraries/mbedtls/include  -I../sdk/libraries/mbedtls/include/mbedtls 


CC:=gcc
C_FLAGS = -Wall

all:$(BIN)
	

$(BIN):
	$(CC) $(SRCS) -o $@ $(INC) $(LIBDIR) $(LIB) 

clean:
	rm $(BIN)
