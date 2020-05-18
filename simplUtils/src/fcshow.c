/*======================================================================

FILE:			fcshow.c

DESCRIPTION:	This program displays current SIMPL processes.

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
$Log: fcshow.c,v $
Revision 1.5  2009/01/20 15:04:23  bobfcsoft
use DEFAULT_FIFO_PATH

Revision 1.4  2009/01/16 21:50:49  bobfcsoft
used MAX_FIFO_PATH_LEN

Revision 1.3  2009/01/15 16:42:30  bobfcsoft
add /tmp default to fifoPath

Revision 1.2  2007/07/24 19:52:40  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:58  paul_c
Initial import

Revision 1.6  2003/04/14 13:33:01  root
added MAC_OS_X stuff

Revision 1.5  2002/11/22 16:37:28  root
2.0rc3

====================================================================
======================================================================*/

#ifdef _MAC_OS_X
	#include <db.h>
#endif

#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "simplDefs.h"

int main()
{
DIR *directory;
struct dirent *file;
int len;
char fifoPath[MAX_FIFO_PATH_LEN + 1];
pid_t pid;
register int i;
char name[50];
char *p;

// get the fifo directory
// default to DEFAULT_FIFO_PATH if FIFO_PATH not defined
p = getenv("FIFO_PATH");
if (p == NULL)
	sprintf(fifoPath,DEFAULT_FIFO_PATH);
else
	sprintf(fifoPath,"%s",p);
	
if (access(fifoPath,F_OK) == -1)
	{
	printf("Unable to obtain fifo path-%s\n", strerror(errno));
	exit(-1);
	}

// open the fifo directory
directory = opendir(fifoPath);
if (directory == NULL)
	{
	printf("Cannot open fifo directory-%s\n", strerror(errno));
	exit(-1);
	}

printf("\nSimpl Name                      Pid\n");
printf("------------------------------  ----- \n");

// check all fifo directory entries
while ( (file = readdir(directory)) != NULL )
	{
	if ( (file->d_name[0] != 'Y') && (file->d_name[0] != '.') )
		{
		len = strlen(file->d_name);

		for (i = 0; i < len; i++)
			{
			if (file->d_name[i] == '.')
				{
				break;
				}
			}

		memset(name, 0, 50);
		memcpy(name, file->d_name, i);		
		pid = atoi(file->d_name + i + 1);
		
		if (pid > 0)
			printf("%-30s  %d\n", name, pid);
		}
	}

printf("\n");

closedir(directory);

return(0);
}
