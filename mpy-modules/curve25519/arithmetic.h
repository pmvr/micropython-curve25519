#ifndef __arithmetic__
#define __arithmetic__

void shift_right(uint32_t *x);
uint32_t add_zxy(uint32_t *z, uint32_t *x, uint32_t *y);
uint32_t sub_zxy(uint32_t *z, uint32_t *x, uint32_t *y);
void add_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p);
void mul_zxy(uint32_t *z, uint32_t *x, uint32_t y);
void mul_zx0y0(uint32_t *z, uint32_t x, uint32_t y);
void mont_mul_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p);
void mont_mul_zxy0_mod_p(uint32_t *z, uint32_t *x, uint32_t y, uint32_t *p);

#endif
