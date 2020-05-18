###############################################
#
# Makefile - for tclSurrogate/test
#
#
#-----------------------------------------------------------------------
#    Copyright (C) 2009 FCSoftware Inc. 
#
#    This software is in the public domain.
#    Permission to use, copy, modify, and distribute this software and its
#    documentation for any purpose and without fee is hereby granted, 
#    without any conditions or restrictions.
#    This software is provided "as is" without express or implied warranty.
#
#    If you discover a bug or add an enhancement contact us on the
#    SIMPL project mailing list.
#-----------------------------------------------------------------------
#
#$Log: Makefile,v $
#Revision 1.4  2011/03/23 18:00:14  bobfcsoft
#removed space in -L option for Mac compatibility
#
#Revision 1.3  2009/06/12 12:21:57  bobfcsoft
#fixed places that make looks for headers
#
#Revision 1.2  2009/04/14 20:59:49  bobfcsoft
#added i386 target
#
#Revision 1.1  2009/01/19 20:16:02  bobfcsoft
#relocated files
#
#
###############################################

CC=cc
DE=NODEBUG

PROGRAM_NAME=
line1 = --------------------------------------------------------------------
line2 = ====================================================================

DEST_DIR=.

OBJ_DIR=obj
LIB_DIR=../lib
BIN_DIR=../bin
SIMPL_DIR=../../..

INSTALL_LIB_DIR=$(SIMPL_DIR)/lib
INSTALL_BIN_DIR=$(SIMPL_DIR)/bin

LOCAL_INCL_DIR=../include
INCL_DIR=$(SIMPL_DIR)/include
SIMPL_INCL_DIR=$(SIMPL_DIR)/simplipc/include
FCLOGGER_INCL_DIR=$(SIMPL_DIR)/fclogger/include

SOCKET_SENDER_OBJECT =\
	$(OBJ_DIR)/socketSender.o

FIFO_SENDER_OBJECT =\
	$(OBJ_DIR)/fifoSender.o

FIFO_RECEIVER_OBJECT =\
	$(OBJ_DIR)/fifoReceiver.o

CFLAGS=\
	-g\
	-Wall \
	-I $(LOCAL_INCL_DIR)\
	-I $(SIMPL_INCL_DIR)\
	-I $(FCLOGGER_INCL_DIR)\
	-I $(INCL_DIR)

LDFLAGS=\
	-L$(SIMPL_DIR)/fclogger/lib\
	-lsimpllog\
	-L$(SIMPL_DIR)/simplipc/lib\
	-lsimpl\
	-L$(LIB_DIR)\
	-lfcsocket

LDFLAGS2=\
	-L$(SIMPL_DIR)/lib\
	-lsimpl\
	-L$(LIB_DIR)\
	-lfcsocket

ARCHFLAGS =


#========================
# Default targets
#========================
all:\
     title \
     $(BIN_DIR)/socketSender \
     $(BIN_DIR)/fifoReceiver \
     $(BIN_DIR)/fifoSender
	@echo tclSurrogate all

title:
	@echo tclSurrogate/test build starting

description:
	@(\
	echo;\
	echo "$(line1)";\
	echo "Compiling $(PROGRAM_NAME) -> $(OBJ_NAME)";\
	echo )

#========================
# Link
#========================
$(BIN_DIR)/fifoReceiver: $(FIFO_RECEIVER_OBJECT)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -o $(BIN_DIR)/fifoReceiver $(FIFO_RECEIVER_OBJECT) $(LDFLAGS)

$(BIN_DIR)/fifoSender: $(FIFO_SENDER_OBJECT)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -o $(BIN_DIR)/fifoSender $(FIFO_SENDER_OBJECT) $(LDFLAGS)

$(BIN_DIR)/socketSender: $(SOCKET_SENDER_OBJECT)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -o $(BIN_DIR)/socketSender $(SOCKET_SENDER_OBJECT) $(LDFLAGS)

#========================
# Compiles
#========================
$(OBJ_DIR)/fifoReceiver.o : fifoReceiver.c
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/fifoReceiver.o fifoReceiver.c

$(OBJ_DIR)/fifoSender.o : fifoSender.c
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/fifoSender.o fifoSender.c

$(OBJ_DIR)/socketSender.o : socketSender.c
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/socketSender.o socketSender.c

#========================
# cleanup
#========================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/socketSender
	@-rm -f $(BIN_DIR)/fifoSender
	@-rm -f $(BIN_DIR)/fifoReceiver
	@echo tclSurrogate clean

#========================
# install
#========================
install:\
     title \
     $(BIN_DIR)/socketSender \
     $(BIN_DIR)/fifoSender\
     $(BIN_DIR)/fifoReceiver
	@echo tclSurrogate install

#===================
#  i386
#===================
i386:
	make install ARCHFLAGS="-march=i386"
	@echo tclSurrogate i386

