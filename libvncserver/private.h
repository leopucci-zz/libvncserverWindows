#ifndef RFB_PRIVATE_H
#define RFB_PRIVATE_H

/* from cursor.c */

void rfbShowCursor(rfbClientPtr cl);
void rfbHideCursor(rfbClientPtr cl);
void rfbRedrawAfterHideCursor(rfbClientPtr cl,sraRegionPtr updateRegion);

/* from main.c */

rfbClientPtr rfbClientIteratorHead(rfbClientIteratorPtr i);

/* from tight.c */

#ifdef LIBVNCSERVER_HAVE_LIBZ
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
extern void rfbTightCleanup(rfbScreenInfoPtr screen);
#endif

/* from zlib.c */
extern void rfbZlibCleanup(rfbScreenInfoPtr screen);

/* from zrle.c */
void rfbFreeZrleData(rfbClientPtr cl);

#endif


/* from ultra.c */

extern void rfbFreeUltraData(rfbClientPtr cl);

#ifdef LIBVNCSERVER_HAVE_LIBZ
#include <zlib.h>
#endif
#include <rfb/rfb.h>

#define RFB_ALIGN_LEN 32
#define RFB_ALIGN_SIZE(size) (size+(RFB_ALIGN_LEN-1) & ~(RFB_ALIGN_LEN-1))
#define RFB_CLIENT_REC_PRIV(rfbClient) (((char*)rfbClient) + RFB_ALIGN_SIZE(sizeof(rfbClientRec)))

struct rfbClientRecPrivate { 
#ifdef LIBVNCSERVER_HAVE_LIBZ
    /* zlib encoding -- necessary compression state info per client */

    struct z_stream_s compStream;
    rfbBool compStreamInited;
    uint32_t zlibCompressLevel;
    void* zrleData;
    int zywrleLevel;
    int zywrleBuf[rfbZRLETileWidth * rfbZRLETileHeight];
#endif
#if defined(LIBVNCSERVER_HAVE_LIBZ) || defined(LIBVNCSERVER_HAVE_LIBPNG)
    /** the quality level is also used by ZYWRLE and TightPng */
    int tightQualityLevel;

#ifdef LIBVNCSERVER_HAVE_LIBJPEG
    /* tight encoding -- preserve zlib streams' state for each client */
    z_stream zsStruct[4];
    rfbBool zsActive[4];
    int zsLevel[4];
    int tightCompressLevel;
#endif
#endif

#if defined(LIBVNCSERVER_HAVE_LIBZ) || defined(LIBVNCSERVER_HAVE_LIBPNG)
    uint32_t tightEncoding;  /* rfbEncodingTight or rfbEncodingTightPng */
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
    /* TurboVNC Encoding support (extends TightVNC) */
    int turboSubsampLevel;
    int turboQualityLevel;  /* 1-100 scale */
#endif
#endif
	// point back to client
	rfbClientPtr pClient;
};

static rfbClientPtr rfbClientRecAlloc()
{
	rfbClientPtr cl;
	struct rfbClientRecPrivate *priv;
	int size = RFB_ALIGN_SIZE(sizeof(rfbClientRec))+sizeof(struct rfbClientRecPrivate);
    cl = (rfbClientPtr)calloc(size,1);
	if (!cl)
		return cl;
	priv = (struct rfbClientRecPrivate*)(RFB_CLIENT_REC_PRIV(cl));
	priv->pClient = cl;
	return cl;
}

#endif

