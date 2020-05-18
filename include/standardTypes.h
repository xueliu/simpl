/*************************************************************

	FILE:			standardTypes.h
	VERSION:		95 Mar 12

	DESCRIPTION:	This header defines all ints, char etc in logical
	manner.

	AUTHOR:			R.D. Findlay

-----------------------------------------------------------------------
    Copyright (C) 1998, 2007 FCSoftware Inc. 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    If you discover a bug or add an enhancement contact us on the
    SIMPL project mailing list. 

-----------------------------------------------------------------------

	Revision history:
	=======================================================
$Log: standardTypes.h,v $
Revision 1.3  2009/06/30 20:48:16  bobfcsoft
commented out BOOL

Revision 1.2  2007/07/24 20:18:45  bobfcsoft
new contact info

Revision 1.1.1.1  2005/03/27 11:50:35  paul_c
Initial import

Revision 1.4  2000/10/04 01:47:23  root
LGPL'd

Revision 1.3  1999/11/04 15:30:11  root
redefined INT32 and UINT32 to get rid of warnings on some systems

Revision 1.2  1999/02/04 15:06:51  root
cleanup

Revision 1.1  1998/08/20 14:36:20  root
Initial revision

 * Revision 1.1  1997/03/23  18:23:04  root
 * Initial revision
 *
	=======================================================

*************************************************************/
#ifndef STANDARD_TYPES_DEF
#define STANDARD_TYPES_DEF

typedef signed char				INT8;
//typedef char					BOOL;
typedef unsigned char				UINT8;
typedef short int				INT16;
typedef unsigned short int 			UINT16;
typedef int					INT32;
typedef unsigned int				UINT32;
#endif
