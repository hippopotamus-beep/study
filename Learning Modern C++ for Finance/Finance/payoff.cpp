#include <algorithm>

#include "payoff.h"


//CallPayoff
CallPayoff::CallPayoff(double strike) : strike_{strike}{}


double CallPayoff::payoff(double spot) const{
    return std::max(spot - strike_, 0.0);
}

std::unique_ptr<Payoff> CallPayoff::clone() const{
    return std::make_unique<CallPayoff>(*this);
}


//PullPayoff
PullPayoff::PullPayoff(double strike) : strike_{strike}{}


double PullPayoff::payoff(double spot) const{
    return std::max(spot - strike_, 0.0);
}

std::unique_ptr<Payoff> PullPayoff::clone() const{
    return std::make_unique<PullPayoff>(*this);
}




