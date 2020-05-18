#===============================================================
#
#  Makefile for simplUtils tree
#
#-----------------------------------------------------------------------
#    Copyright (C) 1999, 2008 FCSoftware Inc. 
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
#===============================================================
#$Log: Makefile,v $
#Revision 1.6  2009/04/14 19:28:27  bobfcsoft
#added i386 target
#
#Revision 1.5  2009/01/08 17:21:00  bobfcsoft
#more debian prep
#
#Revision 1.4  2008/12/02 14:15:37  bobfcsoft
#removed @
#
#Revision 1.3  2008/11/27 20:33:50  johnfcsoft
#clobber-->clean
#
#Revision 1.2  2008/11/26 17:25:45  bobfcsoft
#cleanup
#
#Revision 1.1.1.1  2005/03/27 11:50:54  paul_c
#Initial import
#
#Revision 1.4  2003/04/23 20:45:37  root
#added support for dynamic libraries
#
#Revision 1.3  2003/04/14 13:25:30  root
#added test directory
#
#Revision 1.2  2000/10/04 02:02:01  root
#LGPL'd
#
#Revision 1.1  1999/11/04 15:42:43  root
#Initial revision
#
#===============================================================

#
# NOTE: these are relative to the next lower subdirectory
#       and NOT to this level
#
SIMPL_BIN=../../bin
SIMPL_LIB=../../lib
SIMPL_INCL=../../include

PROJECT=SIMPL

SRC_DIR=src
TEST_DIR=test

#===================================
# entry point for simple make
#===================================
all:
	@echo "$(PROJECT) simplUtils build <all> starting"
	@echo `date`
	make -C $(SRC_DIR) SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL) 
	make -C $(TEST_DIR) 
	@echo "$(PROJECT) simplUtils <all> done"
	@echo `date`

#===================================
# to force a total rebuild
#===================================
clean:
	@echo "$(PROJECT) simplUtils <clean> starting"
	make -C $(SRC_DIR) -s clean
	make -C $(TEST_DIR) -s clean
	@echo "$(PROJECT) simplUtils <clean> done"

#===================================
# to move into master area
#===================================
install:
	@echo "$(PROJECT) simplUtils build <install> starting"
	@echo `date`
	make -C $(SRC_DIR) install SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL) 
	make -C $(TEST_DIR) install
	@echo "$(PROJECT) simplUtils <install> done"
	@echo `date`

#===================================
# remove from master area
#===================================
uninstall:
	@echo "$(PROJECT) simplUtils build <uninstall> starting"
	@echo `date`
	make -C $(SRC_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL) 
	@echo "$(PROJECT) simplUtils <uninstall> done"
	@echo `date`

dynamic:
	@echo "$(PROJECT) simplUtils build <dynamic> starting"
	@echo `date`
	make -C $(SRC_DIR) dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL) 
	make -C $(TEST_DIR) install
	@echo "$(PROJECT) simplUtils <dynamic> done"
	@echo `date`

i386:
	@echo "$(PROJECT) simplUtils build <dynamic> starting"
	@echo `date`
	make -C $(SRC_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL) 
	make -C $(TEST_DIR) i386
	@echo "$(PROJECT) simplUtils <dynamic> done"
	@echo `date`

i386_dynamic:
	@echo "$(PROJECT) simplUtils build <dynamic> starting"
	@echo `date`
	make -C $(SRC_DIR) i386_dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL) 
	make -C $(TEST_DIR) i386
	@echo "$(PROJECT) simplUtils <dynamic> done"
	@echo `date`
