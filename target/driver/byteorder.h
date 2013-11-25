#ifndef __ASM_ARM_BYTEORDER_H
#define __ASM_ARM_BYTEORDER_H


#include "types.h"

#if !defined(__STRICT_ANSI__) || defined(__KERNEL__)
#  define __BYTEORDER_HAS_U64__
#  define __SWAB_64_THRU_32__
#endif

#if 0
#ifdef __ARMEB__ // beagleboradでは、このマクロは定義しない
#include "big_endian.h"
#else
#include "linux/byteorder/little_endian.h"
#endif

#endif

#endif
