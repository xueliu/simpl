#=======================================================================
#
# FILE:			Makefile
#
# DESCRIPTION:	This make file produces the RS232 surrogate program.
#
# AUTHOR:		FC Software Inc.
#-----------------------------------------------------------------------
#    Copyright (C) 2005, 2008 FCSoftware Inc. 
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
# Revision history:
# $Log: Makefile,v $
# Revision 1.11  2011/09/08 16:57:59  bobfcsoft
# added CYGWIN logic
#
# Revision 1.10  2011/03/23 17:47:44  bobfcsoft
# changed to case insensitive file names for Mac compatibility
#
# Revision 1.9  2010/01/29 20:39:37  johnfcsoft
# added DEAD CHILD
#
# Revision 1.8  2009/04/13 17:37:34  bobfcsoft
# added i386 target
#
# Revision 1.7  2009/01/08 17:36:30  bobfcsoft
# more debian prep
#
# Revision 1.6  2008/12/02 15:10:34  bobfcsoft
# specify local libraries
#
# Revision 1.5  2008/12/01 18:53:19  bobfcsoft
# removed subdir creation targets
#
# Revision 1.4  2008/11/27 20:35:46  johnfcsoft
# clobber-->clean
#
# Revision 1.3  2008/11/27 02:09:46  bobfcsoft
# cleanup
#
# Revision 1.2  2008/11/26 17:30:20  bobfcsoft
# cleanup
#
# Revision 1.1  2008/04/21 17:17:01  johnfcsoft
# initial
#
#
#=======================================================================

#====================================================================
# definitions
#====================================================================

#MSG_TYPE = SUR_BIN
MSG_TYPE = SUR_CHR

OS_TYPE=_LINUX_OS
ifeq (_CYGWIN, $(OS_TYPE))
DEAD_CHILD = ZOMBIE
else
DEAD_CHILD = NO_ZOMBIE
endif

CC = cc

OBJ_DIR = ./obj
BIN_DIR = ../bin
LIB_DIR = ../../../simplipc/lib
MISCLIB_DIR = ../../../simplUtils/lib
INC_DIR = ../../../include
LOCAL_INC_DIR = ../include
SIMPLIPC_INC_DIR = ../../../simplipc/include
SIMPLUTILS_INC_DIR = ../../../simplUtils/include
INSTALL_BIN_DIR=../../../bin

SURROGATE_OBJ =	\
	$(OBJ_DIR)/surrogate.o	\
	$(OBJ_DIR)/surrogateInit.o \
	$(OBJ_DIR)/surrogate_R.o \
	$(OBJ_DIR)/surrogate_S.o \
	$(OBJ_DIR)/surrogate_rr.o \
	$(OBJ_DIR)/surrogate_ss.o \
	$(OBJ_DIR)/surrogateUtils.o

RS232_OBJ = \
	$(OBJ_DIR)/rs232_rw.o

CFLAGS = \
	-D$(MSG_TYPE) \
	-D$(DEAD_CHILD) \
	-O \
	-Wall \
	-I$(SIMPLIPC_INC_DIR) \
	-I$(SIMPLUTILS_INC_DIR) \
	-I$(INC_DIR) \
	-I$(LOCAL_INC_DIR)
LDFLAGS = -L$(LIB_DIR) -lsimpl -L$(MISCLIB_DIR) -lsimplmisc
ARCHFLAGS =

#=====================================================================
# Default targets
#=====================================================================
all: \
    title \
    $(BIN_DIR)/surrogate \
	$(BIN_DIR)/rs232_rw
	@echo surrogate all

title:
	@echo surrogate starting

#=====================================================================
# linking
#=====================================================================
$(BIN_DIR)/surrogate: $(SURROGATE_OBJ)
	$(CC) -o $@ $(SURROGATE_OBJ) $(LDFLAGS)

$(BIN_DIR)/rs232_rw: $(RS232_OBJ)
	$(CC) -o $@ $(RS232_OBJ) $(LDFLAGS)
	
#=====================================================================
# compiling
#=====================================================================
$(OBJ_DIR)/surrogate.o: surrogate.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/surrogateInit.o: surrogateInit.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/surrogate_R.o: surrogate_R.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/surrogate_rr.o: surrogate_rr.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/surrogate_S.o: surrogate_S.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/surrogate_ss.o: surrogate_ss.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/surrogateUtils.o: surrogateUtils.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/rs232_rw.o: rs232_rw.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

#=====================================================================
#  cleanup
#=====================================================================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/surrogate
	@-rm -f $(BIN_DIR)/rs232_rw
	@echo surrogate clean

#=====================================================================
#  install 
#=====================================================================
install: \
    title \
    $(BIN_DIR)/surrogate \
	$(BIN_DIR)/rs232_rw
	-cp $(BIN_DIR)/surrogate $(INSTALL_BIN_DIR)/surrogateRS232
	-cp $(BIN_DIR)/rs232_rw $(INSTALL_BIN_DIR)/rs232_rw
	@echo surrogate install

#===================
#  i386
#===================
i386:
	make install ARCHFLAGS="-march=i386"
	@echo surrogate i386

#===================
#  debug
#===================
debug:
	make install DE=-g
	@echo surrogate debug
