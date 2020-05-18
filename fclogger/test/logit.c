/*======================================
	logit.c

Description:
This module can be used to stimulate the fclogger.  It is written
using the simplipc library calls.

-----------------------------------------------------------------------
    Copyright (C) 2001,2007 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the 
    SIMPL project mailing list.

-----------------------------------------------------------------------
======================================*/
/*
 * $Log: logit.c,v $
 * Revision 1.2  2007/07/24 20:38:31  bobfcsoft
 * new contact info
 *
 * Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
 * Initial import
 *
 * Revision 1.5  2002/11/22 16:27:04  root
 * 2.0rc3
 *
 * Revision 1.3  2002/10/07 21:39:49  root
 * added globalMask to globals
 *
 * Revision 1.2  2002/06/12 18:19:23  root
 * fcipc merge completed
 *
 * Revision 1.1  2001/04/20 19:29:08  root
 * Initial revision
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#define _ALLOC
#include "loggerVars.h"
#undef _ALLOC

#define MY_MASK		0x00000001

// globals
unsigned globalMask;
char loggerName[40];
char message[80];
int nbytes;
char outArea[8192];
char inArea[8192];

#include "loggerProto.h"

/*--------------------------------------
	logit - entry point
--------------------------------------*/
int main(int argc, char **argv, char **envp)
{
static char *fn="logit";
int i;
int nrbytes;

printf("%s: starting\n",fn);

initialize(argc, argv);

for(i=0; i<10; i++)
	{
	printf("%s: sending log message[%d]\n",fn,i);

fcLogx(__FILE__, fn
	, globalMask
	, MY_MASK
	, "msg[%d]=<%.79s> of %d bytes -> %s"
	, i
	, message
	, nbytes
	, loggerName
	);
	
	sleep(10);
	}

printf("%s: done\n", fn);

name_detach();

exit(1);
}// end logit

/*============================================
	initialize - entry point
============================================*/
int initialize(int argc, char **argv)
{
static char *fn="initialize";
int i;                          /* loop variable */

globalMask = 0xffffffff;	// all on

/*===============================================
  process command args
===============================================*/
for(i=1; i<=argc; ++i)
        {
        char *p = argv[i];

        if(p == NULL) continue;

        if(*p == '-')
                {
                switch(*++p)
                        {
			case 'l':
                                for(;*p != 0; p++);
				
				sprintf(loggerName,"%s",++p);
				logger_ID = is_logger_upx(loggerName);
				break;

			case 'm':
                                for(;*p != 0; p++);
				
				sprintf(message,"%.79s",++p);
				nbytes=strlen(message);
				break;

                        case 'n':
                                for(;*p != 0; p++);
				
				name_attach(++p, NULL);
                                break;

                        default:
				printf("%s:unknown arg %s\n",fn, p);
                                break;
                        }/*end switch*/
                } /* end if *p */
        }/*end for i*/

} /* end initialize */
