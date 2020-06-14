import curve25519
import time
# from urandom import randint

d = b'\x70\x1f\xb4\x30\x86\x55\xb4\x76\xb6\x78\x9b\x73\x25\xf9\xea\x8c\xdd\xd1\x6a\x58\x53\x3f\xf6\xd9\xe6\x00\x09\x46\x4a\x5f\x9d\x54\x00\x00\x00\x00'
u = b'\x09' + bytes(31)
v = b'\xd9\xd3\xce~\xa2\xc5\xe9)\xb2a|m~M=\x92L\xd1Hw,\xdd\x1e\xe0\xb4\x86\xa0\xb8\xa1\x19\xae \x00\x00\x00\x00'

print('Test vectors from https://tools.ietf.org/html/rfc8031#appendix-A')
print('Test 1: X25519: q = d*u')
start = time.ticks_ms()  # get millisecond counter
b = curve25519.x25519(d, u)
delta = time.ticks_diff(time.ticks_ms(), start)  # compute time difference
print('Computation time: %d ms' % delta)
q = int.from_bytes(b, 'little')
print('q [hex/dec] = %x  %d' % (q, q))
if q != 0x66c7fb0d9f7090f777fa8493081ce8a4f174dbbbf9a36f16ba571206d4ddd548:
    print('Test 1 failed.')
else:
    print('Test 1 passed.')

print()

print('Test 2: X25519 + y-coordinate recovery + transform to Edwards-curve')
print('(x, y) = Edward(q, r), (q, r) = d*(u, v)')
start = time.ticks_ms()  # get millisecond counter
b = curve25519.x25519_ed(d, u, v)
delta = time.ticks_diff(time.ticks_ms(), start)  # compute time difference
print('Computation time: %d ms' % delta)
x = int.from_bytes(b[0], 'little')
y = int.from_bytes(b[1], 'little')
print('x [hex/dec] = %x  %d' % (x, x))
print('y [hex/dec] = %x  %d' % (y, y))
if x != 0x1ce7e6e3a747a25352df2d3155f06427ba389769e37755731dead2b54c5cef03 or y != 0x4dd1c7c2001c147333ceedf77ebd48b1100e2a95f88cf1f40d1b74ec7279e657:
    print('Test 2 failed.')
else:
    print('Test 2 passed.')
print()
