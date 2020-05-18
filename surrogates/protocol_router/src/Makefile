#=======================================================================
#
# FILE:			Makefile
#
# DESCRIPTION:	This make file produces the protocolRouter program.
#
# AUTHOR:		FC Software Inc.
#-----------------------------------------------------------------------
#    Copyright (C) 2000, 2008 FCSoftware Inc. 
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
#    If you discover a bug or add an enhancement contact us on the: 
#
#	SIMPL project mailing list
#-----------------------------------------------------------------------
#
# Revision history:
# ======================================================================
# $Log: Makefile,v $
# Revision 1.8  2009/04/13 17:37:01  bobfcsoft
# added i386 target
#
# Revision 1.7  2009/01/08 17:35:45  bobfcsoft
# more debian prep
#
# Revision 1.6  2008/12/02 15:05:49  bobfcsoft
# specify local library
#
# Revision 1.5  2008/12/01 18:52:08  bobfcsoft
# removed subdir creation targets
#
# Revision 1.4  2008/11/27 20:38:02  johnfcsoft
# clobber-->clean
#
# Revision 1.3  2008/11/27 02:09:23  bobfcsoft
# cleanup
#
# Revision 1.2  2008/11/26 17:29:32  bobfcsoft
# cleanup
#
# Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
# Initial import
#
# Revision 1.1  2002/11/22 16:39:09  root
# Initial revision
#
#
#=======================================================================

SIMPL_BIN=../../../bin

#====================================================================
# definitions
#====================================================================
CC = cc
DE =

OBJ_DIR = ./obj
BIN_DIR = ../bin
LIB_DIR = ../../../simplipc/lib
INC_DIR = ../../../include
SIMPLIPC_INC_DIR=../../../simplipc/include

PROTOCOL_ROUTER_OBJ =	\
			$(OBJ_DIR)/protocolRouter.o

CFLAGS =  $(DE) -O -Wall -I$(SIMPLIPC_INC_DIR) -I$(INC_DIR)
LDFLAGS = -L$(LIB_DIR) -lsimpl
ARCHFLAGS =

#=====================================================================
# Default targets
#=====================================================================
all: \
    title \
    $(BIN_DIR)/protocolRouter 
	@echo protocolRouter all

title:
	@echo protocolRouter starting

description:
	@(\
	echo;\
	echo "$(line1)";\
	echo "Compiling $(PROGRAM_NAME) module -> $<";\
	echo )

#=====================================================================
# linking
#=====================================================================
$(BIN_DIR)/protocolRouter: $(PROTOCOL_ROUTER_OBJ)
	$(CC) -o $@ $? $(LDFLAGS)
	
#=====================================================================
# compiling
#=====================================================================
$(OBJ_DIR)/protocolRouter.o : protocolRouter.c
	$(CC) -c $(ARCHFLAGS) $(CFLAGS) -o $@ $<

#=====================================================================
#  cleanup
#=====================================================================
clean:
	@-rm -f $(OBJ_DIR)/*.o
	@-rm -f $(BIN_DIR)/protocolRouter
	@echo protocolRouter clean

#=====================================================================
#  install 
#=====================================================================
install: \
    title \
    $(BIN_DIR)/protocolRouter 
	-cp $(BIN_DIR)/protocolRouter $(SIMPL_BIN)
	@echo protocolRouter install

#=====================================================================
#  uninstall 
#=====================================================================
uninstall: \
    title
	-rm $(SIMPL_BIN)/protocolRouter
	@echo protocolRouter uninstall

#===================
#  i386
#===================
i386:
	make install ARCHFLAGS="-march=i386"
	@echo protocolRouter i386

#===================
#  debug
#===================
debug:
	make install DE=-g
	@echo protocolRouter debug
