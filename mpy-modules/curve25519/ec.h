#ifndef __ec__
#define __ec__

#define DIFF_P 19
#define MINUS_IM 678152731  // -m^(-1) mod b is 678152731 for curve25519
void X25519(uint32_t *q, uint32_t *r, uint8_t *k, uint32_t *u, uint32_t *v, uint8_t toEdward);

#endif
