#ifndef _XMODEM_H_INCLUDED_
#define _XMODEM_H_INCLUDED_

#include "kernel/defines.h"


/*! XMODEMでの送信制御 */
BOOL send_xmodem(UINT8 *bufp, UINT32 size);

/*! XMODEMでの受信制御 */
INT32 recv_xmodem(UINT8 *buf);

/*! XMODEM受信時でのホストとのタイミング調整 */
void adjust_timing_xmodem(void);

#endif
