#===============================================================
#
#  Makefile for simpl/surrogates tree
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
#Revision 1.12  2012/04/09 13:23:43  bobfcsoft
#support for experimental TCP surrogate
#
#Revision 1.11  2010/02/01 14:54:03  bobfcsoft
#moved tclSurrogate up
#
#Revision 1.10  2009/04/14 19:44:25  bobfcsoft
#added i386 target
#
#Revision 1.9  2009/01/19 20:12:40  bobfcsoft
#added tclSurrogate
#
#Revision 1.8  2009/01/08 17:29:10  bobfcsoft
#more debian prep
#
#Revision 1.7  2008/12/02 14:16:21  bobfcsoft
#removed @
#
#Revision 1.6  2008/11/27 20:34:33  johnfcsoft
#clobber-->clean
#
#Revision 1.5  2008/11/26 17:28:48  bobfcsoft
#cleanup
#
#Revision 1.4  2008/04/21 17:10:27  johnfcsoft
#added rs232 make
#
#Revision 1.3  2006/01/10 15:30:40  bobfcsoft
#no more tcp_x in v3.0
#
#Revision 1.2  2005/11/12 12:23:41  bobfcsoft
#allow tcp_x
#
#Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
#Initial import
#
#Revision 1.1  2002/11/22 16:38:21  root
#Initial revision
#
#===============================================================


#
# NOTE: these are relative to the src, test level subdirectory
#       and NOT to this level
#

SIMPL_BIN=../../../bin

PROJECT=SIMPL

PROTOCOL_ROUTER_DIR=protocol_router
TCP_PROTOCOL_DIR=tcp
X_TCP_PROTOCOL_DIR=tcp_x
RS232_PROTOCOL_DIR=rs232
TCLSURROGATE_DIR=tclSurrogate

#===================================
# entry point for simple make
#===================================
all:
	@echo "$(PROJECT) surrogate build <all> starting"
	@echo `date`
	make -C $(TCLSURROGATE_DIR)
	make -C $(PROTOCOL_ROUTER_DIR) SIMPL_BIN=$(SIMPL_BIN)
	make -C $(TCP_PROTOCOL_DIR) SIMPL_BIN=$(SIMPL_BIN)
	make -C $(X_TCP_PROTOCOL_DIR) SIMPL_BIN=$(SIMPL_BIN)
	make -C $(RS232_PROTOCOL_DIR)
	@echo "$(PROJECT) surrogate <all> done"
	@echo `date`

#===================================
# to force a total rebuild
#===================================
clean:
	@echo "$(PROJECT) surrogate <clean> starting"
	make -C $(PROTOCOL_ROUTER_DIR) -s clean
	make -C $(TCP_PROTOCOL_DIR) -s clean
	make -C $(X_TCP_PROTOCOL_DIR) -s clean
	make -C $(RS232_PROTOCOL_DIR) -s clean
	make -C $(TCLSURROGATE_DIR) -s clean
	@echo "$(PROJECT) surrogate <clean> done"

#===================================
# to move into master area
#===================================
install:
	@echo "$(PROJECT) surrogate build <install> starting"
	@echo `date`
	make -C $(TCLSURROGATE_DIR) install SIMPL_BIN=$(SIMPL_BIN)
	make -C $(PROTOCOL_ROUTER_DIR) install SIMPL_BIN=$(SIMPL_BIN)
	make -C $(TCP_PROTOCOL_DIR) install SIMPL_BIN=$(SIMPL_BIN)
	make -C $(X_TCP_PROTOCOL_DIR) install SIMPL_BIN=$(SIMPL_BIN)
	make -C $(RS232_PROTOCOL_DIR) install
	@echo "$(PROJECT) surrogate <install> done"
	@echo `date`

#========================
# remove from master area
#========================
uninstall:
	@echo "$(PROJECT) surrogate <uninstall> starting"
	@echo `date`
	make -C $(PROTOCOL_ROUTER_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN)
	make -C $(TCP_PROTOCOL_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN)
	make -C $(X_TCP_PROTOCOL_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN)
	make -C $(TCLSURROGATE_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN)
	@echo "$(PROJECT) surrogate <uninstall> done"
	@echo `date`

i386:
	@echo "$(PROJECT) surrogate build <i386> starting"
	@echo `date`
	make -C $(TCLSURROGATE_DIR) i386 SIMPL_BIN=$(SIMPL_BIN)
	make -C $(PROTOCOL_ROUTER_DIR) i386 SIMPL_BIN=$(SIMPL_BIN)
	make -C $(TCP_PROTOCOL_DIR) i386 SIMPL_BIN=$(SIMPL_BIN)
	make -C $(X_TCP_PROTOCOL_DIR) i386 SIMPL_BIN=$(SIMPL_BIN)
	make -C $(RS232_PROTOCOL_DIR) i386
	@echo "$(PROJECT) surrogate <i386> done"
	@echo `date`

