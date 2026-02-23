#include "md5.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>

namespace hashface {

// Constants for MD5Transform routine
static const uint32_t S11 = 7, S12 = 12, S13 = 17, S14 = 22;
static const uint32_t S21 = 5, S22 = 9,  S23 = 14, S24 = 20;
static const uint32_t S31 = 4, S32 = 11, S33 = 16, S34 = 23;
static const uint32_t S41 = 6, S42 = 10, S43 = 15, S44 = 21;

// F, G, H and I are basic MD5 functions
static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) | (~x & z);
}
static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
    return (x & z) | (y & ~z);
}
static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
    return x ^ y ^ z;
}
static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
    return y ^ (x | ~z);
}

// ROTATE_LEFT rotates x left n bits
static inline uint32_t ROTATE_LEFT(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

// FF, GG, HH, and II transformations
static inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = ROTATE_LEFT(a + F(b, c, d) + x + ac, s) + b;
}
static inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = ROTATE_LEFT(a + G(b, c, d) + x + ac, s) + b;
}
static inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = ROTATE_LEFT(a + H(b, c, d) + x + ac, s) + b;
}
static inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
    a = ROTATE_LEFT(a + I(b, c, d) + x + ac, s) + b;
}

MD5::MD5() : finalized_(false) {
    // Initialize with magic numbers
    state_[0] = 0x67452301;
    state_[1] = 0xefcdab89;
    state_[2] = 0x98badcfe;
    state_[3] = 0x10325476;
    count_[0] = 0;
    count_[1] = 0;
    std::memset(buffer_, 0, sizeof(buffer_));
}

void MD5::update(const uint8_t* input, size_t inputLen) {
    size_t i, index, partLen;
    
    // Compute number of bytes mod 64
    index = (count_[0] >> 3) & 0x3F;
    
    // Update number of bits
    if ((count_[0] += (inputLen << 3)) < (inputLen << 3)) {
        count_[1]++;
    }
    count_[1] += (inputLen >> 29);
    
    partLen = 64 - index;
    
    // Transform as many times as possible
    if (inputLen >= partLen) {
        std::memcpy(&buffer_[index], input, partLen);
        transform(buffer_);
        
        for (i = partLen; i + 63 < inputLen; i += 64) {
            transform(&input[i]);
        }
        
        index = 0;
    } else {
        i = 0;
    }
    
    // Buffer remaining input
    std::memcpy(&buffer_[index], &input[i], inputLen - i);
}

void MD5::update(const std::string& str) {
    update(reinterpret_cast<const uint8_t*>(str.c_str()), str.size());
}

std::vector<uint8_t> MD5::finalize() {
    static const uint8_t PADDING[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    if (finalized_) {
        return std::vector<uint8_t>(16);
    }
    
    uint8_t bits[8];
    encode(bits, count_, 8);
    
    // Pad out to 56 mod 64
    size_t index = (count_[0] >> 3) & 0x3f;
    size_t padLen = (index < 56) ? (56 - index) : (120 - index);
    update(PADDING, padLen);
    
    // Append length (before padding)
    update(bits, 8);
    
    // Store state in digest
    std::vector<uint8_t> digest(16);
    encode(digest.data(), state_, 16);
    
    finalized_ = true;
    return digest;
}

void MD5::transform(const uint8_t block[64]) {
    uint32_t a = state_[0], b = state_[1], c = state_[2], d = state_[3];
    uint32_t x[16];
    
    decode(x, block, 64);
    
    // Round 1
    FF(a, b, c, d, x[0], S11, 0xd76aa478);
    FF(d, a, b, c, x[1], S12, 0xe8c7b756);
    FF(c, d, a, b, x[2], S13, 0x242070db);
    FF(b, c, d, a, x[3], S14, 0xc1bdceee);
    FF(a, b, c, d, x[4], S11, 0xf57c0faf);
    FF(d, a, b, c, x[5], S12, 0x4787c62a);
    FF(c, d, a, b, x[6], S13, 0xa8304613);
    FF(b, c, d, a, x[7], S14, 0xfd469501);
    FF(a, b, c, d, x[8], S11, 0x698098d8);
    FF(d, a, b, c, x[9], S12, 0x8b44f7af);
    FF(c, d, a, b, x[10], S13, 0xffff5bb1);
    FF(b, c, d, a, x[11], S14, 0x895cd7be);
    FF(a, b, c, d, x[12], S11, 0x6b901122);
    FF(d, a, b, c, x[13], S12, 0xfd987193);
    FF(c, d, a, b, x[14], S13, 0xa679438e);
    FF(b, c, d, a, x[15], S14, 0x49b40821);
    
    // Round 2
    GG(a, b, c, d, x[1], S21, 0xf61e2562);
    GG(d, a, b, c, x[6], S22, 0xc040b340);
    GG(c, d, a, b, x[11], S23, 0x265e5a51);
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
    GG(a, b, c, d, x[5], S21, 0xd62f105d);
    GG(d, a, b, c, x[10], S22, 0x02441453);
    GG(c, d, a, b, x[15], S23, 0xd8a1e681);
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
    GG(a, b, c, d, x[9], S21, 0x21e1cde6);
    GG(d, a, b, c, x[14], S22, 0xc33707d6);
    GG(c, d, a, b, x[3], S23, 0xf4d50d87);
    GG(b, c, d, a, x[8], S24, 0x455a14ed);
    GG(a, b, c, d, x[13], S21, 0xa9e3e905);
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
    GG(c, d, a, b, x[7], S23, 0x676f02d9);
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);
    
    // Round 3
    HH(a, b, c, d, x[5], S31, 0xfffa3942);
    HH(d, a, b, c, x[8], S32, 0x8771f681);
    HH(c, d, a, b, x[11], S33, 0x6d9d6122);
    HH(b, c, d, a, x[14], S34, 0xfde5380c);
    HH(a, b, c, d, x[1], S31, 0xa4beea44);
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
    HH(b, c, d, a, x[10], S34, 0xbebfbc70);
    HH(a, b, c, d, x[13], S31, 0x289b7ec6);
    HH(d, a, b, c, x[0], S32, 0xeaa127fa);
    HH(c, d, a, b, x[3], S33, 0xd4ef3085);
    HH(b, c, d, a, x[6], S34, 0x04881d05);
    HH(a, b, c, d, x[9], S31, 0xd9d4d039);
    HH(d, a, b, c, x[12], S32, 0xe6db99e5);
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
    HH(b, c, d, a, x[2], S34, 0xc4ac5665);
    
    // Round 4
    II(a, b, c, d, x[0], S41, 0xf4292244);
    II(d, a, b, c, x[7], S42, 0x432aff97);
    II(c, d, a, b, x[14], S43, 0xab9423a7);
    II(b, c, d, a, x[5], S44, 0xfc93a039);
    II(a, b, c, d, x[12], S41, 0x655b59c3);
    II(d, a, b, c, x[3], S42, 0x8f0ccc92);
    II(c, d, a, b, x[10], S43, 0xffeff47d);
    II(b, c, d, a, x[1], S44, 0x85845dd1);
    II(a, b, c, d, x[8], S41, 0x6fa87e4f);
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
    II(c, d, a, b, x[6], S43, 0xa3014314);
    II(b, c, d, a, x[13], S44, 0x4e0811a1);
    II(a, b, c, d, x[4], S41, 0xf7537e82);
    II(d, a, b, c, x[11], S42, 0xbd3af235);
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
    II(b, c, d, a, x[9], S44, 0xeb86d391);
    
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    
    // Zeroize sensitive information
    std::memset(x, 0, sizeof(x));
}

void MD5::encode(uint8_t* output, const uint32_t* input, size_t len) {
    for (size_t i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = input[i] & 0xff;
        output[j + 1] = (input[i] >> 8) & 0xff;
        output[j + 2] = (input[i] >> 16) & 0xff;
        output[j + 3] = (input[i] >> 24) & 0xff;
    }
}

void MD5::decode(uint32_t* output, const uint8_t* input, size_t len) {
    for (size_t i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = static_cast<uint32_t>(input[j]) |
                    (static_cast<uint32_t>(input[j + 1]) << 8) |
                    (static_cast<uint32_t>(input[j + 2]) << 16) |
                    (static_cast<uint32_t>(input[j + 3]) << 24);
    }
}

std::vector<uint8_t> MD5::hash(const std::string& input) {
    MD5 md5;
    md5.update(input);
    return md5.finalize();
}

std::string MD5::to_hex(const std::vector<uint8_t>& hash) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    
    for (uint8_t byte : hash) {
        oss << std::setw(2) << static_cast<int>(byte);
    }
    
    return oss.str();
}

} // namespace hashface