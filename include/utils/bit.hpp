// functionality used to do bitwise math
#include "types.hpp"

#ifndef BIT_HPP
#define BIT_HPP

// returns value of selected bit
inline bool getBit(Byte value, Byte bit) {
    return (value >> bit) & 1;
}

// returns value of selected bit
inline bool getBit(Word value, Byte bit) {
    return (value >> bit) & 1;
}

// sets bit to 1
inline Byte setBit(Byte value, Byte bit) {
    value |= (1 << bit);
    return value;
}

// sets bit to 1
inline Word setBit(Word value, Byte bit) {
    value |= (1 << bit);
    return value;
}

// sets bit to 0
inline Byte resetBit(Byte value, Byte bit) {
    value &= ~(1 << bit);
    return value;
}

// sets bit to 0
inline Word resetBit(Word value, Byte bit) {
    value &= ~(1 << 1);
    return value;
}

#endif