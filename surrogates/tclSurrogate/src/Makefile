###############################################
#
# Makefile - for tclSurrogate 
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
#
# $Log: Makefile,v $
# Revision 1.8  2011/09/08 16:58:55  bobfcsoft
# added CYGWIN logic
#
# Revision 1.7  2011/03/23 17:54:37  bobfcsoft
# removed space in -L option for Mac compatibility
#
# Revision 1.6  2010/01/30 11:10:46  bobfcsoft
# NO_ZOMBIE default
#
# Revision 1.5  2009/06/12 12:21:44  bobfcsoft
# fixed places that make looks for headers
#
# Revision 1.4  2009/04/13 17:37:59  bobfcsoft
# added i386 target
#
# Revision 1.3  2009/01/22 13:53:27  bobfcsoft
# header fixes
#
# Revision 1.2  2009/01/21 13:19:42  bobfcsoft
# link to local libraries
#
# Revision 1.1  2009/01/19 20:15:09  bobfcsoft
# relocated files
#
#
###############################################

SIMPL_BIN=../../../bin

CC=cc
DE=NODEBUG

OS_TYPE=_LINUX_OS
ifeq (_CYGWIN, $(OS_TYPE))
DEAD_CHILD = ZOMBIE
else
DEAD_CHILD = NO_ZOMBIE
endif

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

SIMPL_SRC_DIR=$(SIMPL_DIR)/simplipc/src

LOCAL_INCL_DIR=../include
INCL_DIR=$(SIMPL_DIR)/include
SIMPLIPC_INCL_DIR=$(SIMPL_DIR)/simplipc/include
SIMPLUTILS_INCL_DIR=$(SIMPL_DIR)/simplUtils/include
FCLOGGER_INCL_DIR=$(SIMPL_DIR)/fclogger/include

TCL_SURROGATE_OBJ=\
	$(OBJ_DIR)/tclSurrogate.o\
	$(OBJ_DIR)/tclSurroInit.o\
	$(OBJ_DIR)/tclSurroFifo.o\
	$(OBJ_DIR)/tclSurroSocket.o

FCSOCKET_OBJ=\
	$(OBJ_DIR)/socketUtils.o

CFLAGS=\
	-D$(DEAD_CHILD)\
	-D$(DE)\
	-Wall\
	-I $(LOCAL_INCL_DIR)\
	-I $(SIMPLIPC_INCL_DIR)\
	-I $(SIMPLUTILS_INCL_DIR)\
	-I $(FCLOGGER_INCL_DIR)\
	-I $(INCL_DIR)

LDFLAGS=\
	-L$(SIMPL_DIR)/simplUtils/lib\
	-lsimplmisc\
	-L$(SIMPL_DIR)/fclogger/lib\
	-lsimpllog\
	-L$(SIMPL_DIR)/simplipc/lib\
	-lsimpl\
	-L$(LIB_DIR)\
	-lfcsocket

ARCHFLAGS =

#========================
# Default targets
#========================
all:\
     title \
     $(BIN_DIR)/tclSurrogate
	@echo tclSurrogate all

title:
	@echo tclSurrogate/src build starting

description:
	@(\
	echo;\
	echo "$(line1)";\
	echo "Compiling $(PROGRAM_NAME) -> $(OBJ_NAME)";\
	echo )

#========================
# Link
#========================
$(BIN_DIR)/tclSurrogate:	$(TCL_SURROGATE_OBJ) $(LIB_DIR)/libfcsocket.a
	$(CC) -o $(BIN_DIR)/tclSurrogate $(TCL_SURROGATE_OBJ) $(LDFLAGS)
	
$(LIB_DIR)/libfcsocket.a: $(FCSOCKET_OBJ)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	ar vrs $(LIB_DIR)/libfcsocket.a $?	

#===================
# Compiles
#===================
$(OBJ_DIR)/socketUtils.o : socketUtils.c
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/socketUtils.o socketUtils.c

$(OBJ_DIR)/tclSurrogate.o: tclSurrogate.c $(HEADERS)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/tclSurrogate.o tclSurrogate.c

$(OBJ_DIR)/tclSurroFifo.o: tclSurroFifo.c $(HEADERS)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/tclSurroFifo.o tclSurroFifo.c

$(OBJ_DIR)/tclSurroInit.o: tclSurroInit.c $(HEADERS)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/tclSurroInit.o tclSurroInit.c

$(OBJ_DIR)/tclSurroSocket.o: tclSurroSocket.c $(HEADERS)
	@make -s description PROGRAM_NAME="$<" OBJ_NAME="$@"
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/tclSurroSocket.o tclSurroSocket.c

#===================
# Cleanup
#===================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(LIB_DIR)/*.a
	@-rm -f $(BIN_DIR)/tclSurrogate
	@echo tclSurrogate clean

#===================
# Install
#===================
install:\
     title \
     $(BIN_DIR)/tclSurrogate
	cp $(BIN_DIR)/tclSurrogate $(SIMPL_BIN)/tclSurrogate
	@echo tclSurrogate install

#=====================================================================
#  uninstall 
#=====================================================================
uninstall: \
    title 
	-rm $(SIMPL_BIN)/tclSurrogate
	@echo tclSurrogate uninstall

#===================
#  i386
#===================
i386:
	make install ARCHFLAGS="-march=i386"
	@echo tclSurrogate i386

#===================
#  debug
#===================
debug:
	make install DE=-g
	@echo tclSurrogate debug
