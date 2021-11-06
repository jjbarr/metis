#ifndef SHELF_H
#define SHELF_H
#include <stdint.h>
#include "config.h"

#define D_SIZE (SH_HEIGHT*SH_WIDTH)
#define D_BYTES ((SH_SIZE+0x07)>>3)

typedef struct drawer_s {
    uint8_t slots[D_BYTES];
} Drawer;

#ifdef __cplusplus
extern "C" {
#endif
void shelf_init(void);
void shelf_clearall(void);
void shelf_show(Drawer *s);
int shelf_isset(Drawer *s, uint8_t y, uint8_t x);
int shelf_set(Drawer *s, uint8_t y, uint8_t x);
int shelf_clear(Drawer *s, uint8_t y, uint8_t x);
void shelf_clearstate(Drawer *s);
#ifdef __cplusplus
}    
#endif
#endif//shelf.h ends here
