#ifndef SHELF_H
#define SHELF_H
#include <stdint.h>
#include "config.h"

#define SH_SIZE (SH_HEIGHT*SH_WIDTH)
#define SH_BYTES ((SH_SIZE+0x07)>>3)

typedef struct shelf_state_s {
    uint8_t slots[SH_BYTES];
} shelf_state_t;

#ifdef __cplusplus
extern "C" {
#endif
void shelf_init(void);
void shelf_clearall(void);
void shelf_show(shelf_state_t *s);
int shelf_isset(shelf_state_t *s, int y, int x);
int shelf_set(shelf_state_t *s, int y, int x);
int shelf_clear(shelf_state_t *s, int y, int x);
void shelf_clearstate(shelf_state_t *s);
#ifdef __cplusplus
}    
#endif
#endif//shelf.h ends here
