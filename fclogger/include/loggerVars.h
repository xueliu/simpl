/*************************************************************

	loggerVars.h

DESCRIPTION:	
This file contains definitions of various things used by
the trace logger interface.

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1998, 2002 FCSoftware Inc. 

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
    $Log: loggerVars.h,v $
    Revision 1.1  2009/01/08 16:18:44  bobfcsoft
    relocated

    Revision 1.3  2007/07/24 20:18:45  bobfcsoft
    new contact info

    Revision 1.2  2006/07/11 14:48:02  bobfcsoft
    added Relay

    Revision 1.1.1.1  2005/03/27 11:50:35  paul_c
    Initial import

    Revision 1.9  2002/11/22 16:20:23  root
    2.0rc3

    Revision 1.8  2002/11/22 15:10:08  root
    2.0rc2

    Revision 1.7  2002/10/07 21:38:18  root
    removed globalMask declaration

    Revision 1.6  2002/06/12 18:13:17  root
    fcipc merge completed

    Revision 1.5  2000/10/04 01:45:54  root
    LGPL'd

    Revision 1.4  1999/09/09 15:54:46  root
    added new FCID stuff

    Revision 1.3  1999/04/08 14:20:52  root
    removed ERRLOG references

    Revision 1.2  1999/02/04 14:59:56  root
    cleanup

    Revision 1.1  1998/08/20 14:35:35  root
    Initial revision

 * Revision 1.1  1997/03/23  18:22:42  root
 * Initial revision
 *
=======================================================

*************************************************************/
#ifndef _LOGGERVARS_DEF
#define _LOGGERVARS_DEF

#include "simpl.h"	

#define TRACE_MAX_BUFFER_SIZE				1024
#define LOGGER_BUFFER_SIZE				200

#define LOGMASK_MARKER			0x01
#define LOGMASK_MESSAGE			0x02
#define LOGMASK_FUNCTION_IO		0x04
#define LOGMASK_INTERNAL_STRUCTURE	0x08
#define LOGMASK_MISCELLANEOUS		0x100

_ALLOC int logger_ID;
_ALLOC char logBuf[LOGGER_BUFFER_SIZE];
_ALLOC char loggerArea[TRACE_MAX_BUFFER_SIZE];

#endif
