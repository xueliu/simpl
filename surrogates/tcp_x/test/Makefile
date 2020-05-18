#=======================================================================
#
# FILE:			Makefile
#
# DESCRIPTION:	This make file produces some test programs.
#
# AUTHOR:		FC Software Inc.
#-----------------------------------------------------------------------
#    Copyright (C) 2002, 2008 FCSoftware Inc. 
#
#    This software is in the public domain.
#    Permission to use, copy, modify, and distribute this software and its
#    documentation for any purpose and without fee is hereby granted, 
#    without any conditions or restrictions.
#    This software is provided "as is" without express or implied warranty.
#
#    If you discover a bug or add an enhancement contact us on the: 
#
#	SIMPL project mailing list
#-----------------------------------------------------------------------
#
# Revision history:
# ======================================================================
# $Log: Makefile,v $
# Revision 1.8  2009/01/08 17:37:23  bobfcsoft
# more debian prep
#
# Revision 1.7  2008/12/02 15:09:17  bobfcsoft
# specify local libraries
#
# Revision 1.6  2008/12/01 18:55:16  bobfcsoft
# removed subdir creation targets
#
# Revision 1.5  2008/11/27 20:35:20  johnfcsoft
# clobber-->clean
#
# Revision 1.4  2008/11/27 02:10:22  bobfcsoft
# cleanup
#
# Revision 1.3  2008/11/26 17:31:09  bobfcsoft
# cleanup
#
# Revision 1.2  2005/05/26 14:18:05  bobfcsoft
# added sender2/receive2
#
# Revision 1.1.1.1  2005/03/27 11:51:03  paul_c
# Initial import
#
# Revision 1.3  2003/04/23 20:39:40  root
# added test0 and test1 targets
#
# Revision 1.2  2003/04/14 14:01:07  root
# enabled MAC_OS_X stuff
#
# Revision 1.1  2002/11/22 16:41:13  root
# Initial revision
#
#
#=======================================================================

#====================================================================
# definitions
#====================================================================
CC = cc
DE = DE

OBJ_DIR = ./obj
BIN_DIR = ../bin
LIB_DIR = ../../../simplipc/lib
MISCLIB_DIR = ../../../simplUtils/lib
INC_DIR = ../../../include
SIMPLIPC_INC_DIR=../../../simplipc/include
SIMPLUTILS_INC_DIR=../../../simplUtils/include

RECEIVER_OBJ =	\
			$(OBJ_DIR)/receiver.o

SENDER_OBJ =	\
			$(OBJ_DIR)/sender.o

# release flags
COMFLAGS = -O2 -pipe
#COMFLAGS = -mno-486 -O2 -pipe
# debug flags
#COMFLAGS = -g -pipe
# profile flags
#COMFLAGS = -g -pipe -a -pg

CFLAGS = $(COMFLAGS) -Wall -I$(SIMPLIPC_INC_DIR) -I$(SIMPLUTILS_INC_DIR) -I$(INC_DIR)
LDFLAGS = $(COMFLAGS) -L$(LIB_DIR) -lsimpl -L$(MISCLIB_DIR) -lsimplmisc

#=====================================================================
# Default target: all
#=====================================================================
all: install

install:\
    title \
    $(BIN_DIR)/receiver \
    $(BIN_DIR)/sender \
    $(BIN_DIR)/go \
    $(BIN_DIR)/stop
	@echo test all

title:
	@echo test starting

test0:
	ln -fs receiver0.c receiver.c
	ln -fs sender0.c sender.c
	make install

test1:
	ln -fs receiver1.c receiver.c
	ln -fs sender1.c sender.c
	make install

test2:
	ln -fs receiver2.c receiver.c
	ln -fs sender2.c sender.c
	make install

#=====================================================================
# linking
#=====================================================================
$(BIN_DIR)/receiver: $(RECEIVER_OBJ)
	$(CC) -o $@ $< $(LDFLAGS)

$(BIN_DIR)/sender: $(SENDER_OBJ)
	$(CC) -o $@ $< $(LDFLAGS)
	
$(BIN_DIR)/go: go
	cp go $(BIN_DIR)/go

$(BIN_DIR)/stop: stop
	cp stop $(BIN_DIR)/stop

#=====================================================================
# compiling
#=====================================================================
$(OBJ_DIR)/receiver.o: receiver.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR)/sender.o: sender.c
	$(CC) -c $(CFLAGS) -o $@ $<

#=====================================================================
#  cleanup
#=====================================================================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/receiver
	@-rm -f $(BIN_DIR)/sender
	@-rm -f $(BIN_DIR)/go
	@-rm -f $(BIN_DIR)/stop
	@echo test clean
