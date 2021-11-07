#ifndef SHELF_H
#define SHELF_H
#include "config.h"
#include <stdint.h>

#define D_SIZE (D_HEIGHT * D_WIDTH)
#define D_BYTES ((D_SIZE + 0x07) >> 3)

typedef struct drawer_s {
    uint8_t slots[D_BYTES];
} Drawer;

// call in setup
void drawer_init(void);

void drawer_clearall(void);
void drawer_show(Drawer* s);
int drawer_isset(Drawer* s, uint8_t y, uint8_t x);
int drawer_set(Drawer* s, uint8_t y, uint8_t x);
int drawer_clear(Drawer* s, uint8_t y, uint8_t x);
void drawer_clearstate(Drawer* s);
#endif  // shelf.h ends here
