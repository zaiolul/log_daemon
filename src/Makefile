
BIN:=log_daemon
SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:.c=.o)

LIB:= -llink_core -lmiddleware_implementation -lplatform_port -lutils_modules
LDFLAGS:=-L../sdk/build/lib $(LIB)
INC:=-I../sdk/utils -I../sdk/include -I../sdk/interface -I../sdk/libraries/coreJSON/source/include \
-I../sdk/libraries/mbedtls/include  -I../sdk/libraries/mbedtls/include/mbedtls 
CC:=gcc
CFLAGS = -Wall

.PHONY: all clean

all:$(BIN)
	
$(BIN): $(SRCS)
	$(CC) -o $@ $^ $(INC) $(LDFLAGS) 

clean:
	rm $(BIN) 