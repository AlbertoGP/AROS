/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc:
    Lang: english
*/

#include <aros/libcall.h>
#include <proto/mathieeedoubbas.h>
#include <proto/exec.h>
#include <exec/types.h>
#include "mathieeedoubtrans_intern.h"

/*****************************************************************************

    NAME */
	
      AROS_LHQUAD1(double, IEEEDPCos,

/*  SYNOPSIS */

      AROS_LHAQUAD(double, y, D0, D1),

/*  LOCATION */

      struct MathIeeeDoubTransBase *, MathIeeeDoubTransBase, 7, MathIeeeDoubTrans)

/*  FUNCTION

      Calculate the cosine of a given IEEE double precision number in radians

    INPUTS

      y - IEEE double precision floating point number

    RESULT

      IEEE double precision floating point number


      flags:
        zero     : result is zero
        negative : result is negative
        overflow : 0

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS
      cos (x) = sin (x+ pi/2);

    HISTORY

******************************************************************************/
{
  LONG tmp;
  QUAD z,Res, ysquared, yabs, Qtmp;
  AND64C(yabs, y, (IEEEDPMantisse_Mask_Hi | IEEEDPExponent_Mask_Hi),
                  (IEEEDPMantisse_Mask_Lo | IEEEDPExponent_Mask_Lo));


  if (is_eqC(yabs, IEEEDPPInfty_Hi, IEEEDPPInfty_Lo))
  {
		SetSR(Overflow_Bit, Zero_Bit | Negative_Bit | Overflow_Bit);
		Set_Value64C(Res, IEEEDPNAN_Hi, IEEEDPNAN_Lo);
		return Res;
  }

  z = IEEEDPFloor(IEEEDPDiv(yabs, pi));
  Qtmp  = IEEEDPMul(z,pi);
  OR64QC(tmp, IEEEDPSign_Mask_Hi, IEEEDPSign_Mask_Lo) /* Qtmp=-Qtmp */
  yabs = IEEEDPAdd(yabs, Qtmp);
  if (is_greaterC(yabs, pio2_Hi, pio2_Lo))
  {
    OR64QC(Qtmp, IEEEDPSign_Mask_Hi, IEEEDPSign_Mask_Lo) /* Qtmp=-Qtmp */
    yabs  =IEEEDPAdd(pi, yabs);
    tmp = TRUE;
  }
  else
    tmp = FALSE;

  ysquared = IEEEDPMul(yabs,yabs);
  Res = IEEEDPAdd(cosf1,
         IEEEDPMul(ysquared,
          IEEEDPAdd(cosf2,
           IEEEDPMul(ysquared,
            IEEEDPAdd(cosf3,
             IEEEDPMul(ysquared,
              IEEEDPAdd(cosf4,
               IEEEDPMul(ysquared,
                IEEEDPAdd(cosf5,
                 IEEEDPMul(ysquared, 
                  IEEEDPAdd(cosf6,
                   IEEEDPMul(ysquared,
                    IEEEDPAdd(cosf7,
                     IEEEDPMul(ysquared, cosf8)
                    )
                   )
                  )
                 )
                )
               )
              )
             )
            )
           )
          )
         )
        );

  if (is_eqC(Res, 0x0, 0x0))
  {
  	SetSR(Zero_Bit, Zero_Bit | Negative_Bit | Overflow_Bit);
  	Set_Value64C(Res, 0x0, 0x0);
  	return Res;
  }

  if (TRUE == intern_IEEEDPisodd(z))
    XOR64QC(Res, IEEEDPSign_Mask_Hi, IEEEDPSign_Mask_Lo);

  if (TRUE == tmp)
    XOR64QC(Res, IEEEDPSign_Mask_Hi, IEEEDPSign_Mask_Lo);

  if (is_lessSC(Res, 0x0, 0x0))
  	SetSR(Negative_Bit, Zero_Bit | Negative_Bit | Overflow_Bit);

  return Res;
} /* IEEEDPCos */
