#include "stdint.h"
#include "arithmetic.h"

const uint32_t a24R = 0x468ba6;
const uint32_t Curve_p[8] = {0xffffffed, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff};  // little endian
const uint32_t pR2 = 0x5a4;
const uint32_t R = 38;
const uint32_t sqrt_minus_486664R[8] = {0x1baf4abd, 0xa7ab4e9e, 0xcb6e1095, 0xa9fa8eee, 0x688c332e, 0xc1bcaf57, 0xa83f001e, 0x4038adb9};  // little endian

void random_z(uint32_t *z) {
    // TODO: a true random source should be provided
    for (uint32_t i=0; i<7; i++) z[i] = 0xC3A50FE1;
    z[7] &= 0x7fffffff;
}


#define to_Montgomery(x) mont_mul_zxy0_mod_p(x, x, pR2, Curve_p);
#define from_Montgomery(x) mont_mul_zxy0_mod_p(x, x, 1, Curve_p);


void recover_y(uint32_t *x, uint32_t *y, uint32_t *z, uint32_t *xp, uint32_t *yp, uint32_t *xq, uint32_t *zq, uint32_t *xpq, uint32_t *zpq) {
    // https://eprint.iacr.org/2017/212.pdf
    const uint32_t AR = 18493156;
    uint32_t v1[8], v2[8], v3[8], v4[8], s[8], t[8];
    
    mont_mul_zxy_mod_p(v1, xp, zq, Curve_p);
    add_zxy_mod_p(v2, xq, v1, Curve_p);
    sub_zxy_mod_p(v3, xq, v1, Curve_p);
    mont_mul_zxy_mod_p(t, v3, v3, Curve_p);
    mont_mul_zxy_mod_p(v3, t, xpq, Curve_p);
    mont_mul_zxy0_mod_p(v1, zq, AR, Curve_p);
    add_zxy_mod_p(v1, v1, v1, Curve_p);
    add_zxy_mod_p(v2, v2, v1, Curve_p);
    mont_mul_zxy_mod_p(v4, xp, xq, Curve_p);
    add_zxy_mod_p(v4, v4, zq, Curve_p);
    mont_mul_zxy_mod_p(s, v2, v4, Curve_p);
    mont_mul_zxy_mod_p(t, v1, zq, Curve_p);
    sub_zxy_mod_p(s, s, t, Curve_p);
    mont_mul_zxy_mod_p(v2, s, zpq, Curve_p);
    sub_zxy_mod_p(y, v2, v3, Curve_p);
    add_zxy_mod_p(v1, yp, yp, Curve_p);
    mont_mul_zxy_mod_p(s, v1, zq, Curve_p);
    mont_mul_zxy_mod_p(v1, s, zpq, Curve_p);
    mont_mul_zxy_mod_p(x, v1, xq, Curve_p);
    mont_mul_zxy_mod_p(z, v1, zq, Curve_p);
}


void montgomery2edward(uint32_t *x, uint32_t *y, uint32_t *u, uint32_t *v) {
    // (x, y) = (sqrt(-486664)*u/v, (u-1)/(u+1))
    uint32_t s[8], t[8], w[8];
    
    for (uint32_t i=1; i<8; i++) t[i] = 0;
    t[0] = R;  // 1*R
    add_zxy_mod_p(s, u, t, Curve_p);  // s = u+1
    mont_mul_zxy_mod_p(w, s, v, Curve_p);
    // mod_inverse(w, w); // w = (u+1)^(-1) * v^(-1)
    mod_inverse(Curve_p, w);
    to_Montgomery(w);
    to_Montgomery(w);
    sub_zxy_mod_p(y, u, t, Curve_p);  // y = u-1
    mont_mul_zxy_mod_p(x, y, w, Curve_p);  // x = (u-1)/((u+1) * v)
    mont_mul_zxy_mod_p(y, x, v, Curve_p);
    mont_mul_zxy_mod_p(x, sqrt_minus_486664R, w, Curve_p);
    mont_mul_zxy_mod_p(t, x, s, Curve_p);
    mont_mul_zxy_mod_p(x, t, u, Curve_p);
}


// https://tools.ietf.org/html/rfc7748 sec. 5
#define cswap(swap, x, y) \
    __asm__ volatile ( \
    "LDMIA  %1, {r3}\n" \
    "LDMIA  %2, {r4}\n" \
    "RSB    r5, %0, 0\n" \
    "EOR    r6, r3, r4\n" \
    "AND    r5, r5, r6\n" \
    "EOR    r3, r3, r5\n" \
    "EOR    r4, r4, r5\n" \
    "STMIA  %1, {r3}\n" \
    "STMIA  %2, {r4}\n" \
    : : "r" (swap), "r" (x), "r" (y) : "r3", "r4", "r5", "r6"  \
    );


void X25519(uint32_t *q, uint32_t *r, uint8_t *k, uint32_t *u, uint32_t *v, uint8_t toEdward) {
    // cpmputes q = k * u
    uint32_t x1[8], x[2*8], z[2*8];
    uint32_t *x2 = x; uint32_t *x3 = x+8;
    uint32_t *z2 = z; uint32_t *z3 = z+8;
    uint8_t swap, kt, kw;
    uint32_t A[8], AA[8], B[8], BB[8], C[8], D[8], E[8], DA[8], CB[8];
    
    // x1, z1 = u, 1
    // x2, z2 = 1, 0
    // x3, z3 = u, 1
    random_z(z3);
    random_z(x2);
    for (uint32_t i=0; i<8; i++) {
        A[i] = u[i];
        z2[i] = 0;
    }
    to_Montgomery(A);
    for (uint32_t i=0; i<8; i++) x1[i] = A[i];
    mont_mul_zxy_mod_p(x3, A, z3, Curve_p);
    
    swap = 0;
    kw = k[31];
    for (int32_t t=254; t>=0; t--) {
        if (t % 8 == 7) kw = k[t / 8];
        kt = (kw >> (t % 8)) & 1;
        swap ^= kt;
        // conditional swap
        cswap(swap, &x2, &x3);
        cswap(swap, &z2, &z3);
        swap = kt;
        
        add_zxy_mod_p(A, x2, z2, Curve_p);
        mont_mul_zxy_mod_p(AA, A, A, Curve_p);
        sub_zxy_mod_p(B, x2, z2, Curve_p);
        mont_mul_zxy_mod_p(BB, B, B, Curve_p);
        sub_zxy_mod_p(E, AA, BB, Curve_p);
        add_zxy_mod_p(C, x3, z3, Curve_p);
        sub_zxy_mod_p(D, x3, z3, Curve_p);
        mont_mul_zxy_mod_p(DA, D, A, Curve_p);
        mont_mul_zxy_mod_p(CB, C, B, Curve_p);
        add_zxy_mod_p(A, DA, CB, Curve_p);
        mont_mul_zxy_mod_p(x3, A, A, Curve_p);
        sub_zxy_mod_p(A, DA, CB, Curve_p);
        mont_mul_zxy_mod_p(B, A, A, Curve_p);
        mont_mul_zxy_mod_p(z3, x1, B, Curve_p);
        mont_mul_zxy_mod_p(x2, AA, BB, Curve_p);
        mont_mul_zxy0_mod_p(A, E, a24R, Curve_p);
        add_zxy_mod_p(AA, AA, A, Curve_p);
        mont_mul_zxy_mod_p(z2, E, AA, Curve_p);
    }
    // conditional swap
    cswap(swap, &x2, &x3);
    cswap(swap, &z2, &z3);
    if (r && v) {
        // compute y-coordinate
        for (uint32_t i=0; i<8; i++) {
            AA[i] = u[i];
            BB[i] = v[i];
        }
        to_Montgomery(AA);
        to_Montgomery(BB);
        recover_y(A, B, C, AA, BB, x2, z2, x3, z3);
        from_Montgomery(C);
        mod_inverse(Curve_p, C);
        mont_mul_zxy_mod_p(q, A, C, Curve_p);
        mont_mul_zxy_mod_p(r, B, C, Curve_p);
        if (toEdward) {
            mont_mul_zxy0_mod_p(A, q, pR2, Curve_p);
            mont_mul_zxy0_mod_p(B, r, pR2, Curve_p);
            montgomery2edward(q, r, A, B);
            from_Montgomery(q);
            from_Montgomery(r);
        }
    } else {
        // compute x-only
        // compute z2^(-1)
        from_Montgomery(z2);
        mod_inverse(Curve_p, z2);
        mont_mul_zxy_mod_p(q, x2, z2, Curve_p);
    }
}
