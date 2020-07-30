/*
    Copyright � 1995-2020, The AROS Development Team. All rights reserved.
     $Id$

    Desc: SAGA Gfx Hidd for AROS (V4)
    Lang: english
*/

#define DEBUG 1

#include <aros/debug.h>

#include <exec/exec.h>
#include <devices/inputevent.h>
#include <proto/exec.h>
#include <proto/input.h>

#include "sagagfx_hw.h"

void SAGA_SetPLL_V4(ULONG clock)
{
    D( bug( "[SAGA] SAGA_SetPLL_V4(%d)\n", clock ) );
}

void SAGA_LoadCLUT_V4(ULONG *palette, UWORD startIndex, UWORD count)
{
    if ( palette )
    {
        if ( startIndex > 255 )
            return;

        if ( startIndex + count > 256 )
            count = 256 - startIndex;

        for ( int i = 0; i < count; i++ )
        {
            WRITE32(SAGA_VIDEO_CLUT_V4, ( ( startIndex + i ) << 24 ) | palette[ startIndex + i ] );
        }
    }
}


/* END OF FILE */
