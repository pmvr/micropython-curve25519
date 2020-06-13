#include "stdint.h"
#include "ec.h"


uint32_t add_zxy(uint32_t *z, uint32_t *x, uint32_t *y) {
    uint32_t carry;
    __asm__ volatile (
    "LDMIA  %1!, {r3-r6}\n"
    "LDMIA  %2!, {r7-r10}\n"
    "ADDS   r3, r3, r7\n"
    "ADCS   r4, r4, r8\n"
    "ADCS   r5, r5, r9\n"
    "ADCS   r6, r6, r10\n"
    "STMIA  %3!, {r3-r6}\n"
    "LDMIA  %1, {r3-r6}\n"
    "LDMIA  %2, {r7-r10}\n"
    "ADCS   r3, r3, r7\n"
    "ADCS   r4, r4, r8\n"
    "ADCS   r5, r5, r9\n"
    "ADCS   r6, r6, r10\n"
    "STMIA  %3, {r3-r6}\n"
    "MOV    %0, 0\n"
    "ADCS   %0, %0, 0\n"
    : "=r" (carry) : "r" (x), "r" (y), "r" (z) : "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
    return carry;
}


uint32_t sub_zxy(uint32_t *z, uint32_t *x, uint32_t *y) {
    uint32_t carry;
    __asm__ volatile (
    "LDMIA  %1!, {r3-r6}\n"
    "LDMIA  %2!, {r7-r10}\n"
    "SUBS   r3, r3, r7\n"
    "SBCS   r4, r4, r8\n"
    "SBCS   r5, r5, r9\n"
    "SBCS   r6, r6, r10\n"
    "STMIA  %3!, {r3-r6}\n"
    "LDMIA  %1, {r3-r6}\n"
    "LDMIA  %2, {r7-r10}\n"
    "SBCS   r3, r3, r7\n"
    "SBCS   r4, r4, r8\n"
    "SBCS   r5, r5, r9\n"
    "SBCS   r6, r6, r10\n"
    "STMIA  %3, {r3-r6}\n"
    "MOV    %0, 0\n"
    "ADCS   %0, %0, 0\n"
    : "=r" (carry) : "r" (x), "r" (y), "r" (z) : "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
    return carry;
}


void add_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p) {
    uint32_t z1[8], z2[8];
    uint32_t* ret[2] = {z1, z2};
    add_zxy(z1, x, y);
    uint32_t c = sub_zxy(z2, z1, p);  // carry not set if negative
    for (int i=0; i<8; i++) z[i] = ret[c][i];
}


void sub_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p) {
    // x-y % p
    uint32_t z1[8], z2[8];
    uint32_t* ret[2] = {z2, z1};
    uint32_t c = sub_zxy(z1, x, y); // carry not set if negative
    add_zxy(z2, z1, p);
    for (int i=0; i<8; i++) z[i] = ret[c][i];
}


void mul_add_zxy(uint32_t *z, uint32_t *x, uint32_t y) {
    // z += x*y
    // Note, UMAAL is not available
    __asm__ volatile (
    // 0
    "LDMIA  %0!, {r3}\n"
    "UMULL  r5, r6, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADDS   r5, r5, r3\n"
    "STMIA  %2!, {r5}\n"
    // 1
    "LDMIA  %0!, {r3}\n"
    "MOV    r5, 0\n"
    "UMLAL  r6, r5, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r6, r6, r3\n"
    "STMIA  %2!, {r6}\n"
    // 2
    "LDMIA  %0!, {r3}\n"
    "MOV    r6, 0\n"
    "UMLAL  r5, r6, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r5, r5, r3\n"
    "STMIA  %2!, {r5}\n"
    // 3
    "LDMIA  %0!, {r3}\n"
    "MOV    r5, 0\n"
    "UMLAL  r6, r5, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r6, r6, r3\n"
    "STMIA  %2!, {r6}\n"
    // 4
    "LDMIA  %0!, {r3}\n"
    "MOV    r6, 0\n"
    "UMLAL  r5, r6, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r5, r5, r3\n"
    "STMIA  %2!, {r5}\n"
    // 5
    "LDMIA  %0!, {r3}\n"
    "MOV    r5, 0\n"
    "UMLAL  r6, r5, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r6, r6, r3\n"
    "STMIA  %2!, {r6}\n"
    // 6
    "LDMIA  %0!, {r3}\n"
    "MOV    r6, 0\n"
    "UMLAL  r5, r6, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r5, r5, r3\n"
    "STMIA  %2!, {r5}\n"
    // 7
    "LDMIA  %0!, {r3}\n"
    "MOV    r5, 0\n"
    "UMLAL  r6, r5, r3, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r6, r6, r3\n"
    "STMIA  %2!, {r6}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r5, r5, r3\n"
    "STMIA  %2, {r5}\n"
    : : "r" (x), "r" (y), "r" (z) : "r3", "r5", "r6"
    );
}


void mul_add_zx0y0(uint32_t *z, uint32_t x, uint32_t y) {
    // z += x0 * y0
    __asm__ volatile (
    // 0
    "UMULL  r5, r6, %0, %1\n"
    "LDMIA  %2, {r3}\n"
    "ADDS   r3, r3, r5\n"
    "STMIA  %2!, {r3}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r6\n"
    "STMIA  %2!, {r3}\n"
    "MOV    r5, 0\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r5\n"
    "STMIA  %2!, {r3}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r5\n"
    "STMIA  %2!, {r3}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r5\n"
    "STMIA  %2!, {r3}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r5\n"
    "STMIA  %2!, {r3}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r5\n"
    "STMIA  %2!, {r3}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r5\n"
    "STMIA  %2!, {r3}\n"
    "LDMIA  %2, {r3}\n"
    "ADCS   r3, r3, r5\n"
    "STMIA  %2, {r3}\n"
    //"SUBS   %2, 32\n"  //*
    : : "r" (x), "r" (y), "r" (z) : "r3", "r5", "r6"
    );
}


void pu_add_shift(uint32_t *t, uint32_t u) {
    // computes t = (t + (2^255 -19) * u) >> 32
    __asm__ volatile (
    "MOV    r3, %2\n"
    "UMULL  r5, r6, r3, %1\n"    
    "MOV    r2, 0\n"
    "LSRS   %1, %1, 1\n"
    "RRXS   r2, r2\n"
    // t[8]:t[7] + (u << 255)
    "ADDS   %0, %0, 28\n"
    "LDMIA  %0, {r3}\n"
    "ADDS   r3, r3, r2\n"
    "STMIA  %0!, {r3}\n"
    "LDMIA  %0, {r3}\n"
    "ADCS   r3, r3, %1\n"
    "STMIA  %0, {r3}\n"
    "SUBS   %0, %0, 32\n"
    // t - 19*u
    "MOV    r2, %0\n"
    "LDMIA  %0!, {r4}\n"
    "SUBS   r4, r4, r5\n"
    "LDMIA  %0!, {r4}\n"
    "SBCS   r4, r4, r6\n"
    "STMIA  r2!, {r4}\n"
    "MOV    r5, 0\n"
    "LDMIA  %0!, {r4}\n"
    "SBCS   r4, r4, r5\n"
    "STMIA  r2!, {r4}\n"
    "LDMIA  %0!, {r4}\n"
    "SBCS   r4, r4, r5\n"
    "STMIA  r2!, {r4}\n"
    "LDMIA  %0!, {r4}\n"
    "SBCS   r4, r4, r5\n"
    "STMIA  r2!, {r4}\n"
    "LDMIA  %0!, {r4}\n"
    "SBCS   r4, r4, r5\n"
    "STMIA  r2!, {r4}\n"
    "LDMIA  %0!, {r4}\n"
    "SBCS   r4, r4, r5\n"
    "STMIA  r2!, {r4}\n"
    "LDMIA  %0!, {r4}\n"
    "SBCS   r4, r4, r5\n"
    "STMIA  r2!, {r4}\n"
    "LDMIA  %0, {r4}\n"
    "SBCS   r4, r4, r5\n"
    "STMIA  r2!, {r4}\n"
    "STMIA  r2, {r5}\n"  // ms-word = 0
    //"SUBS   %0, %0, 32\n"
    : : "r" (t), "r" (u) , "r" (DIFF_P) : "r2", "r3", "r4", "r5", "r6"
    );
}


void mont_mul_zxy_mod_p(uint32_t *z, uint32_t *x, uint32_t *y, uint32_t *p) {
    // see Alg. 14.36 HoAC
    // -m^(-1) mod b is 678152731 for curve25519
    uint32_t u;
    uint32_t tmp[9], a[9];
    uint32_t* ret[2] = {a, tmp};
    
    for (int i=0; i<9; i++) a[i] = 0;
    for (int i=0; i<8; i++) {
        u = (a[0] + x[i] * y[0]) * MINUS_IM;
        mul_add_zxy(a, y, x[i]);  // A <- A + xi y 
        pu_add_shift(a, u);  // A <- (A + u m) / b
    }
    uint32_t c = sub_zxy(tmp, a, p);  // carry not set if negative
    for (int i=0; i<8; i++) z[i] = ret[c][i];
}


void mont_mul_zxy0_mod_p(uint32_t *z, uint32_t *x, uint32_t y, uint32_t *p) {
    // see Alg. 14.36 HoAC
    // -m^(-1) mod b is 678152731 for curve25519
    uint32_t u;
    uint32_t tmp[9], a[9];
    uint32_t* ret[2] = {a, tmp};
    
    for (int i=0; i<9; i++) a[i] = 0;
    for (int i=0; i<8; i++) {
        u = (a[0] + x[i] * y) * MINUS_IM;
        mul_add_zx0y0(a, y, x[i]);  // A <- A + xi y
        pu_add_shift(a, u);  // A <- (A + u m) / b
    }
    uint32_t c = sub_zxy(tmp, a, p);  // carry not set if negative
    for (int i=0; i<8; i++) z[i] = ret[c][i];
}

