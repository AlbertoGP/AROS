/*
    Copyright � 1995-2017, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Bitmap class for SM502 Gfx Hidd.
    Lang: English.
*/

#define __OOP_NOATTRBASES__

#define DEBUG 1
#include <aros/debug.h>
#include <proto/oop.h>
#include <proto/utility.h>
#include <assert.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <graphics/rastport.h>
#include <graphics/gfx.h>
#include <hidd/hidd.h>
#include <hidd/gfx.h>
#include <oop/oop.h>
#include <aros/symbolsets.h>
#include <aros/debug.h>

#include <string.h>

#include "sagagfx_bitmap.h"
#include "sagagfx_hidd.h"

#include LC_LIBDEFS_FILE


static UWORD getScreenMode( UWORD width, UWORD height )
{
	if( width == 320 && height == 200 )
		return SAGAV4_VIDEO_RES_320x200;
	else if( width == 320 && height == 240 )
		return SAGAV4_VIDEO_RES_320x240;
	else if( width == 320 && height == 200 )
		return SAGAV4_VIDEO_RES_320x256;
	else if( width == 640 && height == 400 )
		return SAGAV4_VIDEO_RES_640x400;
	else if( width == 640 && height == 480 )
		return SAGAV4_VIDEO_RES_640x480;
	else if( width == 640 && height == 512 )
		return SAGAV4_VIDEO_RES_640x512;
	else if( width == 960 && height == 540 )
		return SAGAV4_VIDEO_RES_960x540;

	return 0x0000;
}

/*********** BitMap::New() *************************************/
OOP_Object *METHOD(SAGABitMap, Root, New)
{
    bug("[SAGABitMap] BitMap::New()\n");

    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);
    if (o)
    {
        OOP_MethodID                disp_mid;
        struct SAGAGfxBitmapData *  data;
        HIDDT_ModeID                modeid;
        OOP_Object                  *sync, *pf;
        struct TagItem attrs[] = {
            { aHidd_ChunkyBM_Buffer, 0 },
            { TAG_DONE, 0UL }
        };

        data = OOP_INST_DATA(cl, o);

        /* Get attr values */
        bug("[SAGABitMap] %s: Getting attributes\n", __FUNCTION__ );
        OOP_GetAttr(o, aHidd_BitMap_GfxHidd , (APTR)&data->gfxhidd);
        OOP_GetAttr(o, aHidd_BitMap_PixFmt  , (APTR)&data->pixfmtobj);
        OOP_GetAttr(o, aHidd_BitMap_ModeID  , &modeid);

        bug("[SAGABitMap] %s: Getting sync and pf values\n", __FUNCTION__ );
        HIDD_Gfx_GetMode(data->gfxhidd, modeid, &sync, &pf);

        bug("[SAGABitMap] %s: getting bitmap values\n", __FUNCTION__ );
        data->width         = OOP_GET(o, aHidd_BitMap_Width);
        data->height        = OOP_GET(o, aHidd_BitMap_Height);
        data->bytesperline  = OOP_GET(o, aHidd_BitMap_BytesPerRow);
        data->bytesperpix   = OOP_GET(data->pixfmtobj, aHidd_PixFmt_BytesPerPixel);
        data->bitsperpix    = OOP_GET(data->pixfmtobj, aHidd_PixFmt_BitsPerPixel);

        bug("[SAGABitMap] %s: Selected width: %ld\n", __FUNCTION__, data->width );
        bug("[SAGABitMap] %s: Selected height: %ld\n", __FUNCTION__, data->height );
        bug("[SAGABitMap] %s: Bytes per pixel: %d\n", __FUNCTION__, data->bytesperpix );
        bug("[SAGABitMap] %s: Bytes per line: %ld\n", __FUNCTION__, data->bytesperline );
        //ULONG bufferSize = 64 + data->bytesperline * (data->height + 10);
        ULONG bufferSize = 64 + ( data->width * ( data->height +10 ) * data->bytesperpix );
        bug("[SAGABitMap] %s: allocating video memory; %ld bytes\n", __FUNCTION__,  bufferSize );
        data->VideoBuffer = AllocVecPooled(XSD(cl)->mempool, bufferSize );
        data->VideoData = (UBYTE *)(((IPTR)data->VideoBuffer + 31) & ~31);

        attrs[0].ti_Data = (IPTR)data->VideoData;
        OOP_SetAttrs(o, attrs);

        bug("[SAGABitMap] %s: Get video display height and width\n", __FUNCTION__ );

        //Not relevant anymore
        data->hwregs.hpixel     = OOP_GET(sync, aHidd_Sync_HDisp);
        data->hwregs.hsstart    = OOP_GET(sync, aHidd_Sync_HSyncStart);
        data->hwregs.hsstop     = OOP_GET(sync, aHidd_Sync_HSyncEnd);
        data->hwregs.htotal     = OOP_GET(sync, aHidd_Sync_HTotal);
        data->hwregs.vpixel     = OOP_GET(sync, aHidd_Sync_VDisp);
        data->hwregs.vsstart    = OOP_GET(sync, aHidd_Sync_VSyncStart);
        data->hwregs.vsstop     = OOP_GET(sync, aHidd_Sync_VSyncEnd);
        data->hwregs.vtotal     = OOP_GET(sync, aHidd_Sync_VTotal);
        data->hwregs.hvsync     = OOP_GET(sync, aHidd_Sync_Flags);
        data->hwregs.pixelclock = OOP_GET(sync, aHidd_Sync_PixelClock);

        bug("[SAGABitMap] %s: Screenmode resolution: %dx%d\n", __FUNCTION__,  data->hwregs.hpixel, data->hwregs.vpixel );

        data->resolutionMode = getScreenMode( data->hwregs.hpixel, data->hwregs.vpixel );
        bug("[SAGABitMap] %s: Screenmode retreived: 0x%04x\n", __FUNCTION__, data->resolutionMode );

        bug("[SAGABitMap] Bitmap %ld x % ld, %u bytes per pixel, %u bytes per line\n",
                data->width, data->height, data->bytesperpix, data->bytesperline);
        bug("[SAGABitMap] Video data at 0x%p (%u bytes)\n", data->VideoData, data->bytesperline * data->height);

        bug("[SAGABitMap] %s: Set pixel mode\n", __FUNCTION__ );

        //For simple chunky without a colour palette.
        switch (data->bitsperpix)
        {
            case 32:
            	bug("[SAGABitMap] %s: 32 Bit Colour Mode selected\n", __FUNCTION__ );
                data->hwregs.video_mode = SAGA_VIDEO_FORMAT_RGB32;
                break;
            case 24:
            	bug("[SAGABitMap] %s: 24 Bit Colour Mode selected\n", __FUNCTION__ );
                data->hwregs.video_mode = SAGA_VIDEO_FORMAT_RGB24;
                break;
            case 16:
            	bug("[SAGABitMap] %s: 16 Bit Colour Mode selected\n", __FUNCTION__ );
                data->hwregs.video_mode = SAGA_VIDEO_FORMAT_RGB16;
                break;
            case 15:
            	bug("[SAGABitMap] %s: 15 Bit Colour Mode selected\n", __FUNCTION__ );
                data->hwregs.video_mode = SAGA_VIDEO_FORMAT_RGB15;
                break;
        }

        //In case there is a colour palette involved, we should activate 8bit mode
        if (OOP_GET(data->pixfmtobj, aHidd_PixFmt_ColorModel) == vHidd_ColorModel_Palette)
        {
        	D(bug("[SAGABitMap] %s: 8 Bit CLUT Mode selected\n", __FUNCTION__ ));
            data->CLUT = AllocVecPooled(XSD(cl)->mempool, 256 * sizeof(ULONG));
            data->hwregs.video_mode = SAGA_VIDEO_FORMAT_CLUT8;

            D(bug("[SAGABitMap] CLUT at %p\n", data->CLUT));

            if (!data->CLUT)
            {
            	bug("[SAGABitMap] %s: A Colour map wasn't supplied.\n", __FUNCTION__ );
                disp_mid = OOP_GetMethodID(IID_Root, moRoot_Dispose);
                OOP_CoerceMethod(cl, o, (OOP_Msg)&disp_mid);
                return NULL;
            }
        }

        //Set the screen resolution
        data->hwregs.video_mode  |= data->resolutionMode;
        bug("[SAGABitMap] %s: Setting screen resolution ( 0x%04x ) into register 0x%08lx\n", __FUNCTION__, data->hwregs.video_mode, SAGA_VIDEO_MODE );
        bug("[SAGABitMap] %s: Setting Framebuffer ( 0x%08lx ) into register 0x%08lx\n", __FUNCTION__, data->VideoData, SAGA_VIDEO_BPLPTR );

    }

    D(bug("[SAGABitMap] Returning object %p\n", o));

    return o;
}

/**********  Bitmap::Dispose()  ***********************************/
VOID METHOD(SAGABitMap, Root, Dispose)
{
    struct SAGAGfxBitmapData *data = OOP_INST_DATA(cl, o);

    D(bug("[SAGABitMap] Dispose(0x%p)\n", o));

    if (data->CLUT)
        FreeVecPooled(XSD(cl)->mempool, data->CLUT);

    if (data->VideoBuffer)
        FreeVecPooled(XSD(cl)->mempool, data->VideoBuffer);

    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

/*** BitMap::Get() *******************************************/

VOID METHOD(SAGABitMap, Root, Get)
{
    struct SAGAGfxBitmapData *data = OOP_INST_DATA(cl, o);
    ULONG idx;

    if (IS_BM_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
        case aoHidd_BitMap_Visible:
            *msg->storage = data->disp;
            return;

        case aoHidd_BitMap_LeftEdge:
            *msg->storage = data->xoffset;
            return;

        case aoHidd_BitMap_TopEdge:
            *msg->storage = data->yoffset;
            return;
        }
    }
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

BOOL METHOD(SAGABitMap, Hidd_BitMap, SetColors)
{
    struct SAGAGfxBitmapData *data = OOP_INST_DATA(cl, o);
    ULONG xc_i, col_i;
    UBYTE p_shift;
    UWORD red, green, blue;

    D(bug("[SAGABitMap] SetColors(%u, %u)\n", msg->firstColor, msg->numColors));

    if (!OOP_DoSuperMethod(cl, o, (OOP_Msg)msg)) {
        D(bug("[SAGAGfx:BitMap] DoSuperMethod() failed\n"));
        return FALSE;
    }

    if ((msg->firstColor + msg->numColors) > 256)
        return FALSE;

    if (data->CLUT)
    {
        for (xc_i = msg->firstColor, col_i = 0;
             col_i < msg->numColors;
             xc_i ++, col_i ++)
        {
            red   = msg->colors[col_i].red   >> 8;
            green = msg->colors[col_i].green >> 8;
            blue  = msg->colors[col_i].blue  >> 8;

            D(bug("[SAGABitMap] clut %d, r=%d, g=%d, b=%d\n", xc_i, red, green, blue));

            /* Update DAC registers */
            data->CLUT[xc_i] = (red << 16) | (green << 8) | blue;
        }

        /* Upload palette to the DAC if the current bitmap is on display */
        if (XSD(cl)->visible == o)
            SAGA_LoadCLUT(data->CLUT, msg->firstColor, msg->numColors);
    }
    return TRUE;
}
