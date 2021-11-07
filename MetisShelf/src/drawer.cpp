#include <SPI.h>
#include <Arduino.h>
#include <stdint.h>
#include "config.h"
#include "drawer.h"

SPISettings settings(2000000, MSBFIRST, SPI_MODE0);

/*call in setup*/
void drawer_init(void) {
    pinMode(P_DISPLAY, OUTPUT);
    pinMode(P_BANGRESET, OUTPUT);
    digitalWrite(P_BANGRESET, HIGH);
    digitalWrite(P_DISPLAY, LOW);
    SPI.begin();
    return;
}

/*clear all LEDs on the physical shelf*/
void drawer_clearall(void) {
    digitalWrite(P_BANGRESET, LOW);
    delay(10);
    digitalWrite(P_BANGRESET, HIGH);
    return;
}

/*display the shelf state onto the actual physical shelf*/
void drawer_show(Drawer *s) {
    for(int i = D_BYTES-1; i >= 0; i--) {
        SPI.transfer(s->slots[i]);
    }
    digitalWrite(P_DISPLAY, HIGH);
    delay(10);
    digitalWrite(P_DISPLAY, LOW);
}

/* return 1 if shelf led is set in the state. Return 0 if it's not. 
 * Return -1 for out of range
 */
int drawer_isset(Drawer *s, uint8_t y, uint8_t x) {
    int led = (y*D_WIDTH)+x;
    if(led > D_SIZE) return -1;
    return (s->slots[led>>3] & (1<<(led&0x07))) >> (led&0x07);
}

/* sets bit on shelf. Returns 0 if led is set. Returns -1 for out of
 * range
*/
int drawer_set(Drawer *s, uint8_t y, uint8_t x) {
    int led = (y*D_WIDTH)+x;
    if(led > D_SIZE) return -1;
    s->slots[led>>3] |= (1 << (led&0x07));
    return 0;
}

/* clears bit on shelf. Returns 0 if led is cleared. Returns -1 for out of 
 * range.
 */
int drawer_clear(Drawer *s, uint8_t y, uint8_t x) {
    int led = (y*D_WIDTH)+x;
    if(led > D_SIZE) return -1;
    s->slots[led>>3] &= ~(1 << (led&0x07));
    return 0;
}
/*clear the drawer state*/
void drawer_clearstate(Drawer *s) {
    for(int i = 0; i < D_BYTES; i++) {
        s->slots[i] = 0;
    }
}
