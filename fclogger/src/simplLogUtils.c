/*************************************************************
	simplLogUtils.c

DESCRIPTION:	
This file contains some core functions for trace logging.

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 2001, 2002, 2007 FCSoftware Inc. 

    This software is in the public domain.
    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, 
    without any conditions or restrictions.
    This software is provided "as is" without express or implied warranty.

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list. 

-----------------------------------------------------------------------

Revision history:
=======================================================
    $Log: simplLogUtils.c,v $
    Revision 1.3  2008/01/30 16:54:03  bobfcsoft
    bracketed v functions with va_start va_end

    Revision 1.2  2007/07/24 20:40:47  bobfcsoft
    new contact info

    Revision 1.1.1.1  2005/03/27 11:50:45  paul_c
    Initial import

    Revision 1.6  2003/03/28 01:15:01  root
    expanded timestamp in log message

    Revision 1.5  2002/11/22 16:28:17  root
    2.0rc3

    Revision 1.4  2002/11/19 14:57:48  root
    2.0rc1

    Revision 1.3  2002/06/12 18:21:38  root
    fcipc merge completed

    Revision 1.2  2001/06/18 15:54:22  root
    added time.h include

    Revision 1.1  2001/01/29 15:18:16  root
    Initial revision

=======================================================

*************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h> 		/* for exit etc */
#include <string.h>  		/* for memcpy etc */
#include <unistd.h>		/* for getnid */
#include <fcntl.h>              // O_CREAT etc
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>		// for memory map stuff
#include <sys/types.h>

#include "standardTypes.h"	// UINT8 etc

#define _ALLOC extern
#include "loggerVars.h"	// all global variables
#undef _ALLOC

// Prototypes
#include "loggerProto.h"

/*===============================================================
	is_logger_upx - entry point
===============================================================*/
int is_logger_upx(char *name)
{
int rc;		/* ID for logger */

rc = -1;
if(name[0] != 0)
	{
	rc=name_locate(name);
	}
 
return(rc);

} // end is_logger_upx

/*===============================================================
	logMsgx - entry point
===============================================================*/
int logMsgx(char *fileName, char *who, char *text)
{
int retcode=0;
int f;			// length of fileName
int w;			// length of who
int t;			// length of text
char *p;		// working pointer

p = loggerArea;

f=strlen(fileName);
w=strlen(who);
t=strlen(text);

if(f+w+t+1 > LOGGER_BUFFER_SIZE)
	{
	sprintf(p,"(%s) %s:",fileName, who);
	memcpy(&p[f+w+1],text,LOGGER_BUFFER_SIZE-f-w-1);
	}
else
	{
	sprintf(p,"(%s) %s:%s",fileName, who, text);
	}
	
if(logger_ID != -1)
	{
	retcode=Send(logger_ID,loggerArea,NULL,LOGGER_BUFFER_SIZE,0);

	if(retcode == -1)
		logger_ID = -1;
	}

return(retcode);
} // end logMsgx

/*====================================================
	fcLogx - entry point
====================================================*/
int fcLogx
(
   char      *fileName,
   char      *function,
   unsigned int globalMask,
   unsigned int userMask,
   char      *format,
   ...
)
#define START_BUFFER_SIZE 1024    /*  just for the beginning */
{
static char *fn = "fcLog";
static FILE *fiNULL         = NULL;
static char *bufPtr       = NULL;
static char buffer[1024];      /* for error messages in this routine */
static int  bufferSize     = START_BUFFER_SIZE;
int iSize = 0;
int iRC   = 0;
static char tmstmp [16];  /* 00.000 (ss:msec) */
struct timeval   tv_now;  /* These are used by gettimeofday */
struct timezone  tz_now;
struct tm        *now;
int rc = 0;

if((globalMask & userMask) && logger_ID != -1)
  {
/***************************************************************************/
/* Macro definitions :                                                     */
/* va_list : Declare a variable (vaArgs) to point to each argument in turn */
/* va_start : Initialize vaArgs to point to first unnamed argument         */
/***************************************************************************/

va_list   vaArgs; 

/*
 *  Format a timestamp with seconds and milliseconds (00.000)
 */
gettimeofday (&tv_now, &tz_now);
now = localtime ((time_t *) &tv_now.tv_sec);
sprintf (tmstmp, "%02d%02d%02d.%03d",
	now->tm_hour,
	now->tm_min,
            now->tm_sec,
            (int)tv_now.tv_usec / 1000  /* msec */
           );


/***************************************************************************/
/***************************************************************************/
if ( bufPtr == NULL )
  	{
    	bufPtr = (char *) malloc(bufferSize);
    	if ( bufPtr == NULL )
    		{
      		sprintf(buffer,"unable to allocate %d bytes",
                	bufferSize);
      		logMsgx(__FILE__, fn, buffer);
      		return(-1);
    		}
  	}

/*****************************************************************************
 *  This is the only safe way to measure size of possible output             *
 ****************************************************************************/
if (fiNULL == NULL)
  	{
    	fiNULL = fopen ("/dev/null", "w");

    	if ( fiNULL == NULL )
    		{
      		sprintf (buffer,"System error: /dev/null cannot be open");
     		logMsgx(__FILE__, fn, buffer);
      		return(-1);
    		}
  	}

/*****************************************************************************
 *                                                                           *
 *  printf in /dev/null to estimate proper size of buffer to allocate        *
 *                                                                           *
 ****************************************************************************/

#define HEADER_PRINT  "[%-8.8s:%-12.12s] %s ", \
                      fileName,  function, tmstmp

iRC = fprintf (fiNULL, HEADER_PRINT);
if ( iRC < 0 )
  {
    sprintf (buffer, "I/O error while fprintf-ing HEADER to /dev/null");
    logMsgx(__FILE__, fn, buffer);
    return(-1);
  }

iSize = iRC;


/*****************************************************************************
 * Print variable part of parameter list                                     *
 ****************************************************************************/
va_start( vaArgs, format );
iRC =  vfprintf ( fiNULL, format, vaArgs);
va_end( vaArgs );

if ( iRC < 0  )
	{
    	sprintf(buffer,"I/O error while fprintf-ing message to /dev/null");
    	logMsgx(__FILE__, fn, buffer);
    	return(-1);
  	}

if ( iRC > 220  )
  {
    sprintf(buffer, HEADER_PRINT);
    strcat(buffer, ":message is greater than 200");
    logMsgx(__FILE__, fn, buffer);
  } /*end if ( iRC > 220) */

iSize += iRC;

if ( iSize > bufferSize )
  {
    bufferSize = iSize;

    sprintf(buffer,"attempting to grow buffer to %d bytes", bufferSize);
    logMsgx(__FILE__, fn, buffer);

    bufPtr = (char *) realloc ( bufPtr, bufferSize );
  }


/*****************************************************************************
 *  Check realloc                                                            *
 ****************************************************************************/
if (bufPtr == NULL )
  {
    sprintf(buffer, HEADER_PRINT);
    strcat(buffer, ":buffer grow failed");
    logMsgx(__FILE__, fn, buffer);
    return(-1);
  }

/*****************************************************************************
 *                                                                           *
 *  And now, FINALLY, do that thing you came here for ... !!!                *
 *                                                                           *
 ****************************************************************************/

iRC = sprintf(bufPtr, HEADER_PRINT );
va_start( vaArgs, format );
vsprintf ( bufPtr+iRC, format, vaArgs);
va_end( vaArgs );

if(logger_ID != -1)
	{
	rc=Send(logger_ID,bufPtr,NULL,LOGGER_BUFFER_SIZE,0);

	if(rc == -1)
		logger_ID = -1;
	}

  } /* end if globalMask & userMask */

#undef START_BUFFER_SIZE

return(rc);

} // fcLogx
