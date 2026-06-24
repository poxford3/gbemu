// functionality used to do bitwise math
#include "types.hpp"

// returns value of selected bit
bool getBit(Byte value, Byte bit) {
    return (value >> bit) & 1;
}

// returns value of selected bit
bool getBit(Word value, Byte bit) {
    return (value >> bit) & 1;
}

// sets bit to 1
Byte setBit(Byte value, Byte bit) {
    value |= (1 << bit);
    return value;
}

// sets bit to 1
Word setBit(Word value, Byte bit) {
    value |= (1 << bit);
    return value;
}

// sets bit to 0
Byte resetBit(Byte value, Byte bit) {
    value &= ~(1 << 1);
    return value;
}

// sets bit to 0
Word resetBit(Word value, Byte bit) {
    value &= ~(1 << 1);
    return value;
}