/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <kprint.h>

#define V_NON 0 /* No log */
#define V_CRT 1 /* Log critical */
#define V_ERR 2 /* Log errors */
#define V_WRN 3 /* Log warnings */
#define V_LOG 4 /* Log normal behaviour */
#define V_DBG 5 /* Log debug */

#ifndef VERB
#define VERB V_LOG
#endif

#define log_macro(v, pfx, ...) \
	do {if (VERB >= v ) { \
		kprint(pfx " %9d ", (uint)k_ticks()); \
		kprint(__VA_ARGS__);} \
	} while (0)

#define crt(...) log_macro(V_CRT, "CRT", __VA_ARGS__)
#define err(...) log_macro(V_ERR, "ERR", __VA_ARGS__)
#define wrn(...) log_macro(V_WRN, "WRN", __VA_ARGS__)
#define log(...) log_macro(V_LOG, "LOG", __VA_ARGS__)
#define dbg(...) log_macro(V_DBG, "DBG", __VA_ARGS__)

#endif /* _LOG_H_ */
