/*
	checksum.c

	MD4-based checksum utility functions

	Copyright (C) 2000       Jeff Teunissen <d2deek@pmail.net>

	Author: Jeff Teunissen	<d2deek@pmail.net>
	Date: 01 Jan 2000

	Copyright (C) 1996-1997  Id Software, Inc.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to:

		Free Software Foundation, Inc.
		59 Temple Place - Suite 330
		Boston, MA  02111-1307, USA

*/
static const char rcsid[] = 
	"$Id: checksum.c,v 1.1 2002/04/02 06:23:02 jaq Exp $";

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#include "mdfour.h"


unsigned int
Com_BlockChecksum (const void *buffer, int length)
{
	int				digest[4];
	unsigned int	val;

	mdfour ((unsigned char *) digest, (unsigned char *) buffer, length);

	val = digest[0] ^ digest[1] ^ digest[2] ^ digest[3];

	return val;
}

void
Com_BlockFullChecksum (const void *buffer, int len, unsigned char *outbuf)
{
	mdfour (outbuf, (unsigned char *) buffer, len);
}
