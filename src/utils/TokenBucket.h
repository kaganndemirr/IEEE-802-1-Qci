/*
 * TokenBucket.h
 *
 */

#ifndef UTILS_TOKENBUCKET_H_
#define UTILS_TOKENBUCKET_H_

class TokenBucket {
private:
    double pCapacity;
    double pTokens;

public:
    TokenBucket(double capacity, double tokens);
    virtual ~TokenBucket();

    void setCapacity(double capacity);
    double getCapacity();
    double getTokens();

    double add(double tokens);
    double remove(double tokens);
};

#endif /* UTILS_TOKENBUCKET_H_ */
