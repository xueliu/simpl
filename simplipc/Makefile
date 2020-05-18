#================================================
#
#    simplipc top level build
#
#-----------------------------------------------------------------------
#    Copyright (C) 2002,2008 FCSoftware Inc. 
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
#    If you discover a bug or add an enhancement please contact us on the 
#
#	SIMPL project mailing list
#-----------------------------------------------------------------------
#
#$Log: Makefile,v $
#Revision 1.6  2009/04/14 20:54:46  bobfcsoft
#added i386 targets
#
#Revision 1.5  2009/01/08 17:27:50  bobfcsoft
#more debian prep
#
#Revision 1.4  2008/12/02 14:08:05  bobfcsoft
#removed @
#
#Revision 1.3  2008/11/27 20:33:01  johnfcsoft
#clobber-->clean
#
#Revision 1.2  2008/11/26 17:27:20  bobfcsoft
#cleanup
#
#Revision 1.1.1.1  2005/03/27 11:50:50  paul_c
#Initial import
#
#Revision 1.2  2003/04/23 20:44:55  root
#added support for dynamic library
#
#Revision 1.1  2002/06/12 18:38:05  root
#Initial revision
#
#

#
# NOTE: these are relative to the next lower subdirectory
#       and NOT to this level
#
SIMPL_BIN=../../bin
SIMPL_LIB=../../lib
SIMPL_INCL=../../include

PROJECT=SIMPL
TASK=fclogger

SRC_DIR=src
TEST_DIR=test

#===================================
# entry point for simple make
#===================================
all:
	@echo "$(PROJECT) $(TASK) build <all> starting"
	@echo `date`
	make -C $(SRC_DIR) SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(TEST_DIR)
	@echo "$(PROJECT) $(TASK) <all> done"
	@echo `date`

#=================================
# to force a total rebuild
#=================================
clean:
	@echo "$(PROJECT) $(TASK) <clean> starting"
	make -C $(SRC_DIR) -s clean
	make -C $(TEST_DIR) -s clean
	@echo "$(PROJECT) $(TASK) <clean> done"

#========================
# to move into master area
#========================
install:
	@echo "$(PROJECT) $(TASK) build <install> starting"
	@echo `date`
	make -C $(SRC_DIR) install SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(TEST_DIR) install
	@echo "$(PROJECT) $(TASK) <install> done"
	@echo `date`

#========================
# remove from master area
#========================
uninstall:
	@echo "$(PROJECT) $(TASK) build <uninstall> starting"
	@echo `date`
	make -C $(SRC_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	@echo "$(PROJECT) $(TASK) <uninstall> done"
	@echo `date`

#========================
# to move into master area
#========================
dynamic:
	@echo "$(PROJECT) $(TASK) build <dynamic> starting"
	@echo `date`
	make -C $(SRC_DIR) dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(TEST_DIR) install
	@echo "$(PROJECT) $(TASK) <dynamic> done"
	@echo `date`

i386:
	@echo "$(PROJECT) $(TASK) build <i386> starting"
	@echo `date`
	make -C $(SRC_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(TEST_DIR) i386
	@echo "$(PROJECT) $(TASK) <i386> done"
	@echo `date`

i386_dynamic:
	@echo "$(PROJECT) $(TASK) build <i386_dynamic> starting"
	@echo `date`
	make -C $(SRC_DIR) i386_dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(TEST_DIR) i386
	@echo "$(PROJECT) $(TASK) <i386_dynamic> done"
	@echo `date`
