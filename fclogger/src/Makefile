###############################################
#
# Makefile - for fclogger 
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
#-----------------------------------------------------------------------
#
#$Log: Makefile,v $
#Revision 1.11  2011/03/23 16:51:25  bobfcsoft
#removed space in -L option for Mac compatibility
#
#Revision 1.10  2010/02/19 16:46:57  bobfcsoft
#i386 changes
#
#Revision 1.9  2009/04/14 20:51:49  bobfcsoft
#added link in i386 section
#
#Revision 1.8  2009/04/13 17:32:47  bobfcsoft
#added i386 target
#
#Revision 1.7  2009/01/08 17:20:15  bobfcsoft
#more debian prep
#
#Revision 1.6  2008/12/02 17:00:39  bobfcsoft
#use local libraries
#
#Revision 1.5  2008/12/01 18:47:48  bobfcsoft
#removed subdir creation targets
#
#Revision 1.4  2008/11/27 20:39:04  johnfcsoft
#clobber-->clean
#
#Revision 1.3  2008/11/26 17:24:28  bobfcsoft
#cleanup
#
#Revision 1.2  2007/07/24 20:40:47  bobfcsoft
#new contact info
#
#Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
#Initial import
#
#Revision 1.11  2003/04/16 01:49:19  root
#added dynamic target
#
#Revision 1.10  2002/11/22 16:26:01  root
#2.0rc3
#
#Revision 1.9  2002/06/12 18:19:52  root
#fcipc merge completed
#
#Revision 1.8  2001/07/05 14:24:52  root
#added debug target
#
#Revision 1.7  2001/01/29 15:16:42  root
#added stuff for simpllog library
#
#Revision 1.6  2000/10/13 14:00:18  root
#LGPL
#using SIMPL_HOME
#
#Revision 1.5  1999/05/19 16:54:15  root
#enabled full warnings
#
#Revision 1.4  1999/04/08 14:26:28  root
#added fclog stuff
#
#Revision 1.3  1999/02/04 14:40:33  root
#cleanup
#
#Revision 1.2  1999/01/31 12:53:26  root
#*** empty log message ***
#
#Revision 1.1  1998/07/09 17:30:42  root
#Initial revision
#
#
#
###############################################
CC=cc

SIMPL_BIN=../../bin
SIMPL_LIB=../../lib
SIMPL_INCL=../../include

SIMPL_DIR=../..

PROGRAM_NAME=
line1 = --------------------------------------------------------------------
line2 = ====================================================================

DEST_DIR=.

OBJ_DIR=obj
LIB_DIR=../lib
BIN_DIR=../bin

INSTALL_LIB_DIR=$(SIMPL_DIR)/lib
INSTALL_BIN_DIR=$(SIMPL_DIR)/bin

LOCAL_SIMPLLIB_DIR=../../simplipc/lib
LOCAL_INCL_DIR=../include
INCL_DIR=$(SIMPL_DIR)/include
SIMPLIPC_INCL_DIR=../../simplipc/include

LOGGER_OBJ=\
	$(OBJ_DIR)/fclogger.o

SIMPLLOG_OBJ=\
	$(OBJ_DIR)/simplLogUtils.o

MY_LIBRARY = libsimpllog.a

CFLAGS= \
	-Wall\
        -I $(LOCAL_INCL_DIR)\
        -I $(INCL_DIR)\
        -I $(SIMPLIPC_INCL_DIR)

LDFLAGS=-L$(LOCAL_SIMPLLIB_DIR) -lsimpl 
LIBFLAGS=
ARCHFLAGS=

SONAME = libsimpllog.so.1
VERSION = 1.0.0

#========================
# Default targets
#========================
all:\
     title \
     $(LIB_DIR)/$(MY_LIBRARY) \
     $(BIN_DIR)/fclogger
	@echo fclogger all

title:
	@echo fclogger build starting

description:
	@(\
	echo;\
	echo "$(line1)";\
	echo "Compiling $(PROGRAM_NAME) module -> $<";\
	echo )

#========================
# Link
#========================
$(BIN_DIR)/fclogger:	$(LOGGER_OBJ)
	$(CC) -o $(BIN_DIR)/fclogger $(LOGGER_OBJ) $(LDFLAGS)
	
#=====================================================================
# static library
#=====================================================================
$(LIB_DIR)/libsimpllog.a: $(SIMPLLOG_OBJ)
	ar vrs $(LIB_DIR)/libsimpllog.a $?	
			
$(LIB_DIR)/i386/libsimpllog.a: $(SIMPLLOG_OBJ)
	ar vrs $(LIB_DIR)/i386/libsimpllog.a $?	
	ln -s $(LIB_DIR)/i386/libsimpllog.a $(LIB_DIR)/libsimpllog.a
			
#=====================================================================
# dynamic library
#=====================================================================
$(LIB_DIR)/libsimpllog.so: $(LIB_DIR)/$(SONAME)
	ln -sf $(LIB_DIR)/$(SONAME) $(LIB_DIR)/libsimpllog.so

$(LIB_DIR)/$(SONAME): $(LIB_DIR)/libsimpllog.so.$(VERSION)
	ln -sf $(LIB_DIR)/libsimpllog.so.$(VERSION) $(LIB_DIR)/$(SONAME)	

$(LIB_DIR)/libsimpllog.so.$(VERSION): $(SIMPLLOG_OBJ)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $? -lc

$(LIB_DIR)/i386/libsimpllog.so: $(SIMPLLOG_OBJ)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $? -lc
	ln -s $(LIB_DIR)/i386/libsimpllog.so $(LIB_DIR)/libsimpllog.so

#===================
# Compiles
#===================
$(OBJ_DIR)/fclogger.o: fclogger.c
	$(CC) -c $(CFLAGS) $(ARCHFLAGS) -o $(OBJ_DIR)/fclogger.o fclogger.c

$(OBJ_DIR)/simplLogUtils.o: simplLogUtils.c
	$(CC) -c $(LIBFLAGS) $(CFLAGS) $(ARCHFLAGS) -o $(OBJ_DIR)/simplLogUtils.o simplLogUtils.c

#===================
# Cleanup
#===================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/fclogger
	@-rm -f $(LIB_DIR)/libsimpllog.*
	@-rm -f $(INSTALL_LIB_DIR)/libsimpllog.*
	@echo fclogger clean

#===================
# Install
#===================
install:\
     title \
     $(LIB_DIR)/$(MY_LIBRARY) \
     $(BIN_DIR)/fclogger
	cp $(BIN_DIR)/fclogger $(SIMPL_BIN)/fclogger
	cp $(LIB_DIR)/$(MY_LIBRARY) $(SIMPL_LIB)
	cp $(LOCAL_INCL_DIR)/loggerVars.h $(SIMPL_INCL)
	cp $(LOCAL_INCL_DIR)/loggerProto.h $(SIMPL_INCL)
	@echo logger install

#===================
# uninstall
#===================
uninstall:\
     title 
	rm $(SIMPL_BIN)/fclogger
	rm $(SIMPL_LIB)/libsimpllog.*
	rm $(SIMPL_INCL)/loggerVars.h
	rm $(SIMPL_INCL)/loggerProto.h
	@echo logger uninstall

#=====================================================================
#   dynamic
#=====================================================================
dynamic:
	make install MY_LIBRARY=libsimpllog.so LIBFLAGS="-D_REENTRANT -fPIC"
	@echo logger dynamic

#===================
#  i386
#===================
i386:
	make install MY_LIBRARY="i386/libsimpllog.a" ARCHFLAGS="-march=i386"
	@echo logger i386

#===================
#  i386
#===================
i386_dynamic:
	make install MY_LIBRARY="i386/libsimpllog.so" LIBFLAGS="-D_REENTRANT -fPIC" ARCHFLAGS="-march=i386"
	@echo logger i386

#===================
#  debug
#===================
debug:
	make install DE=-g
	@echo logger debug
