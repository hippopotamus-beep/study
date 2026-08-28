#ifndef OPTIONINFO_H
#define OPTIONINFO_H

#include "payoff.h"

class OptionInfo{
public:
    OptionInfo(std::unique_ptr<Payoff> payoff, double time_to_exp);
    OptionInfo(const OptionInfo& rhs);
    OptionInfo& operator =(const OptionInfo& rhs);
    OptionInfo(OptionInfo&& rhs) = default;
    OptionInfo& operator = (OptionInfo&& rhs) = default;
    ~OptionInfo() = default;

    double option_payoff(double spot) const;
    double time_to_expiration() const;
    void swap(OptionInfo& rhs) noexcept;

private:
    std::unique_ptr<Payoff> payoff_ptr_;
    double time_to_exp_;
};

#endif // OPTIONINFO_H
