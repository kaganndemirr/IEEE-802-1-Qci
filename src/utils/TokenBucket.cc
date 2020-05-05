/*
 * TokenBucket.cpp
 *
 */

#include "TokenBucket.h"

TokenBucket::TokenBucket(double capacity, double tokens) : pCapacity(capacity), pTokens(tokens) {
}

TokenBucket::~TokenBucket() {
}

void TokenBucket::setCapacity(double capacity) {
    if (capacity >= 0) {
        pCapacity = capacity;
    }
}
double TokenBucket::getCapacity() {
    return pCapacity;
}
double TokenBucket::getTokens() {
    return pTokens;
}

// @returns Number of overflowing tokens or zero on success
double TokenBucket::add(double tokens) {
    double overflow = pTokens + tokens - pCapacity;

    // Tokens doesn't overflow the bucket
    if (overflow <= 0) {
        pTokens += tokens;
        return 0;
    }

    // Bucket hasn't reached to the capacity
    if (overflow < tokens) {
        pTokens = pCapacity;
    }

    return overflow;
}
// @returns Number of tokens that must be added to the bucket for fulfilling the removal or zero on success
double TokenBucket::remove(double tokens) {
    double remaining = pTokens - tokens;

    if (remaining >= 0) {
        pTokens = remaining;
        return 0;
    }

    return -remaining;
}
