#ifndef RFB_CLIENT_PRIVATE_H
#define RFB_CLIENT_PRIVATE_H

#include <rfb/rfbconfig.h>
#include <rfb/rfbclient.h>
#ifdef LIBVNCSERVER_HAVE_LIBZ
#include <zlib.h>
#endif


#define RFB_ALIGN_LEN 32
#define RFB_ALIGN_SIZE(size) (size+(RFB_ALIGN_LEN-1) & ~(RFB_ALIGN_LEN-1))
#define RFB_CLIENT_PRIV(rfbClient) (((char*)rfbClient) + RFB_ALIGN_SIZE(sizeof(rfbClient)))

struct rfbClientPrivate { 
#ifdef LIBVNCSERVER_HAVE_LIBZ
	/*
	 * Variables for the ``tight'' encoding implementation.
	 */
	z_stream decompStream;
	rfbBool decompStreamInited;

	/* Four independent compression streams for zlib library. */
	z_stream zlibStream[4];
	rfbBool zlibStreamActive[4];

	/** Separate buffer for compressed data. */
#define ZLIB_BUFFER_SIZE 30000
	char zlib_buffer[ZLIB_BUFFER_SIZE];

	/* Filter stuff. Should be initialized by filter initialization code. */
	rfbBool cutZeros;
	int rectWidth, rectColors;
	char tightPalette[256*4];
	uint8_t tightPrevRow[2048*3*sizeof(uint16_t)];

#ifdef LIBVNCSERVER_HAVE_LIBJPEG
	/** JPEG decoder state. */
	rfbBool jpegError;

	struct jpeg_source_mgr* jpegSrcManager;
	void* jpegBufferPtr;
	size_t jpegBufferLen;

#endif
#endif
	rfbClient *pClient;
};

static rfbClient *rfbClientAlloc()
{
	rfbClient* cl;
	struct rfbClientPrivate *priv;
	int size = RFB_ALIGN_SIZE(sizeof(rfbClient))+sizeof(struct rfbClientPrivate);

	cl = (rfbClient*)calloc(size,1);
	if (!cl)
		return cl;

	priv = (struct rfbClientPrivate*)(RFB_CLIENT_PRIV(cl));
	priv->pClient = cl;
	return cl;
}
#endif


