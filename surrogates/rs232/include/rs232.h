/***********************************************************************

FILE:			rs232.h

DATE:			07 Jul 20

DESCRIPTION:	This file contains necessary headers, definitions
				and global variables for the rs232_rw.c etc. files.

-----------------------------------------------------------------------
    Copyright (C) 2005 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list.
-----------------------------------------------------------------------
REVISIONS:
$Log: rs232.h,v $
Revision 1.1  2008/04/21 17:14:58  johnfcsoft
initial


***********************************************************************/

#ifndef _RS232_H
#define _RS232_H

// standard headers
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>

/*
note the values used in /usr/include/bits/termios.h
this file is called by /usr/include/termios.h
*/

// simpl headers
#include <simpl.h>
#include <simplLibProto.h>
#include <simplmiscProto.h>
#include <simplNames.h>
#include <surMsgs.h>

// definitions
#define SERIAL_DEVICE		"/dev/ttyS0"
#define OPEN_FLAGS			O_RDWR | O_NOCTTY
#define BAUD_RATE			B9600
#define CONTROL_MODE_FLAGS	BAUD_RATE | CRTSCTS | CS8 | CLOCAL | CREAD
#define INPUT_MODE_FLAGS	IGNPAR
#define OUTPUT_MODE_FLAGS	0
#define LOCAL_MODE_FLAGS	0
#define INT_WIDTH_32		4 // the size of a 32-bit int
#define INT_WIDTH_64		8 // the size of a 64-bit int

// function prototypes
int initialize(int, char **);
void rs232_r(void);
void rs232_w(void);
int readSerial(int, int, char *);
int upMemory(int);

// globals
char serialDevice[50];
char *memArea;
int memSize;
int verbose;
int intWidth;

#endif
