#ifndef BLACKSCHOLES_H
#define BLACKSCHOLES_H

#include <array>
#include <cmath>
#include <format>
#include <map>

using namespace std;

enum class PayoffType{
    Call = 1,
    Put = -1
};

enum class RiskValues{
    Delta,
    Gamma,
    Vega,
    Rho,
    Theta
};

class BlackScholes{
public:
    BlackScholes(double strike, double spot, double time_to_exp, PayoffType payoff_type,
                 double rate, double div = 0.0);

    double operator()(double vol) const;
    map<RiskValues, double> risk_values(double vol);


private:
    array<double, 2> compute_norm_args(double vol) const;
    double norm_cdf_(double x) const;

    double strike_, spot_, time_to_exp_;
    PayoffType payoff_type_;
    double rate_, div_;
};

double implied_volatility_with_lambda(const BlackScholes& bsc, double opt_mkt_price,
                                      double x0, double x1, double tol,
                                      unsigned max_iter);

#endif // BLACKSCHOLES_H
