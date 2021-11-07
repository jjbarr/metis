#include <Arduino.h>
#include <stdint.h>
#include "serinter.h"

//turns a hex digit into its corresponding number.
//return -1 if fail
int hex(int c) {
    if(c > 0x39) c = (c&~0x20)-'A'+10;
    else c = c-'0';
    if(c>0xf) return -1;
}

//reads a hex number of length n from serial into r. (n should be < 8)
//returns -1 on fail
int hexes(uint32_t *r, int n) {
    int c;
    for(int i = 0; i < n; i++) {
        while((c = Serial.read()) < 0);
        int d = hex(c);
        if(d < 0) return -1;
        *r = (*r << 4)|d;
    }
}

int next_msg(msg *m) {
    while() {
        if(!Serial.available()) return -1;
        int c = Serial.read();
        switch(c) {
        case 's': m->t = MT_SETLED
                break;
        case 'c': m->t = MT_CLRLED
                break;
        case 'x': m->t = MT_CLRALL
        case 'd': m->t = MT_GETSHDIM
                break;
        case 'e': m->t = MT_ENSHELVES
                return 0;
        default: return -1;
        }
        if(hexes(&m->shelf_id, 8) < 0) return -1;
        uint32_t v = 0;
        if(hexes(&v, 2) < 0) return -1;
        m->row = v;
        v = 0;
        if(hexes(&v, 2) < 0) return -1;
        m->col = v;
        return 0;
    }
}
