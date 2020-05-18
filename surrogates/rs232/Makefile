#===============================================================
#
#  Makefile for surrogate/rs232 tree
#
#-----------------------------------------------------------------------
#    Copyright (C) 2005, 2008 FCSoftware Inc. 
#
#    This software is in the public domain.
#    Permission to use, copy, modify, and distribute this software and its
#    documentation for any purpose and without fee is hereby granted, 
#    without any conditions or restrictions.
#    This software is provided "as is" without express or implied warranty.#
#
#    If you discover a bug or add an enhancement contact us on the
#    SIMPL project mailing list.
#-----------------------------------------------------------------------
#===============================================================
#$Log: Makefile,v $
#Revision 1.6  2009/04/14 20:58:49  bobfcsoft
#added i386 target
#
#Revision 1.5  2008/12/02 14:18:10  bobfcsoft
#removed @
#
#Revision 1.4  2008/11/27 20:35:34  johnfcsoft
#clobber-->clean
#
#Revision 1.3  2008/11/26 17:30:01  bobfcsoft
#cleanup
#
#Revision 1.2  2008/05/13 00:33:43  bobfcsoft
#no test dir
#
#Revision 1.1  2008/04/21 17:12:48  johnfcsoft
#inital
#

#===============================================================

PROJECT=SIMPL

SRC_DIR=src
TEST_DIR=test

#===================================
# entry point for simple make
#===================================
all:
	@echo "$(PROJECT) surrogate/rs232 build <all> starting"
	@echo `date`
	make -C $(SRC_DIR)
	@echo "$(PROJECT) surrogate/rs232 <all> done"
	@echo `date`

#===================================
# to force a total rebuild
#===================================
clean:
	@echo "$(PROJECT) surrogate/rs232 <clean> starting"
	make -C $(SRC_DIR) -s clean
	@echo "$(PROJECT) surrogate/rs232 <clean> done"

#===================================
# to move into master area
#===================================
install:
	@echo "$(PROJECT) surrogate/rs232 build <install> starting"
	@echo `date`
	make -C $(SRC_DIR) install
	@echo "$(PROJECT) surrogate/rs232 <install> done"
	@echo `date`

i386:
	@echo "$(PROJECT) surrogate/rs232 build <i386> starting"
	@echo `date`
	make -C $(SRC_DIR) i386
	@echo "$(PROJECT) surrogate/rs232 <i386> done"
	@echo `date`
