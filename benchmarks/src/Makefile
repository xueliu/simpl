###############################################
#
# Makefile - for benchmarks 
#
#-----------------------------------------------------------------------
#    Copyright (C) 1998, 2002, 2008 FCSoftware Inc. 
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
#-----------------------------------------------------------------------
#
#$Log: Makefile,v $
#Revision 1.10  2011/03/23 16:46:32  bobfcsoft
#removed space in -L option for Mac compatibility
#
#Revision 1.9  2009/04/13 17:30:47  bobfcsoft
#added i386 target
#
#Revision 1.8  2009/01/08 17:19:19  bobfcsoft
#more debian prep
#
#Revision 1.7  2008/12/04 14:52:43  bobfcsoft
#removed dependency on SIMPL_HOME
#
#Revision 1.6  2008/12/02 17:01:13  bobfcsoft
#use local libraries
#
#Revision 1.5  2008/12/01 18:50:15  bobfcsoft
#removed subdir creation targets
#
#Revision 1.4  2008/11/27 02:06:03  bobfcsoft
#cleanup
#
#Revision 1.3  2008/11/26 17:23:37  bobfcsoft
#cleanup
#
#Revision 1.2  2007/07/24 20:08:45  bobfcsoft
#new comment info
#
#Revision 1.1.1.1  2005/03/27 11:50:37  paul_c
#Initial import
#
#Revision 1.11  2002/12/02 15:58:47  root
#switched to public licence
#
#Revision 1.10  2002/07/11 19:52:21  root
#SIMPL to SIMPLIPC
#
#Revision 1.9  2002/06/12 18:16:57  root
#make simplipc default case
#
#Revision 1.8  2002/05/29 15:27:31  root
#bug fixes
#
#Revision 1.7  2001/07/05 14:40:30  root
#added _SIMPLIPC case
#
#Revision 1.6  2001/01/29 15:11:06  root
#using Wall compiler option
#
#Revision 1.5  2000/10/13 13:50:20  root
#LGPL
#using SIMPL_HOME
#
#Revision 1.4  2000/04/17 14:56:26  root
#enhanced for SRR, SIPC and QNX
#
#Revision 1.3  1999/08/05 13:25:13  root
#*** empty log message ***
#
#Revision 1.2  1999/03/29 16:07:30  root
#bug fixes
#
#Revision 1.1  1999/02/04 15:41:15  root
#Initial revision
#
#
#
###############################################
CC=cc
OS=_THISIS_LINUX
VER=_SIMPLIPC

PROGRAM_NAME=
line1 = --------------------------------------------------------------------
line2 = ====================================================================

SIMPL_DIR=../..
OBJ_DIR=obj
BIN_DIR=../bin

SIMPLLIB_DIR=../../simplipc/lib
MISCLIB_DIR=../../simplUtils/lib
LOCAL_INCL_DIR=../include
INCL_DIR=$(SIMPL_DIR)/include
SIMPLIPC_INCL_DIR=../../simplipc/include
SIMPLUTILS_INCL_DIR=../../simplUtils/include
DEST_BIN_DIR=$(SIMPL_DIR)/bin

BOB_OBJ=\
	$(OBJ_DIR)/bob.o

SENDER_OBJ=\
	$(OBJ_DIR)/sender.o

RECEIVER_OBJ=\
	$(OBJ_DIR)/receiver.o

CFLAGS= -Wall \
        -I $(LOCAL_INCL_DIR)\
        -I $(SIMPLIPC_INCL_DIR)\
        -I $(SIMPLUTILS_INCL_DIR)\
        -I $(INCL_DIR)\
	-D$(OS)\
	-D$(VER)

LDFLAGS=\
	-L$(SIMPLLIB_DIR)\
	-lsimpl\
	-L$(MISCLIB_DIR)\
	-lsimplmisc\
	-lrt

ARCHFLAGS =

#========================
# Default targets
#========================
all:\
     title \
     whichOS \
     $(BIN_DIR)/sender \
     $(BIN_DIR)/receiver
	@echo benchmark all

whichOS:
	@echo whichOS = $(OS)

title:
	@echo benchmark build starting

description:
	@(\
	echo;\
	echo "$(line1)";\
	echo "Compiling $(PROGRAM_NAME) module -> $<";\
	echo )

#========================
# Link
#========================
$(BIN_DIR)/sender:	$(SENDER_OBJ)
	$(CC) -o $(BIN_DIR)/sender $(SENDER_OBJ) $(LDFLAGS)
	
$(BIN_DIR)/receiver:	$(RECEIVER_OBJ)
	$(CC) -o $(BIN_DIR)/receiver $(RECEIVER_OBJ) $(LDFLAGS)
	
#===================
# Compiles
#===================
$(OBJ_DIR)/receiver.o: receiver.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/receiver.o receiver.c

$(OBJ_DIR)/sender.o: sender.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $(OBJ_DIR)/sender.o sender.c

#===================
#  clobber 
#===================
clobber: clean
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/sender
	@-rm -f $(BIN_DIR)/receiver
	@echo benchmark clean

#===================
#  install 
#===================
install:\
     title \
     whichOS \
     $(BIN_DIR)/sender \
     $(BIN_DIR)/receiver
	@echo benchmark install

#===================
#  i386
#===================
i386:
	make install ARCHFLAGS="-march=i386"
	@echo benchmark i386

