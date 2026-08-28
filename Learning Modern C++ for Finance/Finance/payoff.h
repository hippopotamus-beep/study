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




#endif // PAYOFF_H
