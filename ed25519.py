# -*- coding: utf-8 -*-

# Modified example code from https://tools.ietf.org/html/rfc8032
# implementation uses function x25519, y-recovery and map to edward

import time
import sha512
import curve25519

# Base field Z_p
p = 2**255 - 19
# Group order
q = 2**252 + 27742317777372353535851937790883648493


def sha512_modq(s):
    return int.from_bytes(sha512.digest(s), "little") % q


def point_compress(P):
    x, y = P
    return int.to_bytes(y | ((x & 1) << 255), 32, "little")


# These are functions for manipulating the private key.
def secret_expand(secret):
    if len(secret) != 32:
        raise Exception("Bad size of private key")
    h = sha512.digest(secret)
    a = int.from_bytes(h[:32], "little")
    a &= (1 << 254) - 8
    a |= (1 << 254)
    return (a, h[32:])


# The signature function works as below.
def sign(secret, msg):
    u = b'\x09' + bytes(35)
    v = b'\xd9\xd3\xce~\xa2\xc5\xe9)\xb2a|m~M=\x92L\xd1Hw,\xdd\x1e\xe0\xb4\x86\xa0\xb8\xa1\x19\xae \x00\x00\x00\x00'
    a, prefix = secret_expand(secret)
    # A = point_compress(point_mul(a, G))
    xy = curve25519.x25519_ed(a.to_bytes(36, 'little'), u, v)
    A = point_compress((int.from_bytes(xy[0], 'little'), int.from_bytes(xy[1], 'little')))
    r = sha512_modq(prefix + msg)
    # R = point_mul(r, G)
    xy = curve25519.x25519_ed(r.to_bytes(36, 'little'), u, v)
    R = (int.from_bytes(xy[0], 'little'), int.from_bytes(xy[1], 'little'))
    Rs = point_compress(R)
    h = sha512_modq(Rs + A + msg)
    s = (r + h * a) % q
    return Rs + int.to_bytes(s, 32, "little")



from ubinascii import hexlify, unhexlify
# Test Vectors for Ed25519 - https://tools.ietf.org/html/rfc8032#section-7.1
# Test 1
print('Test 1: Length of message: 0 bytes')
secret = unhexlify(b'9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60')
start = time.ticks_ms()  # get millisecond counter
signature = sign(secret, b'')
delta = time.ticks_diff(time.ticks_ms(), start)  # compute time difference
print('Computation time: %d ms' % delta)
if hexlify(signature) != b'e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b':
    print('Test 1 failed.')
    print(hexlify(signature))
else:
    print('Test 1 passed.')
print()

# Test 2
print('Test 2: Length of message: 1 byte')
secret = unhexlify(b'4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb')
start = time.ticks_ms()  # get millisecond counter
signature = sign(secret, b'\x72')
delta = time.ticks_diff(time.ticks_ms(), start)  # compute time difference
print('Computation time: %d ms' % delta)
if hexlify(signature) != b'92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da085ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00':
    print('Test 2 failed.')
    print(hexlify(signature))
else:
    print('Test 2 passed.')
print()

# Test 3
print('Test 3: Length of message: 2 bytes')
secret = unhexlify(b'c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7')
start = time.ticks_ms()  # get millisecond counter
signature = sign(secret, b'\xaf\x82')
delta = time.ticks_diff(time.ticks_ms(), start)  # compute time difference
print('Computation time: %d ms' % delta)
if hexlify(signature) != b'6291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a':
    print('Test 3 failed.')
    print(hexlify(signature))
else:
    print('Test 3 passed.')
print()

# Test 4
print('Test 4: Length of message: 1023 bytes')
secret = unhexlify(b'f5e5767cf153319517630f226876b86c8160cc583bc013744c6bf255f5cc0ee5')
start = time.ticks_ms()  # get millisecond counter
signature = sign(secret, b'\x08\xb8\xb2\xb73BBCv\x0f\xe4&\xa4\xb5I\x08c!\x10\xa6l/e\x91\xea\xbd3E\xe3\xe4\xeb\x98\xfan&K\xf0\x9e\xfe\x12\xeeP\xf8\xf5N\x9fw\xb1\xe3U\xf6\xc5\x05D\xe2?\xb1C=\xdfs\xbe\x84\xd8y\xde|\x00F\xdcI\x96\xd9\xe7s\xf4\xbc\x9e\xfeW8\x82\x9a\xdb&\xc8\x1b7\xc9:\x1b\'\x0b 2\x9de\x86u\xfcn\xa54\xe0\x81\nD2\x82k\xf5\x8c\x94\x1e\xfbe\xd5z3\x8b\xbd.&d\x0f\x89\xff\xbc\x1a\x85\x8e\xfc\xb8U\x0e\xe3\xa5\xe1\x99\x8b\xd1w\xe9:sc\xc3D\xfek\x19\x9e\xe5\xd0.\x82\xd5"\xc4\xfe\xba\x15E/\x80(\x8a\x82\x1aW\x91\x16\xecm\xad+;1\r\xa9\x03@\x1a\xa6!\x00\xab]\x1a6U>\x06 ;3\x89\x0c\xc9\xb82\xf7\x9e\xf8\x05`\xcc\xb9\xa3\x9c\xe7g\x96~\xd6(\xc6\xadW<\xb1\x16\xdb\xef\xef\xd7T\x99\xda\x96\xbdh\xa8\xa9{\x92\x8a\x8b\xbc\x10;f!\xfc\xde+\xec\xa1#\x1d k\xe6\xcd\x9e\xc7\xaf\xf6\xf6\xc9O\xcdr\x04\xed4U\xc6\x8c\x83\xf4\xa4\x1d\xa4\xaf+t\xef\\S\xf1\xd8\xacp\xbd\xcb~\xd1\x85\xce\x81\xbd\x845\x9dD%M\x95b\x9e\x98U\xa9J|\x19X\xd1\xf8\xad\xa5\xd0S.\xd8\xa5\xaa?\xb2\xd1{\xa7\x0e\xb6$\x8eYN\x1a"\x97\xac\xbb\xb3\x9dP/\x1a\x8cn\xb6\xf1\xce"\xb3\xde\x1a\x1f@\xcc$UA\x19\xa81\xa9\xaa\xd6\x07\x9c\xad\x88B]\xe6\xbd\xe1\xa9\x18~\xbb`\x92\xcfg\xbf+\x13\xfde\xf2p\x88\xd7\x8b~\x88<\x87Y\xd2\xc4\xf5\xc6Z\xdbuS\x87\x8a\xd5u\xf9\xfa\xd8x\xe8\n\x0c\x9b\xa6;\xcb\xcc\'2\xe6\x94\x85\xbb\xc9\xc9\x0b\xfb\xd6$\x81\xd9\x08\x9b\xec\xcf\x80\xcf\xe2\xdf\x16\xa2\xcfe\xbd\x92\xddY{\x07\x07\xe0\x91z\xf4\x8b\xbbu\xfe\xd4\x13\xd28\xf5UZzV\x9d\x80\xc3AJ\x8d\x08Y\xdce\xa4a(\xba\xb2z\xf8zq1O1\x8cx+#\xeb\xfe\x80\x8b\x82\xb0\xce&@\x1d."\xf0M\x83\xd1%]\xc5\x1a\xdd\xd3\xb7Z+\x1a\xe0xE\x04\xdfT:\xf8\x96\x9b\xe3\xeap\x82\xff\x7f\xc9\x88\x8c\x14M\xa2\xafXB\x9e\xc9`1\xdb\xca\xd3\xda\xd9\xaf\r\xcb\xaa\xaf&\x8c\xb8\xfc\xff\xea\xd9O<|\xa4\x95\xe0V\xa9\xb4z\xcd\xb7Q\xfbs\xe6f\xc6\xc6U\xad\xe8)r\x97\xd0z\xd1\xba^C\xf1\xbc\xa3#\x01e\x139\xe2)\x04\xcc\x8cB\xf5\x8c0\xc0J\xaf\xdb\x03\x8d\xda\x08G\xdd\x98\x8d\xcd\xa6\xf3\xbf\xd1\\KLE%\x00J\xa0n\xef\xf8\xcaax:\xac\xecW\xfb=\x1f\x92\xb0\xfe/\xd1\xa8_g$Q{e\xe6\x14\xadh\x08\xd6\xf6\xee4\xdf\xf71\x0f\xdc\x82\xae\xbf\xd9\x04\xb0\x1e\x1d\xc5K)\'\tK-\xb6\x8do\x90;h@\x1a\xde\xbfZ~\x08\xd7\x8f\xf4\xef]ce:e\x04\x0c\xf9\xbf\xd4\xac\xa7\x98Jt\xd3qE\x98g\x80\xfc\x0b\x16\xacE\x16I\xdea\x88\xa7\xdb\xdf\x19\x1fd\xb5\xfc^*\xb4{W\xf7\xf7\'l\xd4\x19\xc1z<\xa8\xe1\xb99\xaeI\xe4\x88\xac\xbak\x96V\x10\xb5H\x01\t\xc8\xb1{\x80\xe1\xb7\xb7P\xdf\xc7Y\x8d]P\x11\xfd-\xccV\x00\xa3.\xf5\xb5*\x1e\xcc\x82\x0e0\x8a\xa3Br\x1a\xac\tC\xbff\x86\xb6K%y7e\x04\xcc\xc4\x93\xd9~j\xed?\xb0\xf9\xcdq\xa4=\xd4\x97\xf0\x1f\x17\xc0\xe2\xcb7\x97\xaa*/%fV\x16\x8elIj\xfc_\xb92F\xf6\xb1\x11c\x98\xa3F\xf1\xa6A\xf3\xb0A\xe9\x89\xf7\x91O\x90\xcc,\x7f\xff5xv\xe5\x06\xb5\r3K\xa7|"[\xc3\x07\xbaSqR\xf3\xf1a\x0eN\xaf\xe5\x95\xf6\xd9\xd9\r\x11\xfa\xa93\xa1^\xf16\x95F\x86\x8a\x7f:E\xa9gh\xd4\x0f\xd9\xd04\x12\xc0\x91\xc61\\\xf4\xfd\xe7\xcbh`i78\r\xb2\xea\xaap{LA\x85\xc3.\xdd\xcd\xd3\x06p^M\xc1\xff\xc8r\xee\xeeGZd\xdf\xac\x86\xab\xa4\x1c\x06\x18\x98?\x87A\xc5\xefh\xd3\xa1\x01\xe8\xa3\xb8\xca\xc6\x0c\x90\\\x15\xfc\x91\x08@\xb9L\x00\xa0\xb9\xd0')
delta = time.ticks_diff(time.ticks_ms(), start)  # compute time difference
print('Computation time: %d ms' % delta)
if hexlify(signature) != b'0aab4c900501b3e24d7cdf4663326a3a87df5e4843b2cbdb67cbf6e460fec350aa5371b1508f9f4528ecea23c436d94b5e8fcd4f681e30a6ac00a9704a188a03':
    print('Test 4 failed.')
    print(hexlify(signature))
else:
    print('Test 4 passed.')
print()

# Test 5
print('Test 5: Length of message: 64 bytes')
secret = unhexlify(b'833fe62409237b9d62ec77587520911e9a759cec1d19755b7da901b96dca3d42')
start = time.ticks_ms()  # get millisecond counter
signature = sign(secret, b"\xdd\xaf5\xa1\x93az\xba\xccAsI\xae A1\x12\xe6\xfaN\x89\xa9~\xa2\n\x9e\xee\xe6KU\xd3\x9a!\x92\x99*'O\xc1\xa86\xba<#\xa3\xfe\xeb\xbdEMD#d<\xe8\x0e*\x9a\xc9O\xa5L\xa4\x9f")
delta = time.ticks_diff(time.ticks_ms(), start)  # compute time difference
print('Computation time: %d ms' % delta)
if hexlify(signature) != b'dc2a4459e7369633a52b1bf277839a00201009a3efbf3ecb69bea2186c26b58909351fc9ac90b3ecfdfbc7c66431e0303dca179c138ac17ad9bef1177331a704':
    print('Test 5 failed.')
    print(hexlify(signature))
else:
    print('Test 5 passed.')
print()
