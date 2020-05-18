#================================================
#  top level Makefile for SIMPL tree
#================================================
#-----------------------------------------------------------------------
#    Copyright (C) 2006,2008 SIMPL Open Source Project. 
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
#    If you discover a bug or add an enhancement here's contact us on the: 
#
#	SIMPL project mailing list
#-----------------------------------------------------------------------
#
#$Log: Makefile,v $
#Revision 1.8  2009/04/14 20:50:13  bobfcsoft
#added i386 stuff
#
#Revision 1.7  2009/01/08 17:18:42  bobfcsoft
#more debian prep
#
#Revision 1.6  2008/12/02 14:06:20  bobfcsoft
#removed @
#
#Revision 1.5  2008/11/27 20:31:34  johnfcsoft
#clobber-->clean
#
#Revision 1.4  2008/11/26 17:22:57  bobfcsoft
#cleanup
#
#Revision 1.3  2008/05/13 00:34:24  bobfcsoft
#fixed sandbox relay dependency
#
#Revision 1.2  2006/08/14 18:33:46  bobfcsoft
#split out tcl and python
#
#Revision 1.1.1.1  2005/03/27 11:50:30  paul_c
#Initial import
#
#Revision 1.12  2004/05/05 09:23:04  root
#fixed install target for Python
#
#Revision 1.11  2003/10/06 18:06:55  root
#added Python version hook
#
#Revision 1.10  2003/06/24 17:50:23  root
#added Python build stuff
#
#Revision 1.9  2003/06/23 16:27:07  root
#added dynamic target
#
#Revision 1.8  2002/12/02 15:56:26  root
#allow the TCLVER to be passed in
#
#Revision 1.7  2002/11/22 16:18:56  root
#2.0rc3
#
#Revision 1.6  2002/07/07 11:06:39  root
#fcUtils dir replaced by simplUtils dir
#
#Revision 1.5  2002/06/12 18:09:19  root
#fcipc merge completed
#
#Revision 1.4  2001/01/29 14:56:41  root
#moved FCUTILS_DIR earlier in order to accomodate simplmisc
#
#Revision 1.3  2000/10/13 13:47:55  root
#added SIMPL_IPC_DIR
#
#Revision 1.2  2000/04/17 14:44:54  root
#*** empty log message ***
#
#Revision 1.1  2000/04/14 01:47:05  root
#Initial revision
#
#
MYPWD=`pwd`

SIMPL_BIN=$(MYPWD)/bin
SIMPL_LIB=$(MYPWD)/lib
SIMPL_INCL=$(MYPWD)/include

SIMPL_IPC_DIR=simplipc
SURROGATES_DIR=surrogates
SIMPL_UTILS_DIR=simplUtils
FCLOGGER_DIR=fclogger
BENCHMARK_DIR=benchmarks/src

#===================================
# entry point for simple make
#===================================
all:
	@echo SIMPL Top Level Build all starting
	@echo `date`
	make -C $(SIMPL_IPC_DIR) SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(FCLOGGER_DIR) SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SIMPL_UTILS_DIR) SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SURROGATES_DIR) SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(BENCHMARK_DIR)
	@echo Done SIMPL TOP LEVEL all done
	@echo `date`

#=================================
# to force a total rebuild
#=================================
clean:
	@echo SIMPL Top Level clean starting
	make -C $(SIMPL_IPC_DIR) clean
	make -C $(SIMPL_UTILS_DIR) clean
	make -C $(SURROGATES_DIR) clean
	make -C $(FCLOGGER_DIR) clean
	make -C $(BENCHMARK_DIR) clean
	@echo SIMPL TOP LEVEL clean done

#========================
# to move into master area
#========================
install:
	@echo SIMPL Top Level Build install starting
	@echo `date`
	make -C $(SIMPL_IPC_DIR) install SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(FCLOGGER_DIR) install SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SIMPL_UTILS_DIR) install SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SURROGATES_DIR) install SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(BENCHMARK_DIR) install
	@echo SIMPL TOP LEVEL install done
	@echo `date`

#========================
# to remove from master area
#========================
uninstall:
	@echo SIMPL Top Level Build uninstall starting
	@echo `date`
	make -C $(SIMPL_IPC_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(FCLOGGER_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SIMPL_UTILS_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SURROGATES_DIR) uninstall SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	@echo SIMPL TOP LEVEL uninstall done
	@echo `date`

dynamic:
	@echo SIMPL Top Level Build dynamic starting
	@echo `date`
	make -C $(SIMPL_IPC_DIR) dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(FCLOGGER_DIR) dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SIMPL_UTILS_DIR) dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SURROGATES_DIR) install SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(BENCHMARK_DIR) install
	@echo SIMPL TOP LEVEL install done
	@echo `date`

#===================
#  i386
#===================
i386:
	@echo SIMPL Top Level Build install starting
	@echo `date`
	make -C $(SIMPL_IPC_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(FCLOGGER_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SIMPL_UTILS_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SURROGATES_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(BENCHMARK_DIR) i386
	@echo SIMPL TOP LEVEL install done
	@echo `date`

i386_dynamic:
	@echo SIMPL Top Level Build dynamic starting
	@echo `date`
	make -C $(SIMPL_IPC_DIR) i386_dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(FCLOGGER_DIR) i386_dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SIMPL_UTILS_DIR) i386_dynamic SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(SURROGATES_DIR) i386 SIMPL_BIN=$(SIMPL_BIN) SIMPL_LIB=$(SIMPL_LIB) SIMPL_INCL=$(SIMPL_INCL)
	make -C $(BENCHMARK_DIR) i386
	@echo SIMPL TOP LEVEL install done
	@echo `date`



