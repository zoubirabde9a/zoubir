#if !defined(INTRINSICS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Zoubir $
   ======================================================================== */

#define INTRINSICS_H
#include <math.h>

inline u32
CeilFloatToUInt32(float A)
{
    u32 Result = (u32)ceilf(A);
    return Result;
}

#if COMPILER_MSVC

inline u32
AtomicCompareExchangeU32(u32 volatile *Value,
                                u32 ExpectedValue,
                                u32 NewValue)
{
    u32 Result =
        InterlockedCompareExchange((long volatile *)Value,
                                   Expected, NewValue);
    return Result;
}

#endif

inline float
Sin(float Angle)
{
    float Result = sinf(Angle);
    return Result;
}
#endif
