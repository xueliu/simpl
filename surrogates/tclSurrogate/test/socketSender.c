/*************************************************************
FILE:		socketSender.c

DESCRIPTION:	
This module acts as Tcl/Tk GUI place holder.

AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1999, 2002, 2007 FCSoftware Inc. 

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
$Log: socketSender.c,v $
Revision 1.2  2010/02/19 16:33:33  bobfcsoft
remove termios.h

Revision 1.1  2009/01/19 20:16:02  bobfcsoft
relocated files

Revision 1.2  2007/07/24 21:47:20  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:50  paul_c
Initial import

Revision 1.10  2002/11/22 16:34:22  root
2.0rc3

Revision 1.9  2002/07/06 12:22:55  root
default logger_name to null

Revision 1.8  2002/06/12 18:33:59  root
added test mode for no reply timeout test
fcipc merge completed

Revision 1.7  2000/10/13 14:07:14  root
LGPL

Revision 1.6  2000/04/17 15:01:26  root
SEND_NO_REPLY added

Revision 1.5  2000/02/15 18:54:29  root
gets -> fgets to fix warning

Revision 1.4  2000/02/15 18:53:07  root
fixed int main warning for 2.2 systems

Revision 1.3  2000/01/21 14:19:01  root
added simulated crash condition

Revision 1.2  1999/12/30 12:14:46  root
made compatible with simpler message structures

Revision 1.1  1999/11/04 15:42:09  root
Initial revision

Revision 1.5  1999/08/23 11:14:48  root
cleanup

Revision 1.4  1999/08/10 19:13:40  root
cleanup

Revision 1.3  1999/08/06 20:27:35  root
removed linux/time.h

Revision 1.2  1999/08/03 22:37:17  root
enabled logger stuff

Revision 1.1  1999/07/13 18:11:54  root
Initial revision

=======================================================
*************************************************************/
#include <stdio.h>
#include <stdlib.h> 				/* for exit etc */
#include <string.h>  				/* for memcpy etc */
#include <unistd.h>					/* for getnid */
#include <sys/time.h>			// select etc.
#include <fcntl.h>
//#include <termios.h>

#include "surroMsgs.h"
#include "surroTestMsgs.h"

#define _ALLOC
#include "socketSender.h"
#undef _ALLOC

// Prototypes
#include "simplProto.h"
#include "socketProto.h"
#include "loggerProto.h"

void initialize(int, char **);

/*=========================================================
	socketSender - entry point
=========================================================*/
int main(int argc, char **argv)
{
static char *fn="socketSender";
int sender;
int x_it=0;
char line[80];
SG_REPLY_MSG *inMsg;
int toPid;

inMsg=(SG_REPLY_MSG *)inArea;

initialize(argc, argv);

#if 0
// arm the select routine
my_fds[0] = STDIN_FILENO;   // keyboard
FD_ZERO(&watchset);
FD_SET(my_fds[0], &watchset);
maxfd=my_fds[0];
#endif

printf("-> ");
fflush(stdout);
while(!x_it)
	{

#if 0
printf("top of loop my_fds[0]=%d my_fds[1]=%d\n",
	my_fds[0],
	my_fds[1]);
#endif

	memset(line,0,3);
	inset = watchset;
	select(maxfd+1, &inset, NULL, NULL, NULL);

// Is this from keyboard
	if(FD_ISSET(my_fds[0], &inset))  //  keyboard is ready
		{
		fgets(line,80,stdin);
		line[strlen(line)-1]=0;  // remove CR

		switch(line[0])
			{
			case '?':	// help
				printf("socketSender commands:\n");
				printf("n <name> - name_attach\n");
				printf("d - detach_name\n");
				printf("l - connect to logger\n");
				printf("t <msg> - logit\n");
				printf("w <name> - name locate\n");
				printf("s <msg> - send it\n");
				printf("S <msg> - send no reply\n");
				printf("a - send ACK\n");
				printf("q - quit\n");
				break;

			case 'x':	// simulated crash
				exit(0);
				break;

			case 'n':	// name attach
				{
				SG_SEND_MSG *outMsg;
				SG_NAME_ATTACH_MSG *wrapMsg;

				outMsg=(SG_SEND_MSG *)outArea;
				wrapMsg=(SG_NAME_ATTACH_MSG *)&outMsg->dataMark;

				outMsg->token=SG_NAME_ATTACH;
				outMsg->nbytes=sizeof(int)+sizeof(SG_NAME_ATTACH_MSG);
				sprintf(wrapMsg->myName,"%.19s",&line[2]);

				sendToSocket(mySocket,
					outArea,
					outMsg->nbytes + 4,
					inArea,
					512);

				{
				SG_REPLY_MSG *inMsg;
				SG_NAME_ATTACH_MSG *replyMsg;
				
				inMsg=(SG_REPLY_MSG *)inArea;
				replyMsg=(SG_NAME_ATTACH_MSG *)&inMsg->dataMark;

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"myName=<%s> myPid=%d",
	replyMsg->myName,
	replyMsg->myPid);

				}
				}
				break;

			case 'q':
			case 'd':	// name detach
				{
				SG_NAME_DETACH_MSG *outMsg;

				outMsg=(SG_NAME_DETACH_MSG *)outArea;

				outMsg->token=SG_NAME_DETACH;
				outMsg->nbytes=0;

				sendToSocket(mySocket,
					outArea,
					outMsg->nbytes + 4,
					inArea,
					512);

				close(mySocket);
				x_it=1;
				}
				break;

			case 't':	// log it
				{
				SG_SEND_MSG *outMsg;
				SG_LOGIT_MSG *wrapMsg;

				outMsg=(SG_SEND_MSG *)outArea;
				wrapMsg=(SG_LOGIT_MSG *)&outMsg->dataMark;

				outMsg->token=SG_LOGIT;
				outMsg->nbytes=strlen(&line[2])+1+52;

				sprintf(wrapMsg->fileName,"myFile");
				sprintf(wrapMsg->funcName,"myFunc");
				wrapMsg->thisMask = 0x00000001;
				wrapMsg->logMask = 0xff;
				sprintf(&wrapMsg->dataMark,"%s", &line[2]);

printf("msg=<%s> nbytes=%d\n",
	&wrapMsg->dataMark,
	outMsg->nbytes);

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"msg=<%s> nbytes=%d",
	&wrapMsg->dataMark,
	outMsg->nbytes);

				sendToSocket( mySocket,
					outArea,
					outMsg->nbytes + 4,
					inArea,
					512);
				}
				break;

			case 'l':  // connect to logger
				{
				SG_SEND_MSG *outMsg;
				SG_IS_LOGGER_UP_MSG *wrapMsg;
				SG_REPLY_MSG *inMsg;
				SG_IS_LOGGER_UP_MSG *replyMsg;
				int rc;

				outMsg=(SG_SEND_MSG *)outArea;
				wrapMsg=(SG_IS_LOGGER_UP_MSG *)&outMsg->dataMark;

				outMsg->token=SG_IS_LOGGER_UP;
				outMsg->nbytes=sizeof(int)+sizeof(SG_IS_LOGGER_UP_MSG);
				sprintf(wrapMsg->loggerName,"LOGGER");

				sendToSocket(mySocket,
					outArea,
					outMsg->nbytes + 4,
					inArea,
					512);

				inMsg=(SG_REPLY_MSG *)inArea;
				replyMsg=(SG_IS_LOGGER_UP_MSG *)&inMsg->dataMark;
				rc=replyMsg->rc;
printf("%s:rc=%d\n",fn,rc);
fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"rc=%d",
	rc);
				}
				break;

			case 'w':  // name locate
				{
				SG_SEND_MSG *outMsg;
				SG_NAME_LOCATE_MSG *wrapMsg;
				SG_REPLY_MSG *inMsg;
				SG_NAME_LOCATE_MSG *replyMsg;

				outMsg=(SG_SEND_MSG *)outArea;
				wrapMsg=(SG_NAME_LOCATE_MSG *)&outMsg->dataMark;

				outMsg->token=SG_NAME_LOCATE;
				outMsg->nbytes=sizeof(int)+sizeof(SG_NAME_LOCATE_MSG);
				sprintf(wrapMsg->thisName,"%.19s",&line[2]);

				sendToSocket( mySocket,
					outArea,
					outMsg->nbytes + 4,
					inArea,
					512);

				inMsg=(SG_REPLY_MSG *)inArea;
				replyMsg=(SG_NAME_LOCATE_MSG *)&inMsg->dataMark;

				printf("%s: token=%d nbytes=%d rc=%d\n",
					fn,
					inMsg->token,
					inMsg->nbytes,
					replyMsg->rc);

				fcLogx(__FILE__, fn,
					globalMask,
					SOCKET_SENDER_MARK,
					"token=%d nbytes=%d rc=%d",
					inMsg->token,
					inMsg->nbytes,
					replyMsg->rc);

				toPid=replyMsg->rc;
				}
				break;

			case 's':  // send it
				{
				SG_SEND_MSG *outMsg;
				SURRO_TEST_MSG *wrapMsg;

				outMsg=(SG_SEND_MSG *)outArea;
				wrapMsg=(SURRO_TEST_MSG *)&outMsg->dataMark;

				outMsg->token=SG_SEND_IT;
				outMsg->toWhom=toPid;
				outMsg->nbytes=sizeof(int)+sizeof(SURRO_TEST_MSG);
				wrapMsg->token=SURRO_TEST;
				sprintf(wrapMsg->msg,"%.19s",&line[2]);

printf("%s: token=%d toPid=%d nbytes=%d wraptoken=%d msg=<%s>\n",
	fn,
	outMsg->token,
	outMsg->toWhom,
	outMsg->nbytes,
	wrapMsg->token,
	wrapMsg->msg
	);

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"token=%d toPid=%d nbytes=%d wraptoken=%d msg=<%s>",
	outMsg->token,
	outMsg->toWhom,
	outMsg->nbytes,
	wrapMsg->token,
	wrapMsg->msg
	);
				sendToSocket( mySocket,
					outArea,
					outMsg->nbytes + 4,
					inArea,
					512);

				{
				SG_REPLY_MSG *inMsg;
				SURRO_TEST_MSG *wrapMsg;
				
				inMsg=(SG_REPLY_MSG *)inArea;
				wrapMsg=(SURRO_TEST_MSG *)&inMsg->dataMark;
fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"token=%d wraptoken=%d msg=<%s>",
	inMsg->token,
	wrapMsg->token,
	wrapMsg->msg);
				}
				}
				break;

			case 'S':  // send no reply 
				{
				SG_SEND_MSG *outMsg;
				SURRO_TEST_MSG *wrapMsg;

				outMsg=(SG_SEND_MSG *)outArea;
				wrapMsg=(SURRO_TEST_MSG *)&outMsg->dataMark;

				outMsg->token=SG_SEND_NO_REPLY;
				outMsg->toWhom=toPid;
				outMsg->nbytes=sizeof(int)+sizeof(SURRO_TEST_MSG);
				wrapMsg->token=SURRO_TEST;
				sprintf(wrapMsg->msg,"%.19s",&line[2]);

printf("%s: token=%d toPid=%d nbytes=%d wraptoken=%d msg=<%s>\n",
	fn,
	outMsg->token,
	outMsg->toWhom,
	outMsg->nbytes,
	wrapMsg->token,
	wrapMsg->msg
	);

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"token=%d toPid=%d nbytes=%d wraptoken=%d msg=<%s>",
	outMsg->token,
	outMsg->toWhom,
	outMsg->nbytes,
	wrapMsg->token,
	wrapMsg->msg
	);
				relayToSocket( mySocket,
					outArea,
					outMsg->nbytes + 4);
				}
				break;

			case 'a':  // send ACK 
				{
				SG_ACK_MSG *outMsg;

				outMsg=(SG_ACK_MSG *)outArea;

				outMsg->token=SG_ACK;
				outMsg->nbytes=0;

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"Sending ACK token=%d nbytes=%d",
	outMsg->token,
	outMsg->nbytes
	);
				relayToSocket( mySocket,
					outArea,
					outMsg->nbytes + 4);
				}
				break;

			default:
				break;
			} // end switch

		printf("-> ");
		fflush(stdout);
		} // end if keyboard
	else
// Is it from socket?
	if(FD_ISSET(my_fds[1], &inset))  //  socket is ready
		{
		UINT16 *token;

		token=(UINT16 *)inArea;

		sender = receiveFromSocket(mySocket, inArea);

		if(sender == -1)
			x_it=1;
		else
		switch(*token)
			{
			case SG_RELAY_IT:
				{
				SG_RELAY_MSG *inMsg;
				SG_REPLY_MSG *rMsg;
				SURRO_TEST_MSG *myMsg;

				inMsg=(SG_RELAY_MSG *)inArea;
				myMsg=(SURRO_TEST_MSG *)&inMsg->dataMark;

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"SG_RELAY_IT: %d bytes from %d str=<%s>",
	inMsg->nbytes,
	inMsg->fromWhom,
	myMsg->msg);

				rMsg=(SG_REPLY_MSG *)outArea;
				rMsg->token=SG_REPLY_IT;
				rMsg->toWhom=inMsg->fromWhom;
				rMsg->nbytes=sizeof(int);

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"SG_REPLY_IT: toWhom=%d nbyte=%d sender=%d",
	rMsg->toWhom,
	rMsg->nbytes,
	sender);

				if(testMode == 0)
				relayToSocket(sender,outArea,rMsg->nbytes+4);
				}
				break;

			default:
				printf("%s: got unknown token=%d\n",
					fn, inMsg->token);
				break;
			}
		}
	} // end loop


printf("\n%s:done\n",fn);
close(mySocket);
name_detach();

return(1);

} // end socketSender

/*=========================================================
	initialize
=========================================================*/
void initialize(int argc, char **argv)
{
static char *fn="initialize";
int i;
char myName[20];
char logger_name[20];

globalMask=0xffffffff;
myPort=8000;
socketConnected=0;
logger_name[0]=0;
logger_ID=-1;
testMode=0;

/*=========================================================
	process command line arguments
=========================================================*/
for (i=1; i<=argc; ++i) 
	{	
	char *p = argv[i];

	if (p == NULL) continue;

	if (*p == '-')
		{             
		switch (*++p)
			{
			case 'n':
				if(*++p == 0) p++;
				sprintf(myName,"%.19s",p);

				if (name_attach(myName,NULL) == -1)
					{
					printf("%s: Can't attach as %s\n",fn,myName);
					exit(0);
					}
				break;

			case 'p':
				if(*++p == 0) p++;
				myPort = atoi(p);
				break;

			case 'l':
				if(*++p == 0) p++;
				sprintf(logger_name,"%.19s",p);
				break;

			case 't':
				testMode=1;
				break;	

			default:
				printf("Bad command line switch `%c'\n", *p);
			}
		}
	}	

// connect to logger
logger_ID = is_logger_upx(logger_name);

// arm the select routine
my_fds[0] = STDIN_FILENO;   // keyboard
//mySocket=connectSocket("localhost", myPort);
mySocket=connectSocket("icanprogram.ca", myPort);
my_fds[1] = mySocket;	// socket
FD_ZERO(&watchset);
FD_SET(my_fds[0], &watchset);
FD_SET(my_fds[1], &watchset);
for(i=0; i<2; i++)
	if(my_fds[i] > maxfd) maxfd = my_fds[i];

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"myPort: %d mySocket: %d",
	myPort,
	mySocket);

fcLogx(__FILE__, fn,
	globalMask,
	SOCKET_SENDER_MARK,
	"done");

}// end initialize
