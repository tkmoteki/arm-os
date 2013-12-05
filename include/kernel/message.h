#ifndef _MESSAGE_H_INCLUDED_
#define _MESSAGE_H_INCLUDED_

#include "include/c/lib.h"


/*! kernel message */

#ifdef KERNEL_MSG
/* kernel message */
#define KERNEL_OUTVLE(kernelvalue, kernelcolumn) (putxval(kernelvalue, kernelcolumn))
/* kernel value */
#define KERNEL_OUTMSG(kernelmsg) (puts(kernelmsg))
#else
#define KERNEL_OUTVLE(kernelvalue, kernelcolumn)
#define KERNEL_OUTMSG(kernelmsg)
#endif


#endif
