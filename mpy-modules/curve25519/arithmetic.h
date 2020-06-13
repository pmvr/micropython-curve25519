#ifndef __arithmetic__
#define __arithmetic__

void add_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p);
void sub_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p);
void mont_mul_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p);
void mont_mul_zxy0_mod_p(uint32_t *z, uint32_t *x, uint32_t y, uint32_t *p);

#endif
