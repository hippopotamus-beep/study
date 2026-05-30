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

//OptionInfo
OptionInfo::OptionInfo(std::unique_ptr<Payoff> payoff, double time_to_exp):
    payoff_ptr_{std::move(payoff)}, time_to_exp_{time_to_exp}{}

OptionInfo::OptionInfo(const OptionInfo& rhs):
    payoff_ptr_{rhs.payoff_ptr_->clone()}, time_to_exp_{rhs.time_to_expiration()}{}

OptionInfo& OptionInfo::operator =(const OptionInfo& rhs){
    OptionInfo{rhs}.swap(*this);
    return *this;
}

double OptionInfo::option_payoff(double spot) const{
    return payoff_ptr_->payoff(spot);
}

double OptionInfo::time_to_expiration()const{
    return time_to_exp_;
}

void OptionInfo::swap(OptionInfo& rhs) noexcept{
    using std::swap;
    swap(payoff_ptr_, rhs.payoff_ptr_);
    swap(time_to_exp_, rhs.time_to_exp_);
}


