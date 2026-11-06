#include "BitFlipper.h"

using namespace std;

BitFlipper::BitFlipper(unsigned int seed) : rng(seed) {
}

int BitFlipper::getBit(int hashValue) {
    if (bits.count(hashValue) > 0) {
        return bits[hashValue];
    }
    uniform_int_distribution<int> coin(0, 1);
    int bit = coin(rng);
    bits[hashValue] = bit;
    return bit;
}
