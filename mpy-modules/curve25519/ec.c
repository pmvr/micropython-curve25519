#include "py/dynruntime.h"
#include "arithmetic.h"
#include "ec.h"

const uint32_t a24R = 0x468ba6;
const uint32_t p[8] = {0x7fffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffed};
const uint32_t pR2 = 0x5a4;
const uint32_t R = 38;
const uint32_t sqrt_minus_486664R[8] = {0x4038adb9, 0xa83f001e, 0xc1bcaf57, 0x688c332e, 0xa9fa8eee, 0xcb6e1095, 0xa7ab4e9e, 0x1baf4abd};

uint32_t Curve_p[8];

void random_z(uint32_t *z) {
    // TODO: a true random source should be provided
    for (uint32_t i=0; i<7; i++) z[i] = 0xC3A50FE1;
    z[7] &= 0x7fffffff;
}


void ini_curve() {
    for (int i=0; i<8; i++) {
        Curve_p[i] = p[7-i];
    }
}


void to_Montgomery(uint32_t *x) {
    uint32_t s[8];
    mont_mul_zxy0_mod_p(s, x, pR2, Curve_p);
    for (uint32_t i=0;i<8;i++) x[i] = s[i];
}


void from_Montgomery(uint32_t *x) {
    uint32_t s[8];
    mont_mul_zxy0_mod_p(s, x, 1, Curve_p);
    for (uint32_t i=0;i<8;i++) x[i] = s[i];
}


void mod_inverse(uint32_t *z_inv, uint32_t *z) {
    uint32_t z2[8];
    uint32_t z9[8];
    uint32_t z11[8];
    uint32_t z2_5_0[8];
    uint32_t z2_10_0[8];
    uint32_t z2_20_0[8];
    uint32_t z2_50_0[8];
    uint32_t z2_100_0[8];
    uint32_t t0[8];
    uint32_t t1[8];
    uint32_t i;

    mont_mul_zxy_mod_p(z2, z, z, Curve_p);     // 2
    mont_mul_zxy_mod_p(t1, z2, z2, Curve_p);   // 4
    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);   // 8
    mont_mul_zxy_mod_p(z9, t0, z, Curve_p);    // 9
    mont_mul_zxy_mod_p(z11, z9, z2, Curve_p);  // 11
    mont_mul_zxy_mod_p(t0, z11, z11, Curve_p); // 22
    mont_mul_zxy_mod_p(z2_5_0, t0, z9, Curve_p);  // 31 = 2^5 - 2^0

    mont_mul_zxy_mod_p(t0, z2_5_0, z2_5_0, Curve_p); // 2^6 - 2^1
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);   // 2^7 - 2^2
    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);   // 2^8 - 2^3
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);   // 2^9 - 2^4
    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);   // 2^10 - 2^5
    mont_mul_zxy_mod_p(z2_10_0, t0, z2_5_0, Curve_p);   // 2^10 - 2^0
    
    mont_mul_zxy_mod_p(t0, z2_10_0, z2_10_0, Curve_p);  // 2^11 - 2^1
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);   // 2^12 - 2^2
    for (i = 2; i < 10; i += 2) {
        mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);
        mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);
    }  // 2^20 - 2^10
    mont_mul_zxy_mod_p(z2_20_0, t1, z2_10_0, Curve_p);   // 2^20 - 2^0
        
    mont_mul_zxy_mod_p(t0, z2_20_0, z2_20_0, Curve_p);  // 2^21 - 2^1
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);   // 2^22 - 2^2
    for (i = 2; i < 20; i += 2) {
        mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);
        mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);
    }  // 2^40 - 2^20
    mont_mul_zxy_mod_p(t0, t1, z2_20_0, Curve_p);   // 2^40 - 2^0

    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);   // 2^41 - 2^1
    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);   // 2^42 - 2^2
    for (i = 2; i < 10; i += 2) {
        mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);
        mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);
    }  // 2^50 - 2^10
    mont_mul_zxy_mod_p(z2_50_0, t0, z2_10_0, Curve_p);   // 2^50 - 2^0

    mont_mul_zxy_mod_p(t0, z2_50_0, z2_50_0, Curve_p);   // 2^51 - 2^1
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);   // 2^42 - 2^2
    for (i = 2; i < 50; i += 2) {
        mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);
        mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);
    }  // 2^100 - 2^50
    mont_mul_zxy_mod_p(z2_100_0, t1, z2_50_0, Curve_p);   // 2^100 - 2^0

    mont_mul_zxy_mod_p(t1, z2_100_0, z2_100_0, Curve_p);   // 2^101 - 2^1
    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);   // 2^102 - 2^2
    for (i = 2; i < 100; i += 2) {
        mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);
        mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);
    }  // 2^200 - 2^100
    mont_mul_zxy_mod_p(t1, t0, z2_100_0, Curve_p);   // 2^200 - 2^0

    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);   // 2^201 - 2^1
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);   // 2^102 - 2^2
    for (i = 2; i < 50; i += 2) {
        mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);
        mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);
    }  // 2^250 - 2^50
    mont_mul_zxy_mod_p(t0, t1, z2_50_0, Curve_p);   // 2^250 - 2^0

    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);  // 2^251 - 2^1
    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);  // 2^252 - 2^2
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);  // 2^253 - 2^3
    mont_mul_zxy_mod_p(t0, t1, t1, Curve_p);  // 2^254 - 2^4
    mont_mul_zxy_mod_p(t1, t0, t0, Curve_p);  // 2^255 - 2^5
    mont_mul_zxy_mod_p(z_inv, t1, z11, Curve_p);  // 2^255 - 21
}

void recover_y(uint32_t *x, uint32_t *y, uint32_t *z, uint32_t *xp, uint32_t *yp, uint32_t *xq, uint32_t *zq, uint32_t *xpq, uint32_t *zpq) {
    // https://eprint.iacr.org/2017/212.pdf
    uint32_t AR = 18493156;
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
    mod_inverse(w, w); // w = (u+1)^(-1) * v^(-1)
    sub_zxy_mod_p(y, u, t, Curve_p);  // y = u-1
    mont_mul_zxy_mod_p(x, y, w, Curve_p);  // x = (u-1)/((u+1) * v)
    mont_mul_zxy_mod_p(y, x, v, Curve_p);
    for (uint32_t i=0; i<8; i++) t[i] = sqrt_minus_486664R[7-i];
    mont_mul_zxy_mod_p(x, t, w, Curve_p);
    mont_mul_zxy_mod_p(t, x, s, Curve_p);
    mont_mul_zxy_mod_p(x, t, u, Curve_p);
}


void cswap(uint32_t swap, uint32_t **x, uint32_t **y) {
    // https://tools.ietf.org/html/rfc7748 sec. 5
    __asm__ volatile (
    "LDMIA  %1, {r3}\n"
    "LDMIA  %2, {r4}\n"
    "RSB    r5, %0, 0\n"
    "EOR    r6, r3, r4\n"
    "AND    r5, r5, r6\n"
    "EOR    r3, r3, r5\n"
    "EOR    r4, r4, r5\n"
    "STMIA  %1, {r3}\n"
    "STMIA  %2, {r4}\n"
    : : "r" (swap), "r" (x), "r" (y) : "r3", "r4", "r5", "r6" 
    );
}

void X25519(uint32_t *q, uint32_t *r, uint8_t *k, uint32_t *u, uint32_t *v, uint8_t toEdward) {
    // cpmputes q = k * u
    uint32_t x1[8], x[2*8], z[2*8];
    uint32_t *x2 = x; uint32_t *x3 = x+8;
    uint32_t *z2 = z; uint32_t *z3 = z+8;
    uint8_t swap, kt, kw;
    uint32_t A[8], AA[8], B[8], BB[8], C[8], D[8], E[8], DA[8], CB[8];
    
    ini_curve();

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
        mod_inverse(C, C);
        mont_mul_zxy_mod_p(q, A, C, Curve_p);
        mont_mul_zxy_mod_p(r, B, C, Curve_p);
        if (toEdward) {
            for (uint32_t i=0; i<8; i++) {
                A[i] = q[i];
                B[i] = r[i];
            }
            montgomery2edward(q, r, A, B);
        }
        from_Montgomery(q);
        from_Montgomery(r);
    } else {
        // compute x-only
        // compute z2^(-1) = z2^{p-2}
        mod_inverse(A, z2);
        mont_mul_zxy_mod_p(q, x2, A, Curve_p);
        from_Montgomery(q);
    }
}
