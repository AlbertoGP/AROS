#ifndef SAGAGFX_HW_V4_H
#define SAGAGFX_HW_V4_H

/*
    Copyright © 1995-2020, The AROS Development Team. All rights reserved.
    $Id$

    Desc: SAGAGfx hardware header.
    Lang: English.
*/

#include <aros/debug.h>
#include <exec/types.h>

void SAGA_SetPLL_V4(ULONG clock);
void SAGA_LoadCLUT_V4(ULONG *palette, UWORD startIndex, UWORD count);

#endif /* SAGAGFX_HW_V4_H */
