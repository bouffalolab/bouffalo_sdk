#ifndef __BL_SYS_H__
#define __BL_SYS_H__

#if defined(BL702)
#include "bl702_sys.h"
#elif defined(BL602)
#include "bl602_sys.h"
#elif defined(BL702L)
#include "bl702l_sys.h"
#elif defined(BL616)
#include "bl616_sys.h"
#elif defined(BL616CL)
#include "bl616cl_sys.h"
#elif defined(BL618DG)
#include "bl618dg_sys.h"
#endif

#endif
