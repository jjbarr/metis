#ifndef SERINTER_H
#define SERINTER_H
#include <stdint.h>
typedef enum {
    MT_SETLED,
    MT_CLRLED,
    MT_CLRALL,
    MT_ENSHELVES, //enumerate shelves: send back the shelves, hex, comma separated, period-terminated
    MT_GETSHDIM //get the shelf dimensions as w,h (comma-separated, hex)
} msgtype;

typedef struct msg_s {
    msgtype t;
    uint32_t shelf_id;
    uint8_t row;
    uint8_t col;
} msg;

void serinter_init(void);

/* if next_msg returns 0, there is a new message to read. If it returns -1,
   there isn't.*/
int next_msg(msg *m);

#endif
