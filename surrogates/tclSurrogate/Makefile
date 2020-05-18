#===============================================================
#
#  Makefile for surrogates/tclSurrogate
#
#-----------------------------------------------------------------------
#    Copyright (C) 2009 FCSoftware Inc. 
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
#===============================================================
#
#$Log: Makefile,v $
#Revision 1.2  2009/04/14 20:59:31  bobfcsoft
#added i386 target
#
#Revision 1.1  2009/01/19 20:13:32  bobfcsoft
#moved from simpl/tcl
#
#
#===============================================================

#
# NOTE: these are relative to the next lower subdirectory
#       and NOT to this level
#
SIMPL_BIN=../../../bin

PROJECT=SIMPL

SRC_DIR=src
TEST_DIR=test

#===================================
# entry point for simple make
#===================================
all:
	@echo "$(PROJECT) tclSurrogate build <all> starting"
	@echo `date`
	@make -C $(SRC_DIR) SIMPL_BIN=$(SIMPL_BIN)
	@make -C $(TEST_DIR) 
	@echo "$(PROJECT) tclSurrogate <all> done"
	@echo `date`

#===================================
# to force a total rebuild
#===================================
clean:
	@echo "$(PROJECT) tclSurrogate <clean> starting"
	@make -C $(SRC_DIR) -s clean
	@make -C $(TEST_DIR) -s clean
	@echo "$(PROJECT) tclSurrogate <clean> done"

#===================================
# to move into master area
#===================================
install:
	@echo "$(PROJECT) tclSurrogate <install> starting"
	@echo `date`
	@make -C $(SRC_DIR) install SIMPL_BIN=$(SIMPL_BIN)
	@make -C $(TEST_DIR) install
	@echo "$(PROJECT) tclSurrogate <install> done"
	@echo `date`

#========================
# remove from master area
#========================
uninstall:
	@echo "$(PROJECT) tclSurrogate <uninstall> starting"
	@echo `date`
	make -C $(SRC_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN)
	@echo "$(PROJECT) tclSurrogate <uninstall> done"
	@echo `date`

i386:
	@echo "$(PROJECT) tclSurrogate <i386> starting"
	@echo `date`
	@make -C $(SRC_DIR) i386 SIMPL_BIN=$(SIMPL_BIN)
	@make -C $(TEST_DIR) i386
	@echo "$(PROJECT) tclSurrogate <i386> done"
	@echo `date`

