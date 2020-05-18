###############################################
#
# Makefile - for testBinhex
#
#-----------------------------------------------------------------------
#    Copyright (C) 1999, 2002, 2008 FCSoftware Inc. 
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
#$Log: Makefile,v $
#Revision 1.8  2011/03/23 16:59:25  bobfcsoft
#removed space in -L option for Mac compatibility
#
#Revision 1.7  2009/04/14 20:53:54  bobfcsoft
#added i386 stuff
#
#Revision 1.6  2009/01/08 17:27:17  bobfcsoft
#more debian prep
#
#Revision 1.5  2008/12/02 15:04:04  bobfcsoft
#use local libraries
#
#Revision 1.4  2008/12/02 14:14:28  bobfcsoft
#removed subdir creation
#
#Revision 1.3  2008/11/27 20:34:18  johnfcsoft
#clobber-->clean
#
#Revision 1.2  2008/11/26 17:26:44  bobfcsoft
#cleanup
#
#Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
#Initial import
#
#Revision 1.2  2003/04/16 00:03:29  root
#added install target
#
#Revision 1.1  2003/04/14 13:32:01  root
#Initial revision
#
#
###############################################

CC=cc
DE=

PROGRAM_NAME=
line1 = --------------------------------------------------------------------
line2 = ====================================================================

OBJ_DIR=./obj
BIN_DIR=../bin
INCL_DIR=../../include
SIMPLUTILS_INCL_DIR=../../simplUtils/include
DEST_BIN_DIR=../../bin

TEST_BIN_STR_OBJ = \
	$(OBJ_DIR)/testbinstr.o

CFLAGS=\
	$(DE) \
	-Wall \
	-I $(INCL_DIR)\
	-I $(SIMPLUTILS_INCL_DIR)

LDFLAGS=\
	-L../lib\
	-lsimplmisc

ARCHFLAGS =

#========================
# Default targets
#========================
all: \
     title \
     $(BIN_DIR)/testbinstr
	@echo testbinstr all

title:
	@echo test bin hexc build starting

description:
	@(\
	echo;\
	echo "$(line1)";\
	echo "Compiling $(PROGRAM_NAME) module -> $<";\
	echo )

#========================
# Link
#========================
$(BIN_DIR)/testbinstr:	$(TEST_BIN_STR_OBJ)
	$(CC) -o $@ $? $(LDFLAGS)
	
#===================
# Compiles
#===================
$(OBJ_DIR)/testbinstr.o:	testbinstr.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

#===================
#  clean
#===================

clean:
	@-rm -f $(OBJ_DIR)/testbinstr.o
	@-rm -f $(BIN_DIR)/testbinstr
	@echo testbinstr clean

install: \
     title \
     $(BIN_DIR)/testbinstr
	@echo testbinstr install

#===================
#  i386
#===================
i386:
	make install ARCHFLAGS="-march=i386"
	@echo simplUtils/test i386

#===================
#  debug
#===================
debug:
	make all DE=-g
	@echo testbinstr debug
