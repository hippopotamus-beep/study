#ifndef PAYOFF_H
#define PAYOFF_H

#include <memory>

class Payoff
{
public:
    virtual ~Payoff() = default;

    virtual double payoff(double price) const = 0;
    virtual std::unique_ptr<Payoff> clone() const = 0;
};

class CallPayoff final : public Payoff{
public:
    CallPayoff(double strike);

    double payoff(double spot) const override;
    std::unique_ptr<Payoff> clone() const override;

private:
    double strike_;
};

class PullPayoff final : public Payoff{
public:
    PullPayoff(double strike);

    double payoff(double spot) const override;
    std::unique_ptr<Payoff> clone() const override;

private:
    double strike_;
};

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

class MCOptionValuation{
public:
    MCOptionValuation(OptionInfo&& opt, double vol, double int_rate, int time_steps,
                      double div_rate);

    double calc_prive(double spot, int unif_start_seed, int num_scenarios);

private:
    OptionInfo opt_;
};


#endif // PAYOFF_H
