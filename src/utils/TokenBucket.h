/*
 * TokenBucket.h
 *
 */

#ifndef UTILS_TOKENBUCKET_H_
#define UTILS_TOKENBUCKET_H_

#include <ostream>
#include <iomanip>

class TokenBucket {
private:
    double pCapacity;
    double pTokens;

public:
    TokenBucket(double capacity, double tokens);
    virtual ~TokenBucket();

    double setCapacity(double capacity);
    double getCapacity();
    double getTokens();

    double add(double tokens);
    double remove(double tokens);

    friend std::ostream& operator<<(std::ostream& os, const TokenBucket& bucket)
    {
        os << std::fixed << std::setprecision(2) << "capacity=" << bucket.pCapacity << "  tokens=" << bucket.pTokens; // no endl!
        return os;
    }
};

#endif /* UTILS_TOKENBUCKET_H_ */
