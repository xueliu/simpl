###############################################
#
# Makefile - for simplUtils 
#
#-----------------------------------------------------------------------
#    Copyright (C) 2007,2008 SIMPL project. 
#
#    This software is in the public domain.
#    Permission to use, copy, modify, and distribute this software and its
#    documentation for any purpose and without fee is hereby granted, 
#    without any conditions or restrictions.
#    This software is provided "as is" without express or implied warranty.
#
#    If you discover a bug or add an enhancement contact us on the SIMPL
#    project mailing list.
#
#-----------------------------------------------------------------------
#
#$Log: Makefile,v $
#Revision 1.12  2011/03/23 16:58:35  bobfcsoft
#removed space in -L option for Mac compatibility
#
#Revision 1.11  2009/04/14 20:53:10  bobfcsoft
#added link in i386 section
#
#Revision 1.10  2009/04/14 19:40:47  bobfcsoft
#added i386_dynamic target
#
#Revision 1.9  2009/04/13 17:34:26  bobfcsoft
#added i386 target
#
#Revision 1.8  2009/01/08 17:26:57  bobfcsoft
#more debian prep
#
#Revision 1.7  2008/12/02 15:02:57  bobfcsoft
#use local libraries
#
#Revision 1.6  2008/12/02 14:13:11  bobfcsoft
#removed subdir creation
#
#Revision 1.5  2008/11/27 20:34:03  johnfcsoft
#clobber-->clean
#
#Revision 1.4  2008/11/26 17:26:26  bobfcsoft
#cleanup
#
#Revision 1.3  2007/09/12 16:35:34  bobfcsoft
#added fPIC flag for 64bit compile
#
#Revision 1.2  2007/07/03 18:28:04  bobfcsoft
#added sandboxRelay
#
#Revision 1.1.1.1  2005/03/27 11:50:55  paul_c
#Initial import
#
#Revision 1.13  2003/04/23 20:46:17  root
#dynamic library fixes
#
#Revision 1.12  2003/04/16 16:31:50  root
#fixed typo
#
#Revision 1.11  2003/04/16 01:58:54  root
#added dynamic target
#
#Revision 1.10  2003/04/14 13:29:51  root
#added MACOSX stuff
#
#Revision 1.9  2002/12/02 16:04:40  root
#export simplmisc
#
#Revision 1.8  2002/11/22 16:37:07  root
#2.0rc3
#
#
###############################################

SIMPL_BIN=../../bin
SIMPL_INCL=../../include


# default OS type
OS_TYPE = _LINUX_OS
# so far this value can also be set OS_TYPE = _MAC_OS_X

CC=cc
DE=

PROGRAM_NAME=
line1 = --------------------------------------------------------------------
line2 = ====================================================================

OBJ_DIR=./obj
BIN_DIR=../bin
LIB_DIR=../lib

LOCAL_SIMPLLIB_DIR=../../simplipc/lib
LOCAL_SIMPLLOG_DIR=../../fclogger/lib
LOCAL_INCL_DIR=../include
INCL_DIR=../../include
SIMPLIPC_INCL_DIR=../../simplipc/include
SIMPLUTILS_INCL_DIR=../../simplUtils/include
FCLOGGER_INCL_DIR=../../fclogger/include
DEST_BIN_DIR=../../bin
DEST_LIB_DIR=../../lib

MISC_OBJ= \
	$(OBJ_DIR)/miscUtils.o\
	$(OBJ_DIR)/binstr.o
			
DUMP_PROTOCOL_TABLE_OBJ = \
	$(OBJ_DIR)/dumpProtocolTable.o

FCSLAY_OBJ = \
	$(OBJ_DIR)/fcslay.o

FCSHOW_OBJ = \
	$(OBJ_DIR)/fcshow.o

SANDBOX_RELAY_OBJ =\
	$(OBJ_DIR)/sandboxRelay.o\
	$(OBJ_DIR)/sandboxRelayInit.o\
	$(OBJ_DIR)/sandboxRelayUtils.o

MY_LIBRARY = libsimplmisc.a

CFLAGS=\
	-D$(OS_TYPE) \
	$(DE) \
	-fPIC \
	-Wall\
	-I $(LOCAL_INCL_DIR)\
	-I $(SIMPLIPC_INCL_DIR)\
	-I $(SIMPLUTILS_INCL_DIR)\
	-I $(FCLOGGER_INCL_DIR)\
	-I $(INCL_DIR)

LDFLAGS=\
	-L$(LOCAL_SIMPLLIB_DIR)\
	-lsimpl

LDFLAGS2=\
	-L$(LOCAL_SIMPLLIB_DIR)\
	-lsimpl\
	-L$(LOCAL_SIMPLLOG_DIR)\
	-lsimpllog\
	-L../lib/\
	-lsimplmisc

LIBFLAGS =
ARCHFLAGS =


SONAME = libsimplmisc.so.1
VERSION = 1.0.0

#========================
# Default targets
#========================
all:\
     title \
     $(BIN_DIR)/sandboxRelay \
     $(LIB_DIR)/libsimplmisc.a \
     $(BIN_DIR)/dumpProtocolTable \
     $(BIN_DIR)/fcslay \
     $(BIN_DIR)/fcshow
	@echo simplUtils all

title:
	@echo simplUtils/src build starting

description:
	@(\
	echo;\
	echo "$(line1)";\
	echo "Compiling $(PROGRAM_NAME) module -> $<";\
	echo )

#========================
# Link
#========================
$(BIN_DIR)/sandboxRelay:	$(SANDBOX_RELAY_OBJ) $(LIB_DIR)/$(MY_LIBRARY)
	$(CC) -o $(BIN_DIR)/sandboxRelay $(SANDBOX_RELAY_OBJ) $(LDFLAGS2)
	
$(BIN_DIR)/dumpProtocolTable:	$(DUMP_PROTOCOL_TABLE_OBJ)
	$(CC) -o $@ $? $(LDFLAGS)
	
$(BIN_DIR)/fcslay:	$(FCSLAY_OBJ)
	$(CC) -o $@ $? $(LDFLAGS)
	
$(BIN_DIR)/fcshow:	$(FCSHOW_OBJ)
	$(CC) -o $@ $? $(LDFLAGS)
	
#=====================================================================
# static library
#=====================================================================
$(LIB_DIR)/libsimplmisc.a: $(MISC_OBJ)
	ar vrs $@ $?	

$(LIB_DIR)/i386/libsimplmisc.a: $(MISC_OBJ)
	ar vrs $(LIB_DIR)/i386/libsimplmisc.a $?	
	ln -s $(LIB_DIR)/i386/libsimplmisc.a $(LIB_DIR)/libsimplmisc.a

#=====================================================================
# dynamic library
#=====================================================================
$(LIB_DIR)/libsimplmisc.so: $(LIB_DIR)/$(SONAME)
	ln -sf $(LIB_DIR)/$(SONAME)	$(LIB_DIR)/libsimplmisc.so

$(LIB_DIR)/$(SONAME): $(LIB_DIR)/libsimplmisc.so.$(VERSION)
	ln -sf $(LIB_DIR)/libsimplmisc.so.$(VERSION) $(LIB_DIR)/$(SONAME)	

$(LIB_DIR)/libsimplmisc.so.$(VERSION): $(MISC_OBJ)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $? -lc

$(LIB_DIR)/i386/libsimplmisc.so: $(MISC_OBJ)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $? -lc
	ln -s $(LIB_DIR)/i386/libsimplmisc.so $(LIB_DIR)/libsimplmisc.so

#===================
# Compiles
#===================
$(OBJ_DIR)/miscUtils.o:	miscUtils.c
	$(CC) -c $(LIBFLAGS) $(ARCHFLAGS) $(CFLAGS) -o $@ $<
			
$(OBJ_DIR)/binstr.o:	binstr.c
	$(CC) -c $(LIBFLAGS) $(ARCHFLAGS) $(CFLAGS) -o $@ $<
			
$(OBJ_DIR)/dumpProtocolTable.o:	dumpProtocolTable.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/fcslay.o:	fcslay.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/fcshow.o:	fcshow.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/sandboxRelay.o:	sandboxRelay.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/sandboxRelayInit.o:	sandboxRelayInit.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/sandboxRelayUtils.o:	sandboxRelayUtils.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

#===================
#  clean
#===================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/sandboxRelay
	@-rm -f $(BIN_DIR)/dumpProtocolTable
	@-rm -f $(BIN_DIR)/fcslay
	@-rm -f $(BIN_DIR)/fcshow
	@-rm -f $(LIB_DIR)/libsimplmisc.*
	@echo simplUtils/src clean

#===================
#  install 
#===================
install:\
     title \
     $(BIN_DIR)/sandboxRelay \
     $(LIB_DIR)/$(MY_LIBRARY) \
     $(BIN_DIR)/dumpProtocolTable \
     $(BIN_DIR)/fcslay \
     $(BIN_DIR)/fcshow
	cp $(BIN_DIR)/sandboxRelay $(DEST_BIN_DIR)
	cp $(BIN_DIR)/dumpProtocolTable $(DEST_BIN_DIR)
	cp $(BIN_DIR)/fcslay $(SIMPL_BIN)
	cp $(BIN_DIR)/fcshow $(SIMPL_BIN)
	cp $(LIB_DIR)/$(MY_LIBRARY) $(DEST_LIB_DIR)
	cp $(LOCAL_INCL_DIR)/simplmiscProto.h $(SIMPL_INCL)
	@echo simplUtils/src install

#===================
#  uninstall 
#===================
uninstall:\
     title
	rm $(DEST_BIN_DIR)/sandboxRelay
	rm $(DEST_BIN_DIR)/dumpProtocolTable
	rm $(SIMPL_BIN)/fcslay
	rm $(SIMPL_BIN)/fcshow
	rm $(DEST_LIB_DIR)/libsimplmisc.*
	rm $(SIMPL_INCL)/simplmiscProto.h
	@echo simplUtils/src uninstall

#=====================================================================
#   dynamic
#=====================================================================
dynamic:
	make install MY_LIBRARY=libsimplmisc.so LIBFLAGS="-D_REENTRANT -fPIC"
	@echo simplmisc dynamic

#===================
#  i386
#===================
i386:
	make install MY_LIBRARY="i386/libsimplmisc.a" ARCHFLAGS="-march=i386"
	@echo simplmisc i386

#===================
#  i386
#===================
i386_dynamic:
	make install MY_LIBRARY="i386/libsimplmisc.so" LIBFLAGS="-D_REENTRANT -fPIC" ARCHFLAGS="-march=i386"
	@echo simplmisc i386

#===================
#  debug
#===================
debug:
	make install DE=-g
	@echo simplUtils/src debug
