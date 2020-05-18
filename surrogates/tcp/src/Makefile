#=======================================================================
#
# FILE:			Makefile
#
# DESCRIPTION:	This make file produces the tcp surrogate program.
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
#    If you discover a bug or add an enhancement contact us on the: 
#
#	SIMPL project mailing list
#-----------------------------------------------------------------------
#
# Revision history:
# ======================================================================
# $Log: Makefile,v $
# Revision 1.13  2011/09/08 17:01:00  bobfcsoft
# added CYGWIN logic
#
# Revision 1.12  2011/03/23 18:12:13  bobfcsoft
# changed to case insensitive file names for Mac compatibility
#
# Revision 1.11  2010/01/29 20:41:22  johnfcsoft
# added DEAD CHILD
#
# Revision 1.10  2009/04/13 17:38:38  bobfcsoft
# added i386 target
#
# Revision 1.9  2009/01/08 17:37:12  bobfcsoft
# more debian prep
#
# Revision 1.8  2008/12/02 15:07:55  bobfcsoft
# specify local libraries
#
# Revision 1.7  2008/12/01 18:54:25  bobfcsoft
# removed subdir creation targets
#
# Revision 1.6  2008/11/27 20:35:03  johnfcsoft
# clobber-->clean
#
# Revision 1.5  2008/11/27 02:10:13  bobfcsoft
# cleanup
#
# Revision 1.4  2008/11/26 17:30:56  bobfcsoft
# cleanup
#
# Revision 1.3  2008/04/21 13:42:28  johnfcsoft
# SUR_CHR set as default
#
# Revision 1.2  2007/01/09 15:17:39  bobfcsoft
# restoring from tcp_x
#
# Revision 1.5  2006/01/10 15:32:22  bobfcsoft
# v3.0 changes
#
# Revision 1.4  2005/12/20 15:26:18  bobfcsoft
# more 3.0 prerelease code
#
# Revision 1.3  2005/11/24 21:46:58  bobfcsoft
# 2nd 3.0 prerelease
#
# Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
# Initial import
#
# Revision 1.2  2003/04/14 14:07:17  root
# enabled MAC_OS_X stuff
#
#
#=======================================================================

#====================================================================
# definitions
#====================================================================

#MSG_TYPE = SUR_BIN
MSG_TYPE = SUR_CHR

MSG_BUFFERING = NOT_BUFFERED
#MSG_BUFFERING = BUFFERED

OS_TYPE=_LINUX_OS
ifeq (_CYGWIN, $(OS_TYPE))
DEAD_CHILD = ZOMBIE
else
DEAD_CHILD = NO_ZOMBIE
endif

SIMPL_BIN=../../../bin

CC = cc

OBJ_DIR = ./obj
BIN_DIR = ../bin
LIB_DIR = ../../../simplipc/lib
MISCLIB_DIR = ../../../simplUtils/lib
INC_DIR = ../../../include
LOCAL_INC_DIR = ../include
SIMPLIPC_INC_DIR = ../../../simplipc/include
SIMPLUTILS_INC_DIR = ../../../simplUtils/include

SURROGATE_OBJ =	\
	$(OBJ_DIR)/surrogate.o \
	$(OBJ_DIR)/surrogateInit.o \
	$(OBJ_DIR)/surrogate_R.o \
	$(OBJ_DIR)/surrogate_rr.o \
	$(OBJ_DIR)/surrogate_S.o \
	$(OBJ_DIR)/surrogate_ss.o \
	$(OBJ_DIR)/surrogateUtils.o

CFLAGS = \
	-D$(MSG_BUFFERING) \
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
    $(BIN_DIR)/surrogate
	@echo surrogate all

title:
	@echo surrogate starting

#=====================================================================
# linking
#=====================================================================
$(BIN_DIR)/surrogate: $(SURROGATE_OBJ)
	$(CC) -o $@ $(SURROGATE_OBJ) $(LDFLAGS)
	
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

#=====================================================================
#  cleanup
#=====================================================================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/surrogate
	@echo surrogate clean

#=====================================================================
#  install 
#=====================================================================
install: \
    title \
    $(BIN_DIR)/surrogate
	-cp $(BIN_DIR)/surrogate $(SIMPL_BIN)/surrogateTcp
	@echo surrogate install

#=====================================================================
#  uninstall 
#=====================================================================
uninstall: \
    title 
	-rm $(SIMPL_BIN)/surrogateTcp
	@echo surrogate uninstall

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
