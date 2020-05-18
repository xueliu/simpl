/***********************************************************************

FILE:			rs232.c

DATE:			07 August 30

DESCRIPTION:	This program immediately forks into two separate
				programs: viz. rs232_r and rs232_w.

AUTHOR:			FC Software Inc.
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
$Log: rs232_rw.c,v $
Revision 1.1  2008/04/21 17:19:06  johnfcsoft
initial

***********************************************************************/


// NOTE: make certain you have chmod a+rw /dev/ttyS* !!!

// application header
#include "rs232.h"

int main(int argc, char **argv)
{
pid_t childPid;
const char *fn = "surrogate_rs232";

// initialize
if (initialize(argc, argv) == -1)
	{
	_simpl_log("%s: cannot initialize\n", fn);
	exit(-1);
	}

// fork off into surrogate receiver and sender parent processes 
childPid = fork();
if (childPid < 0) // failure
	{
	_simpl_log("%s: cannot fork-%s\n", fn, strerror(errno));
	}
else if (childPid == 0) // child
	{
	rs232_r();
	}
else // parent
	{
	rs232_w();
	}

return(0);
}

/**********************************************************************
FUNCTION:	initialize(int, char **argv)

PURPOSE:	set serial port parameters
  
RETURNS:	int
**********************************************************************/	

int initialize(int argc, char **argv)
{
// memArea is global
// memSize is global
char *fn = "intialize_rs232";
register int i;
struct termios tio;
int fd;

// initialize global memory
memArea = (char *)malloc(1024);
if (!memArea)
	{ 
	_simpl_log("%s: memory allocation error-%s\n", fn, strerror(errno));
	return(-1);
	}

// set global memory size to a default 1k
memSize = 1024;

// set the default serial port
strcpy(serialDevice, SERIAL_DEVICE);

// set verbosity off
verbose = 0;

// intWidth is a global variable which is 4 for 32-bit and 8 for 64-bit
// INT_WIDTH_32 is the default macro set in surrogate.h
intWidth = INT_WIDTH_32;

// process command line arguments (if any)
for (i = 1; i <= argc; ++i) 
	{	
	char *p = argv[i];

	if (p == NULL)
		{
		continue;
		}

	if (*p == '-')
		{             
		switch (*++p)
			{
			case 'i':
				if (*++p == 0)
					{
					p++;
					}
				intWidth = atoi(p); 
				break;

			case 's': // serial device
				if (*++p == 0)
					{
					p++;
					}
				strcpy(serialDevice, p);
				break;

			case 'v': //verbosity
				verbose = 1;
				break;

			default:
				_simpl_log("%s: bad command line arg `%c'\n", fn, *p);
				return(-1);
			}
		}
	}	

// open the serial port
fd = open(serialDevice, OPEN_FLAGS);
if (fd < 0)
	{
	_simpl_log("%s: cannot open %s for r/w-%s\n", fn, serialDevice,
		strerror(errno));
	exit(-1);
	}

// clear the new structure for port settings
bzero(&tio, sizeof(tio));

/*
set the control flags to the following defaults (defined in rs232.h):
1. hardware handshaking
2. 8 bits, no parity, 1 stop bit
3. local connection, ie. no modem
4. enable ability to receive characters
*/
tio.c_cflag |= CONTROL_MODE_FLAGS;

// set the input flags to ignore parity errors, otherwise raw
tio.c_iflag |= INPUT_MODE_FLAGS;

// set the output flags to raw
tio.c_oflag = OUTPUT_MODE_FLAGS;

// set the local flags to non-canonical, no echo etc.
tio.c_lflag = LOCAL_MODE_FLAGS;

/*
set the control characters to:
1. no intercharacter timer use
2. return after at least one byte
*/
tio.c_cc[VMIN] = 1;
tio.c_cc[VTIME] = 0;

// clear out
tcflush(fd, TCIFLUSH);

// set the new port settings
tcsetattr(fd, TCSANOW, &tio);

close(fd);

return(0);
}

/**********************************************************************
FUNCTION:	rs232_r(void)

PURPOSE:	receive messages via serial port and SIMPL send them to the
			 appropriate surrogate.
  
RETURNS:	void
**********************************************************************/	

void rs232_r()
{
// memArea is global
// memSize is global
char *fn = "rs232_r";
int fd;
int id;
int numRead;
int numBytes;
int token;
int pid;
char receiver[MAX_PROGRAM_NAME_LEN + 1];

#ifdef SUR_CHR	/********** char message **********/
	const int hdrSize = sizeof(SUR_MSG_CHR_HDR);
	SUR_MSG_CHR_HDR hdr;
#else			/********** binary message **********/
	const int hdrSize = sizeof(SUR_MSG_HDR);
	SUR_MSG_HDR hdr;
#endif

// attach a name for local IPC
if (name_attach(RS232_READER, NULL) == -1)
	{
	_simpl_log("%s: cannot attach name-%s\n", fn, whatsMyError());
	exit(-1);
	}

// open the serial port
fd = open(serialDevice, OPEN_FLAGS);
if (fd <  0)
	{
	_simpl_log("%s: cannot open %s for r/w-%s\n", fn, serialDevice,
		strerror(errno));
	exit(-1);
	}

while (1)
	{
	// all incoming messages start with a standard header
	numRead = readSerial(fd, hdrSize, (char *)&hdr);	
	if (numRead == -1)
		{
		_simpl_log("%s: problem reading header from %s-%s\n", fn,
			serialDevice, strerror(errno));
		continue;
		}

	// check the header size
	if (numRead != hdrSize)
		{
		_simpl_log("%s: wrong header size=%d\n", fn, numRead);
		continue;
		}

	// interpret message header
	#ifdef SUR_CHR	/********** char message **********/
		numBytes = stobSI(hdr.nbytes, intWidth);
		token = stobSI(hdr.token, intWidth);
		pid = stobSI(hdr.surPid, intWidth);
	#else			/********** binary message **********/
		numBytes = hdr.nbytes;
		token = hdr.token;
		pid = hdr.surPid;
	#endif

	// adjust message memory size if necessary
	if ( (numBytes + hdrSize) > memSize )
		{
		if (upMemory(numBytes + hdrSize) == -1)
			{
			_simpl_log("%s: cannot adjust memory-%s\n", fn, strerror(errno));
			exit(-1);
			}
		}

	// copy original header to the start of nmessage memory
	memcpy(memArea, (char *)&hdr, hdrSize);

	// read the message content
	numRead = readSerial(fd, numBytes, memArea + hdrSize);
	if (numRead == -1)
		{
		_simpl_log("%s: problem reading message content from %s-%s\n",
			fn,	serialDevice, strerror(errno));
		continue;
		}

	// check the size of the message
	if (numRead != numBytes)
		{
		_simpl_log("%s: wrong message size=%d\n", fn, numRead);
		continue;
		}

	if (verbose)
		{
		switch (token)
			{
			case SUR_NAME_LOCATE:
				printf("%s: SUR_NAME_LOCATE\n", fn);
				break;
			case SUR_REPLY:
				printf("%s: SUR_REPLY\n", fn);
				break;
			case SUR_ERROR:
				printf("%s: SUR_ERROR\n", fn);
				break;
			case SUR_ALIVE_REPLY:
				printf("%s: SUR_ALIVE_REPLY\n", fn);
				break;
			case SUR_SEND:
				printf("%s: SUR_SEND\n", fn);
				break;
			case SUR_PROXY:
				printf("%s: SUR_PROXY\n", fn);
				break;
			case SUR_ALIVE:
				printf("%s: SUR_ALIVE\n", fn);
				break;
			case SUR_CLOSE:
				printf("%s: SUR_CLOSE\n", fn);
				break;
			default:
				printf("%s: unknown token\n", fn);
			}
		}

	// set the receiving surrogate's SIMPL name
	switch (token)
		{
		case SUR_NAME_LOCATE:
			strcpy(receiver, SURROGATE_RS232_S_PARENT);
			break;
		case SUR_REPLY:
		case SUR_ERROR:
		case SUR_ALIVE_REPLY:
			sprintf(receiver, "%s_%d", SURROGATE_RS232_R_CHILD, pid);
			break;
		case SUR_SEND:
		case SUR_PROXY:
		case SUR_ALIVE:
		case SUR_CLOSE:
			sprintf(receiver, "%s_%d", SURROGATE_RS232_S_CHILD, pid);
			break;
		default:
			_simpl_log("%s: unknown message token\n", fn);
			continue;
		}

	// name locate the appropriate program
	id = name_locate(receiver);
	if (id == -1)
		{
		_simpl_log("%s: name locate of %s error-%s\n", fn, receiver,
			whatsMyError());
		continue;
		}

	// send  message to the appropriate receiver
	if (Send(id, memArea, NULL, hdrSize + numBytes, 0) == -1)
		{
		_simpl_log("%s: send to %s error-%s\n", fn, receiver,
			whatsMyError());
		}

	// close the name located id
	close(id);
	}
}	

/**********************************************************************
FUNCTION:	readSerial(int fd, int numBytes)

PURPOSE:	read numBytes from a serial line
  
RETURNS:	number of bytes read
**********************************************************************/	

int readSerial(int fd, int numBytes, char *mem)
{
int numRead = 0;
int numLeft = numBytes;
char *ptr = mem;
int num;

while (numLeft > 0)
	{
	num = read(fd, ptr, numLeft);
	if (num < 0)
		{
		return(-1);
		}
	else if (num == 0)
		{
		if((numBytes - numLeft) == 0)
			{
			errno = EIO;
			return(-1);
			} 
		else
			{
			break;
			}
		}

	numLeft -= num;
	numRead += num;
	ptr += num;
	}

return(numRead);
}

/**********************************************************************
FUNCTION:	rs232_w(void)

PURPOSE:	receive messages from surrogates to write out via serial port
  
RETURNS:	void
**********************************************************************/	

void rs232_w()
{
// memArea is global
// memSize is global
char *fn = "rs232_w";
int fd;
int nBytes;
char *sender;

// attach a name for local IPC
if (name_attach(RS232_WRITER, NULL) == -1)
	{
	_simpl_log("%s: cannot attach name-%s\n", fn, whatsMyError());
	exit(-1);
	}

// open the serial port
fd = open(serialDevice, OPEN_FLAGS);
if (fd <  0)
	{
	_simpl_log("%s: cannot open %s for r/w-%s\n", fn, serialDevice,
		strerror(errno));
	exit(-1);
	}

while(1)
	{
	// receive incoming message header
	nBytes = Receive(&sender, NULL, 0);
	if (nBytes == -1)
		{
		_simpl_log("%s: receive error-%s\n",fn, whatsMyError());
		continue;
		}
	
	// increase memory if necessary
	if (nBytes > memSize)
		{
		// adjust memory
		if (upMemory(nBytes) == -1)
			{
			_simpl_log("%s: cannot adjust memory-%s\n", fn, strerror(errno));
			exit(-1);
			}
		}

	// get the message from sender's shmem
	simplRcopy(sender, memArea, nBytes);

	// reply to sender
	Reply(sender, NULL, 0);

	// write the message out serially
	if (write(fd, memArea, nBytes) == -1)
		{
		_simpl_log("%s: problem writing message to %s-%s\n", fn,
			serialDevice, strerror(errno));
		continue;
		}
	}
}

/**********************************************************************
FUNCTION:	int upMemory(int)

PURPOSE:	Adjust the global dynamic memory for upward growth. 

RETURNS:	int	
**********************************************************************/	

int upMemory(int nbytes)
{
// memArea is global
// memSize is global
const static char *fn = "upMemory";

// realloc memory
memArea = (char *)realloc(memArea, nbytes);
if (!memArea)
	{ 
	_simpl_log("%s: memory allocation error-%s\n", fn, strerror(errno));
	return(-1);
	}

// reset global memory size
memSize = nbytes;

return(0);
}
