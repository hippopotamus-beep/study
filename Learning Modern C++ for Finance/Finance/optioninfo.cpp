#include "optioninfo.h"

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
