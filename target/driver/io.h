#ifndef _IO_H_INCLUDED_
#define _IO_H_INCLUDED_

#define __arch_getl(a)			(*(volatile unsigned int *)(a))
#define __arch_putl(v,a)		(*(volatile unsigned int *)(a) = (v))

#define writel(v,a)			__arch_putl(v,a)
#define readl(a)			__arch_getl(a)

#endif
