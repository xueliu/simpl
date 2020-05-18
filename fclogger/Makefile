#================================================
#
#    fclogger top level build
#
#-----------------------------------------------------------------------
#    Copyright (C) 2002,2008 FCSoftware Inc. 
#
#    This software is in the public domain.
#    Permission to use, copy, modify, and distribute this software and its
#    documentation for any purpose and without fee is hereby granted, 
#    without any conditions or restrictions.
#    This software is provided "as is" without express or implied warranty.
#
#    If you discover a bug or add an enhancement contact us on the 
#    SIMPL mailing list. 
#
#-----------------------------------------------------------------------
#
#$Log: Makefile,v $
#Revision 1.7  2009/04/14 19:25:47  bobfcsoft
#added i386 target
#
#Revision 1.6  2009/01/08 17:20:04  bobfcsoft
#more debian prep
#
#Revision 1.5  2008/12/02 14:09:34  bobfcsoft
#removed @
#
#Revision 1.4  2008/11/27 20:38:21  johnfcsoft
#clobber-->clean
#
#Revision 1.3  2008/11/26 17:24:11  bobfcsoft
#cleanup
#
#Revision 1.2  2007/07/24 20:36:38  bobfcsoft
#new contact info
#
#Revision 1.1.1.1  2005/03/27 11:50:39  paul_c
#Initial import
#
#Revision 1.3  2003/04/23 20:43:35  root
#added support for dynamic library
#
#Revision 1.2  2002/11/22 16:26:24  root
#2.0rc3
#
#Revision 1.1  2002/06/12 18:17:51  root
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
	@echo "$(PROJECT) $(TASK) build <dynamic> starting"
	@echo `date`
	make -C $(SRC_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(TEST_DIR) i386
	@echo "$(PROJECT) $(TASK) <i386> done"
	@echo `date`

i386_dynamic:
	@echo "$(PROJECT) $(TASK) build <dynamic> starting"
	@echo `date`
	make -C $(SRC_DIR) i386_dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(TEST_DIR) i386
	@echo "$(PROJECT) $(TASK) <i386_dynamic> done"
	@echo `date`
