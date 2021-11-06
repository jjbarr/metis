#include <SPI.h>
#include <Arduino.h>
#include "config.h"
#include "shelf.h"

/*call in setup*/
void shelf_init(void) {
    pinMode(P_DISPLAY, OUTPUT);
    pinMode(P_BANGRESET, OUTPUT);
    digitalWrite(P_BANGRESET, HIGH);
    digitalWrite(P_DISPLAY, LOW);
    SPI.begin();
    return;
}

/*clear all LEDs on the physical shelf*/
void shelf_clearall(void) {
    digitalWrite(P_BANGRESET, LOW);
    delay(10);
    digitalWrite(P_BANGRESET, HIGH);
    return;
}

/*display the shelf state onto the actual physical shelf*/
void shelf_show(shelf_state_t *s) {
    for(int i = 0; i < SH_BYTES; i++) {
        SPI.transfer(s->slots[i]);
    }
    digitalWrite(P_DISPLAY, HIGH);
    delay(10);
    diigtalWrite(P_DISPLAY, LOW);
}

/* return 1 if shelf led is set in the state. Return 0 if it's not. 
 * Return -1 for out of range
 */
int shelf_isset(shelf_state_t *s, int y, int x) {
    if((y*SH_WIDTH + x) > SH_SIZE) return -1;
    return (s->slots[(((y*SH_WIDTH)+x)>>3)] | (1<<(((y*SH_WIDTH)+x)|0x07)))
        >> (((y*SH_WIDTH)+x)|0x07)
}

/* sets bit on shelf. Returns 0 if led is set. Returns -1 for out of
 * range
*/
int shelf_set(shelf_state_t *s, int y, int x) {
    if((y*SH_WIDTH + x) > SH_SIZE) return -1;
    return (s->slots[((y*SH_WIDTH)+x)>>3])
}

/* clears bit on shelf. Returns 0 if led is cleared. Returns -1 for out of 
 * range.
 */
int shelf_clear(shelf_state_t *s, int y, int x) {
    if((y*SH_WIDTH + x) > SH_SIZE) return -1;
}

