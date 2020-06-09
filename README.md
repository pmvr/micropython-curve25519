# micropython-curve25519
Implementation of X25519 and Ed25519 as a micropython mpy-module on a Pyboard

## Background
[Curve25519]([https://cr.yp.to/ecdh.html) is a state-of-the-art for elliptic curve Diffie-Hellman, designed for both performance and security. Forthermore, it is used for Edwards-curve Digital Signature Algorithm ([EdDSA](https://tools.ietf.org/html/rfc8032)).

While the key exchange is refered by the function [X25519](https://tools.ietf.org/html/rfc8031), Ed25519 refers to the digital signature algorithm.

X25519 performs scalar multiplication on the Montgomery form of Curve25519. It uses a Montgomery ladder on the u-coordinate only, see [here](https://tools.ietf.org/html/rfc7748#section-5) for the algorithm.

Ed25519 is performed on the twisted Edwards curve of Curve25519 on the x- und y-coordinate. For performance reasons, the signing operation uses here the function X25519 and then a recovery of the v-coordinate, see Algorithm 5 in this [paper](https://eprint.iacr.org/2017/212.pdf), followed by the [birational map](https://tools.ietf.org/html/rfc7748#section-4.1) between the Montgomery curve and the Edwards curve.

## mpy-Modules
For native machine code in mpy-files see [here](https://docs.micropython.org/en/latest/develop/natmod.html).

There are two modules in the `mpy-modules/` sub-directory:
* One for the SHA512 `mpy-modules/sha512/`, where I modfied [this](https://github.com/routar/C-SHA2) code.
* And one for X25519 `mpy-modules/curve25519/`.

The the resulting two mpy-files have have to be copied to a Pyboard.

## Results
The results are received on a Pyboard ([STM32F722IEK](https://store.micropython.org/product/PYBD-SF2-W4F2)).
### X25519
Python script `x25519.py`

    Test vectors from https://tools.ietf.org/html/rfc8031#appendix-A
    Test 1: X25519: q = d*u
    Computatation time: 26 ms
    q [hex/dec] = 66c7fb0d9f7090f777fa8493081ce8a4f174dbbbf9a36f16ba571206d4ddd548      46489245826987382655505058740283756869827209462947799117248009944518788765000
    Test 1 passed.

    Test 2: X25519 + y-coordinate recovery + transform to Edwards-curve
    (x, y) = Edward(q, r), (q, r) = d*(u, v)
    Computatation time: 27 ms
    x [hex/dec] = 1ce7e6e3a747a25352df2d3155f06427ba389769e37755731dead2b54c5cef03      13074494971479542188989287385397236998770807488645203601973104535274459557635
    y [hex/dec] = 4dd1c7c2001c147333ceedf77ebd48b1100e2a95f88cf1f40d1b74ec7279e657      35198739055214410372845858661063095427357109357427482712729161712065293444695
    Test 2 passed.

### Ed25519
Python script `ed25519.py`

    Test 1: Length of message: 0 bytes
    Computatation time: 58 ms
    Test 1 passed.

    Test 2: Length of message: 1 byte
    Computatation time: 58 ms
    Test 2 passed.

    Test 3: Length of message: 2 bytes
    Computatation time: 58 ms
    Test 3 passed.

    Test 4: Length of message: 1023 bytes
    Computatation time: 67 ms
    Test 4 passed.

    Test 5: Length of message: 64 bytes
    Computatation time: 59 ms
    Test 5 passed.    

## Warning
The source code is not ready for production for both security reasons and missing regression tests.
