#=======================================================================
#
# FILE:			Makefile
#
# DESCRIPTION:	This make file produces the simpl library.
#
# AUTHOR:		FC Software Inc.
#-----------------------------------------------------------------------
#    Copyright (C) 2000,2007,2008 FCSoftware Inc. 
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#    If you discover a bug or add an enhancement contact us on the
#    SIMPL project mailing list.
#
#-----------------------------------------------------------------------
#
# Revision history:
# ======================================================================
# $Log: Makefile,v $
# Revision 1.14  2012/10/30 19:49:11  bobfcsoft
# pass in FLAVOR define
#
# Revision 1.13  2009/08/19 14:39:02  bobfcsoft
# cleanup in i386 area
#
# Revision 1.12  2009/04/14 20:55:48  bobfcsoft
# added link in i386 section
#
# Revision 1.11  2009/04/13 17:28:21  bobfcsoft
# added i386 target
#
# Revision 1.10  2009/01/08 22:10:07  bobfcsoft
# try again to fix dynamic error
#
# Revision 1.9  2009/01/08 22:06:55  bobfcsoft
# fixed dynamic target error
#
# Revision 1.8  2009/01/08 17:28:02  bobfcsoft
# more debian prep
#
# Revision 1.7  2008/12/01 18:45:29  bobfcsoft
# removed subdir creation targets
#
# Revision 1.6  2008/11/27 20:33:14  johnfcsoft
# clobber-->clean
#
# Revision 1.5  2008/11/27 02:07:31  bobfcsoft
# cleanup
#
# Revision 1.4  2008/11/26 17:27:36  bobfcsoft
# cleanup
#
# Revision 1.3  2007/09/12 16:33:20  bobfcsoft
# added fPIC flag for 64bit compile
#
# Revision 1.2  2007/07/24 19:57:22  bobfcsoft
# new contact info
#
# Revision 1.1.1.1  2005/03/27 11:50:50  paul_c
# Initial import
#
# Revision 1.11  2003/04/14 13:38:06  root
# enabled MAC_OS_X stuff
#
# Revision 1.10  2002/11/22 16:34:55  root
# 2.0rc3
#
#
#=======================================================================

#====================================================================
# definitions
#====================================================================

# default OS type
OS_TYPE = _LINUX_OS
# so far this value can also be set OS_TYPE = _MAC_OS_X

# allow experimental ifdefs
FLAVOR = _STABLE
# can also be set to _EXPMTL

SIMPL_BIN=../../bin
SIMPL_LIB=../../lib
SIMPL_INCL=../../include

CC = cc

OBJ_DIR = ./obj
LIB_DIR = ../lib
LOCAL_INC_DIR = ../include
INC_DIR = ../../include

SIMPL_OBJ =	\
			$(OBJ_DIR)/simpl.o \
			$(OBJ_DIR)/simplUtils.o

MY_LIBRARY = libsimpl.a

CFLAGS = \
	-D$(OS_TYPE)\
	-D$(FLAVOR)\
	-fPIC\
	-O\
	-Wall\
	-I$(LOCAL_INC_DIR)\
	-I$(INC_DIR)
LDFLAGS = -L$(LIB_DIR) -lsimpl
LIBFLAGS =

SONAME = libsimpl.so.1
VERSION = 1.0.0

#=====================================================================
# Default targets
#=====================================================================
all: \
    title \
    whichOS \
    $(LIB_DIR)/$(MY_LIBRARY)
	@echo simpl all

title:
	@echo simpl starting

whichOS:
	@echo OS TYPE = $(OS_TYPE)

#=====================================================================
# static library
#=====================================================================
$(LIB_DIR)/libsimpl.a: $(SIMPL_OBJ)
	ar vrs $(LIB_DIR)/libsimpl.a $?	

$(LIB_DIR)/i386/libsimpl.a: $(SIMPL_OBJ)
	ar vrs $(LIB_DIR)/i386/libsimpl.a $?	
	ln -s $(LIB_DIR)/i386/libsimpl.a $(LIB_DIR)/libsimpl.a

#=====================================================================
# dynamic library
#=====================================================================
$(LIB_DIR)/libsimpl.so: $(LIB_DIR)/$(SONAME)
	ln -sf $(LIB_DIR)/$(SONAME)	$(LIB_DIR)/libsimpl.so

$(LIB_DIR)/$(SONAME): $(LIB_DIR)/libsimpl.so.$(VERSION)
	ln -sf $(LIB_DIR)/libsimpl.so.$(VERSION) $(LIB_DIR)/$(SONAME)	

$(LIB_DIR)/libsimpl.so.$(VERSION): $(SIMPL_OBJ)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $? -lc

$(LIB_DIR)/i386/libsimpl.so: $(SIMPL_OBJ)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $? -lc
	ln -s $(LIB_DIR)/i386/libsimpl.so $(LIB_DIR)/libsimpl.so

#=====================================================================
# compiling
#=====================================================================
$(OBJ_DIR)/simpl.o: simpl.c
	$(CC) -c $(LIBFLAGS) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/simplUtils.o: simplUtils.c
	$(CC) -c $(LIBFLAGS) $(CFLAGS) -o $@ $<

#=====================================================================
#  cleanup
#=====================================================================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(LIB_DIR)/libsimpl.*
	@-rm -f $(INSTALL_LIB_DIR)/libsimpl.*
	@echo simpl clean

#=====================================================================
#  install 
#=====================================================================
install: \
     title \
     $(LIB_DIR)/$(MY_LIBRARY)
	-cp $(LIB_DIR)/$(MY_LIBRARY) $(SIMPL_LIB)
	-cp $(LOCAL_INC_DIR)/simpl.h $(SIMPL_INCL)
	-cp $(LOCAL_INC_DIR)/simplProto.h $(SIMPL_INCL)
	-cp $(LOCAL_INC_DIR)/simplDefs.h $(SIMPL_INCL)
	@echo simpl install

#=====================================================================
#  uninstall 
#=====================================================================
uninstall: \
     title 
	rm $(SIMPL_LIB)/libsimpl.*
	rm $(SIMPL_INCL)/simpl.h
	rm $(SIMPL_INCL)/simplProto.h
	rm $(SIMPL_INCL)/simplDefs.h
	@echo simpl install

#=====================================================================
#   dynamic
#=====================================================================
dynamic:
	make install MY_LIBRARY=libsimpl.so LIBFLAGS="-D_REENTRANT"
	@echo simpl dynamic

#===================
#  i386
#===================
i386:
	make install MY_LIBRARY="i386/libsimpl.a" LIBFLAGS="-march=i386"
	@echo simpl i386

#===================
#  i386
#===================
i386_dynamic:
	make install MY_LIBRARY="i386/libsimpl.so" LIBFLAGS="-D_REENTRANT -march=i386"
	@echo simpl dynamic i386

#===================
#  debug
#===================
debug:
	make install DE=-g
	@echo simpl debug
