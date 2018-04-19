#ifndef SBOX_H
#define SBOX_H

extern int sbox_send(char*);
extern int sbox_expect(char*, void (*)(int, void*), void*);
extern int sbox_connect();
extern int sbox_init(char*);

#endif
