/*======================================================================

FILE:			binstr.h	

DESCRIPTION:	This file contains header info. for binstr.c

AUTHOR:			FC Software Inc.
-----------------------------------------------------------------------
    Copyright (C) 2000, 2002 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list. 

-----------------------------------------------------------------------

Revision history:
====================================================================
$Log: binstr.h,v $
Revision 1.2  2007/07/24 20:18:45  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:35  paul_c
Initial import

Revision 1.1  2003/04/23 20:42:17  root
Initial revision


====================================================================
======================================================================*/

#ifndef _BINSTR_H
#define	_BINSTR_H 

typedef union 
	{
	unsigned int m;
	unsigned int n;
	} CUI;

typedef union 
	{
	unsigned int m;
	signed int n;
	} CSI;

typedef union 
	{
	unsigned short int m;
	unsigned short int n;
	} CUSI;

typedef union 
	{
	unsigned short int m;
	signed short int n;
	} CSSI;

typedef union 
	{
	unsigned int m;
	float n;
	} CF;

void btos2(unsigned short int *, char *, int);
void btos4(unsigned int *, char *, int);
void stob2(unsigned short int *, char *, int);
void stob4(unsigned int *, char *, int);

#endif
