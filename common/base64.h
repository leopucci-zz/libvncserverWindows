/* $Id: base64.h,v 1.6 2003/08/29 16:59:52 mouring Exp $ */

/*
 * Copyright (c) 1996 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * Portions Copyright (c) 1995 by International Business Machines, Inc.
 *
 * International Business Machines, Inc. (hereinafter called IBM) grants
 * permission under its copyrights to use, copy, modify, and distribute this
 * Software with or without fee, provided that the above copyright notice and
 * all paragraphs of this notice appear in all copies, and that the name of IBM
 * not be used in connection with the marketing of any product incorporating
 * the Software or modifications thereof, without specific, written prior
 * permission.
 *
 * To the extent it has a right to do so, IBM grants an immunity from suit
 * under its patents, if any, for the use, sale or manufacture of products to
 * the extent that such products are used for performing Domain Name System
 * dynamic updates in TCP/IP networks by means of the Software.  No immunity is
 * granted for any product per se or for any other function of any product.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", AND IBM DISCLAIMS ALL WARRANTIES,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE, EVEN
 * IF IBM IS APPRISED OF THE POSSIBILITY OF SUCH DAMAGES.
 */

#ifndef _BSD_BASE64_H
#define _BSD_BASE64_H

#include <stdint.h>
typedef unsigned char u_char;
#ifndef HAVE___B64_NTOP
# ifndef HAVE_B64_NTOP
int b64_ntop(u_char const *src, size_t srclength, char *target, 
    size_t targsize);
# endif /* !HAVE_B64_NTOP */
# define __b64_ntop(a,b,c,d) b64_ntop(a,b,c,d)
#endif /* HAVE___B64_NTOP */

#ifndef HAVE___B64_PTON
# ifndef HAVE_B64_PTON
int b64_pton(char const *src, u_char *target, size_t targsize);
# endif /* !HAVE_B64_PTON */
# define __b64_pton(a,b,c) b64_pton(a,b,c)
#endif /* HAVE___B64_PTON */

static uint16_t bswap_16(uint16_t x)
{
	uint16_t __x = (x);
	return ((uint16_t)(
		(((uint16_t)(__x) & (uint16_t)0x00ffU) << 8) | 
		(((uint16_t)(__x) & (uint16_t)0xff00U) >> 8) ));
}

static uint32_t bswap_32(uint32_t x) 
{
	uint32_t __x = (x);
	return ((uint32_t)( 
		(((uint32_t)(__x) & (uint32_t)0x000000ffUL) << 24) | 
		(((uint32_t)(__x) & (uint32_t)0x0000ff00UL) <<  8) | 
		(((uint32_t)(__x) & (uint32_t)0x00ff0000UL) >>  8) | 
		(((uint32_t)(__x) & (uint32_t)0xff000000UL) >> 24) ));
}

static uint64_t bswap_64(uint64_t x)
{
	uint64_t __x = (x); 
	return ((uint64_t)(
		(uint64_t)(((uint64_t)(__x) & (uint64_t)0x00000000000000ffULL) << 56) |
		(uint64_t)(((uint64_t)(__x) & (uint64_t)0x000000000000ff00ULL) << 40) |
		(uint64_t)(((uint64_t)(__x) & (uint64_t)0x0000000000ff0000ULL) << 24) |
		(uint64_t)(((uint64_t)(__x) & (uint64_t)0x00000000ff000000ULL) <<  8) |
	        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x000000ff00000000ULL) >>  8) |
		(uint64_t)(((uint64_t)(__x) & (uint64_t)0x0000ff0000000000ULL) >> 24) |
		(uint64_t)(((uint64_t)(__x) & (uint64_t)0x00ff000000000000ULL) >> 40) |
		(uint64_t)(((uint64_t)(__x) & (uint64_t)0xff00000000000000ULL) >> 56) ));
}
#endif /* _BSD_BASE64_H */
