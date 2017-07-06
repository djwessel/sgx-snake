#ifndef __SNAKE_H__
#define __SNAKE_H__

#define GOLD      '$'
#define CACTUS    '*'

#define MAXROW    22
#define MAXCOL    78

#define START_LEN (4 + level)
#define START_COL (MAXCOL / 2 - START_LEN)
#define START_ROW ((MAXROW + 1) / 2)

#define DEFAULT_DELAY 200000
#define DEFAULT_KEYS  {'o', 'p', 'a', 'z', 'f', 'j', 'q'}

#include <assert.h>

#if defined(__cplusplus)
extern "C" {
#endif

void printf(const char *fmt, ...);
int rand(void);

#if defined(__cplusplus)
}
#endif

#endif /* __SNAKE_H__ */

