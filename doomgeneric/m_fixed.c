//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Fixed point implementation.
//



#include "stdlib.h"

#include "doomtype.h"
#include "i_system.h"

#include "m_fixed.h"




// Fixme. __USE_C_FIXED__ or something.

fixed_t
FixedMul
( fixed_t	a,
  fixed_t	b )
{
    return ((int64_t) a * (int64_t) b) >> FRACBITS;
}



//
// FixedDiv, C version.
//

static inline uint32_t uabs32(int32_t x) {
    uint32_t ux = (uint32_t)x;
    if (x < 0) ux = (~ux) + 1;   // two's complement
    return ux;
}

fixed_t FixedDiv(fixed_t a, fixed_t b)
{
    // mismo early-exit que el original, también cubre b == 0
    uint32_t ua = uabs32(a);
    uint32_t ub = uabs32(b);
    if (ub == 0 || (ua >> 14) >= ub) {
        return ((a ^ b) < 0) ? INT_MIN : INT_MAX;
    }

    int neg = ((a ^ b) < 0);

    // parte entera (32-bit idiv)
    uint32_t q  = ua / ub;
    uint32_t r  = ua % ub;

    // parte fraccionaria: 16 bits con división binaria (sin / de 64-bit)
    uint32_t frac = 0;
    for (int i = 0; i < 16; i++) {
        r <<= 1;
        frac <<= 1;
        if (r >= ub) { r -= ub; frac |= 1; }
    }

    uint32_t ures = (q << 16) | (frac & 0xFFFF);
    return neg ? -(int32_t)ures : (int32_t)ures;
}